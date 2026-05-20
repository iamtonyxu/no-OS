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

## 2. READY Heartbeat

After initialization, target sends:
- ASCII string: `READY\n`
- Period: every `1s`

Host must wait for `READY` before any action.
If timeout expires before `READY`, host exits with error.

## 3. Command Overview

Single-byte command channel from host to target:
- `G`: trigger capture and stream sampled IQ text lines
- `S`: configure TX source selector with one trailing parameter byte
- `W`: start waveform-download frame parsing
- `E`: exit command loop (test usage)

## 4. TX Source Configuration Protocol (`S`)

### 4.1 On-wire Layout

Host sends in this order:

1. `S` (1 byte)
2. `srcSel` (1 byte)

### 4.2 Field Definition

- `srcSel`
  - `0`: FPGA DDS
  - `1`: DDR playback
  - `2`: constant data

Default after target init:
- `srcSel = 0`

### 4.3 Target Processing Sequence

1. Read one `srcSel` byte
2. Validate range `0..2`
3. Write `OFFSET_SRC_SEL`
4. Leave all other `fr9009_config` fields and hardware registers unchanged
5. Reply status line

### 4.4 Target Replies

Success:
- `SRC:OK src_sel=<srcSel>`

Error:
- `SRC:ERR:TIMEOUT`
- `SRC:ERR:VALUE`
- `SRC:ERR:WRITE`

## 5. Waveform Download Protocol (`W`)

### 5.1 On-wire Frame Layout

Host sends in this order:

1. `W` (1 byte)
2. Magic `F09` (3 bytes): `0x46 0x30 0x39`
3. `rateSel` (1 byte)
4. `sampleCount` (`uint16`, 2 bytes, little-endian)
5. `payload` (`sampleCount` words, each `uint32`, little-endian)
6. `checksum` (`uint32`, 4 bytes, little-endian)

Total frame length:

`1 + 3 + 1 + 2 + 4 * sampleCount + 4`

### 5.2 Field Definition

- `rateSel`
  - `0`: use `tone_lut_iq_491M`
  - `1`: use `tone_lut_iq_245M`

- `sampleCount`
  - Unit: number of waveform words (`uint32`), not bytes
  - Valid range: `1 .. TX_BUF_LEN` (current `TX_BUF_LEN = 16384`)

- `payload[i]`
  - Word format: `[31:16]=Q`, `[15:0]=I`
  - Each word transported in little-endian byte order

- `checksum`
  - Algorithm: `uint32` sum of all payload words, modulo `2^32`

## 6. Target Processing Sequence (`W`)

1. Read `F09 + rateSel + sampleCount`
2. Validate magic, rate, and length
3. Read `sampleCount` payload words into target LUT
4. Compute checksum while receiving payload
5. Zero-fill LUT tail from `sampleCount` to `TX_BUF_LEN-1`
6. Read and validate checksum field
7. Copy full LUT (`TX_BUF_LEN * 4` bytes) to DDR
8. Reply status line

## 7. Target Replies

Success:
- `WF:OK rate=<rateSel> samples=<sampleCount> checksum=0x<...>`

Error:
- `WF:ERR:TIMEOUT_HDR`
- `WF:ERR:MAGIC`
- `WF:ERR:RATE`
- `WF:ERR:LEN`
- `WF:ERR:TIMEOUT_PAYLOAD`
- `WF:ERR:TIMEOUT_CKS`
- `WF:ERR:CKS`

## 8. Capture Readback Protocol (`G`)

- Host sends `G`
- Target prints marker line:
  - `cap_buffer=`
- Then prints IQ text lines:
  - `<I> <Q>`
- Host parser extracts two signed integers per line and rebuilds complex samples

## 9. Host-side Mapping Rules

- Host decides `rateSel` from waveform sample rate:
  - `Fs > 300e6` -> `rateSel = 0` (491.52M path)
  - otherwise -> `rateSel = 1` (245.76M path)

## 10. Example (Hex, Conceptual)

Assume:
- `rateSel = 0`
- `sampleCount = 3`
- payload words:
  - `0x00010002`
  - `0x00030004`
  - `0x00050006`
- checksum = `0x0009000C`

Then byte stream is:

- `57`                              (`W`)
- `46 30 39`                        (`F09`)
- `00`                              (`rateSel`)
- `03 00`                           (`sampleCount`, LE)
- `02 00 01 00`                     (word0, LE)
- `04 00 03 00`                     (word1, LE)
- `06 00 05 00`                     (word2, LE)
- `0C 00 09 00`                     (`checksum`, LE)

## 11. Notes

- Current target loop sends READY every second and then checks RX.
- This means command handling granularity is approximately 1 second.
- If lower command latency is needed, use non-blocking periodic timing instead of fixed `mdelay(1000)`.
