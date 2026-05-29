# FR9009 UART Protocol (Host <-> Target)

This document describes the current UART protocol implemented by:
- Target: `projects/fr9009/src/main.c`
- Host: `projects/fr9009/matlab/test_top.m`

## 1. Link Basics

- Transport: UART
- Baud rate: `115200`
- Endianness for multibyte fields: little-endian
- Waveform word format (`uint32`):
  - bits `[31:16]`: `Q` (`int16`)
  - bits `[15:0]`: `I` (`int16`)

## 2. Initialization & Main Loop

After hardware init (`main_step`: clock, JESD, RF), the target loads a default built-in waveform into DDR depending on `JESD_MODE`:
- `JESD_MODE=0`: loads `tone_lut_iq_491M` (DAC=491.52M)
- `JESD_MODE=1`: loads `tone_lut_iq_245M` (DAC=245.76M)

Then the target prints a prompt and enters a blocking command loop:

```
Enter 'C' to configure tx source, 'G' to read capture buffer, 'D' to download tx waveform, 'W0' for 491M tx waveform, 'W1' for 245M tx waveform, 'E' to exit...
```

No periodic heartbeat. Each command byte is read with a blocking UART receive.

## 3. Command Overview

Single-byte command channel from host to target (case-insensitive for `C`/`D`/`W`):

| Command | Description |
|---|---|
| `C` / `c` | Configure TX source and associated registers |
| `D` / `d` | Download waveform payload to DDR |
| `W` / `w` | Load built-in waveform table to DDR |
| `G` | Trigger capture and print IQ samples (dual-channel) |
| `E` | Exit command loop |

## 4. TX Source Configuration Protocol (`C`)

### 4.1 On-wire Layout

Host sends in this order:

1. `C` or `c` (1 byte)
2. `srcSel` (1 byte): `0`=DDS, `1`=DDR, `2`=const data
3. Payload depends on `srcSel`:

**srcSel=0 (FPGA DDS):**
| Field | Bytes | Type | Description |
|---|---|---|---|
| `dds_pinc_0` | 4 | `uint32` LE | DDS phase increment for tone 1 (30-bit) |
| `dds_poff_0` | 4 | `uint32` LE | DDS phase offset for tone 1 |
| `dds_pinc_1` | 4 | `uint32` LE | DDS phase increment for tone 2 (30-bit) |
| `dds_poff_1` | 4 | `uint32` LE | DDS phase offset for tone 2 |

Total: `1 + 1 + 16 = 18 bytes`

**srcSel=1 (DDR playback):**
No extra payload.

Total: `1 + 1 = 2 bytes`

**srcSel=2 (DC constant data):**
| Field | Bytes | Type | Description |
|---|---|---|---|
| `const_data_0` | 4 | `uint32` LE | Constant I/Q word for channel 0 |
| `const_data_1` | 4 | `uint32` LE | Constant I/Q word for channel 1 |

Total: `1 + 1 + 8 = 10 bytes`

### 4.2 Target Processing (FR9009_DEVICE path)

When `FR9009_DEVICE=1` (using `fr9009_config_t`):
- DDS mode: the 30-bit `dds_pinc` values are converted to 16-bit frequency words via `round(dds_pinc / 2^14)`. `dds_poff` fields are discarded (no matching field in `fr9009_config_t`). Sets `ddr_play_ctrl=0`, `dds_sync=1`.
- DDR mode: sets `ddr_play_ctrl=1`.
- Const mode: writes `const_data_0` and `const_data_1` directly. Sets `ddr_play_ctrl=0`.
- On success, calls `axi_fr9009_config_init()` to apply hardware registers.

### 4.3 Target Processing (non-FR9009_DEVICE path)

When `FR9009_DEVICE=0` (using `config8_reg_t`):
- DDS mode: writes all four fields (`dds_pinc_0`, `dds_poff_0`, `dds_pinc_1`, `dds_poff_1`) directly without conversion.
- DDR mode: no extra payload.
- Const mode: writes `const_data_0` and `const_data_1` directly.
- On success, calls `CONFIG_8_REG_init()` to apply hardware registers.

### 4.4 Target Replies

Success:
- `tx_config_ok src=<srcSel>`

Error:
- `tx_config_err_src=<srcSel>` (invalid `srcSel` value)
- `tx_config_err_apply src=<srcSel>` (hardware write failed)

## 5. Waveform Download Protocol (`D`)

### 5.1 On-wire Layout

Host sends in this order:

| Field | Bytes | Type | Description |
|---|---|---|---|
| `D` or `d` | 1 | char | Command byte |
| `word_count` | 4 | `uint32` LE | Number of waveform words to follow |
| `payload` | `word_count * 4` | `uint32[]` LE | Waveform words, each `[31:16]=Q, [15:0]=I` |

Total frame length: `1 + 4 + 4 * word_count`

### 5.2 Field Constraints

- `word_count`: valid range `1 .. TX_BUF_LEN` (current `TX_BUF_LEN = 16384`)
- Payload written directly to `TX_BUF_ADDR` (DDR base = `0x3F000000`)
- No magic bytes, no rate selector, no checksum

### 5.3 Target Processing Sequence

1. Read `word_count` (4 bytes, little-endian)
2. Validate `0 < word_count <= TX_BUF_LEN`
3. Read `word_count` payload words, writing each directly to DDR at `TX_BUF_ADDR`
4. Reply status line

### 5.4 Target Replies

Success:
- `waveform_download_ok words=<word_count>`

Error:
- `waveform_download_err_len=<word_count>` (zero or exceeds `TX_BUF_LEN`)

## 6. Built-in Waveform Loading (`W`)

### 6.1 On-wire Layout

Host sends:

1. `W` or `w` (1 byte)
2. `waveform_sel` (1 byte):
   - `0` (ASCII `'0'` = 0x30): load `tone_lut_iq_491M`
   - `1` (ASCII `'1'` = 0x31): load `tone_lut_iq_245M`

Total: `2 bytes`

### 6.2 Target Replies

Success:
- `tx_waveform=491M` (when `waveform_sel='0'`)
- `tx_waveform=245M` (when `waveform_sel='1'`)

Error (unrecognized selector):
- `Invalid tx waveform command: <char>`

## 7. Capture Readback Protocol (`G`)

### 7.1 Protocol

1. Host sends `G` (1 byte)
2. Target calls `trigger_capture()` to arm the RX capture
3. Target prints marker line: `cap_buffer=`
4. Target calls `read_capture(cap_buf, CAP_LENGTH_MAX)`
5. For each of `CAP_LENGTH_READ` sample indices (`CAP_LENGTH_READ = CAP_LENGTH_MAX / 4 = 4096`), target prints two lines:

```
<rx1_i> <rx1_q>
<rx2_i> <rx2_q>
```

### 7.2 Data Extraction (target-side)

From each 128-bit capture word (4 × `uint32` at `cap_buf[j]`..`cap_buf[j+3]` where `j = i * 4`):
- `rx1_i`, `rx1_q` extracted from `cap_buf[j]` (byte-level reordering of 16-bit fields)
- `rx2_i`, `rx2_q` extracted from `cap_buf[j+1]` (byte-level reordering of 16-bit fields)

Each value is a signed 16-bit integer printed in decimal.

### 7.3 Constants

| Constant | Value | Description |
|---|---|---|
| `CAP_LENGTH_MAX` | `16384` | Capture buffer size in `uint32` words |
| `CAP_LENGTH_READ` | `4096` | Number of sample indices printed (dual-channel) |

Total output: `CAP_LENGTH_READ * 2` lines of IQ pairs.

## 8. Host-side Mapping Rules

- Host decides DDS `rateSel` from waveform sample rate:
  - `Fs > 300e6` -> `JESD_MODE = 0` (491.52M DAC path)
  - otherwise -> `JESD_MODE = 1` (245.76M DAC path)

## 9. Example Waveform Download (Hex)

Assume `word_count = 3` with payload words:
- `0x00010002`
- `0x00030004`
- `0x00050006`

Byte stream:

```
44               ('D')
03 00 00 00       (word_count = 3, LE uint32)
02 00 01 00       (word 0, LE)
04 00 03 00       (word 1, LE)
06 00 05 00       (word 2, LE)
```

## 10. Compile-time Variants

Two preprocessor macros affect protocol behavior:

| Macro | Effect |
|---|---|
| `FR9009_DEVICE` | `1`: uses `fr9009_config_t` (16-bit DDS freq words, no phase offset). `0`: uses `config8_reg_t` (full 30-bit DDS fields, preserves phase offsets). |
| `JESD_MODE` | `0`: L=4 DAC=491.52M, loads `tone_lut_iq_491M` at boot. `1`: L=2 DAC=245.76M, loads `tone_lut_iq_245M` at boot. |
