#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
radio_hw_host.py — FR9180 radio_hw SPI 测试上位机

通过 STM32F407 的 USART1 (115200-8-N-1) 与 radio_hw SPI 驱动通信。
命令协议见 Core/Src/main.c 的 radio_hw_console_poll()：

    cmd [hex]   写 arm_cmd_0 = 0x80000000 | payload，bit31 触发 ZC706 PS 中断
    read <hex>  读寄存器（字节偏移地址，十六进制）
    test        重跑 radio_hw_spi_self_test()
    help        打印固件命令帮助

用法：
    python radio_hw_host.py --list           列出串口
    python radio_hw_host.py COM3             交互模式
    python radio_hw_host.py COM3 --test      自动冒烟测试
    python radio_hw_host.py COM3 --baud N    指定波特率

依赖：pyserial  (pip install pyserial)
"""

import argparse
import re
import sys
import time

try:
    import serial
except ImportError:
    sys.exit("缺少 pyserial，请先安装：pip install pyserial")

# 寄存器表（字节地址, 复位值），与 pcore_registers.v / Core/Inc/radio_hw_spi.h 一致。
ALL_REGISTERS = [
    ("SRR",           0x0000, 0x00000000),
    ("SPICR",         0x0004, 0x00000180),
    ("SPISR",         0x0008, 0x000000A5),
    ("device_config", 0x000C, 0x00001234),
    ("chip_type",     0x0010, 0x00000001),
    ("product_id",    0x0014, 0x00000001),
    ("chip_grade",    0x0018, 0x0000AAAA),
    ("scratch_pad",   0x001C, 0x0000FFFF),
    ("vendor_id",     0x0020, 0x0000ABCD),
]
ALL_REGISTERS += [(f"arm_cmd_{i}",    0x0030 + 4 * i, 0x00000000) for i in range(8)]
ALL_REGISTERS += [(f"arm_status_{i}", 0x0050 + 4 * i, 0x00000000) for i in range(8)]

# 只读寄存器（用于链路自检）
RO_REGISTERS = [r for r in ALL_REGISTERS
                if r[0] in ("SPISR", "device_config", "chip_type",
                            "product_id", "chip_grade", "vendor_id")]


def list_ports():
    from serial.tools import list_ports as lp
    ports = list(lp.comports())
    if not ports:
        print("未发现串口。")
        return
    print("可用串口：")
    for p in ports:
        print(f"  {p.device}   {p.description}")


def open_port(port, baud):
    try:
        ser = serial.Serial(port, baud, bytesize=8, parity="N", stopbits=1,
                            timeout=0.5)
    except Exception as e:
        sys.exit(f"无法打开串口 {port}: {e}")
    print(f"已连接 {port} @ {baud} 8N1")
    return ser


def read_response(ser, idle=0.25, total=3.0):
    """读取设备输出，直到静默 idle 秒或达到 total 秒。"""
    ser.timeout = 0.05
    lines = []
    last = time.time()
    start = time.time()
    while True:
        raw = ser.readline()
        if raw:
            last = time.time()
            text = raw.decode(errors="replace").rstrip("\r\n")
            if text:
                lines.append(text)
        elif time.time() - last >= idle:
            break
        if time.time() - start >= total:
            break
    return lines


def transact(ser, line, idle=0.25, total=3.0):
    """发送一行命令，返回响应（去掉回显的命令行）。"""
    ser.reset_input_buffer()
    ser.write((line + "\n").encode())
    ser.flush()
    lines = read_response(ser, idle=idle, total=total)
    if lines and lines[0].strip() == line.strip():
        lines = lines[1:]
    return lines


def parse_value(line):
    m = re.search(r"=\s*0x([0-9A-Fa-f]+)", line)
    return int(m.group(1), 16) if m else None


def read_reg(ser, addr):
    lines = transact(ser, f"read 0x{addr:x}")
    for line in lines:
        v = parse_value(line)
        if v is not None:
            return v
    return None


def dump(ser):
    print(f"{'register':<14} {'addr':>8} {'value':>10}   note")
    for name, addr, reset in ALL_REGISTERS:
        v = read_reg(ser, addr)
        if v is None:
            print(f"{name:<14} 0x{addr:04x}   <error>")
            continue
        note = "" if v == reset else "  <-- differs from reset"
        print(f"{name:<14} 0x{addr:04x} 0x{v:08x}{note}")


def auto_test(ser):
    ok = True

    print("== 1/3 只读寄存器链路检查 ==")
    for name, addr, reset in RO_REGISTERS:
        v = read_reg(ser, addr)
        if v is None:
            print(f"  [FAIL] {name}: 读取失败")
            ok = False
        elif v != reset:
            print(f"  [FAIL] {name}: read 0x{v:08X}, expected 0x{reset:08X}")
            ok = False
        else:
            print(f"  [ OK ] {name} = 0x{v:08X}")

    print("== 2/3 固件自测 (test) ==")
    lines = transact(ser, "test", total=6.0)
    passed = any("passed" in l.lower() for l in lines)
    for l in lines:
        print("  " + l)
    if not passed:
        print("  [FAIL] 自测未输出 'passed'")
        ok = False

    print("== 3/3 触发 ZC706 PS 中断（连发 3 次 cmd）==")
    for i in range(3):
        lines = transact(ser, f"cmd 0x{0xA0 + i:x}")
        print("  " + (lines[0] if lines else "<无响应>"))
        time.sleep(0.1)

    print()
    print("RESULT:", "PASS" if ok else "FAIL")
    return 0 if ok else 1


def interactive(ser):
    print("=" * 62)
    print("radio_hw host console")
    print("  固件命令:  cmd [hex] / read <hex> / test / help")
    print("  上位机命令: dump (读全部寄存器) / exit / quit")
    print("  提示: 若无回显，检查波特率/接线")
    print("=" * 62)
    while True:
        try:
            user = input("> ").strip()
        except (EOFError, KeyboardInterrupt):
            print()
            break
        if not user:
            continue
        low = user.lower()
        if low in ("exit", "quit"):
            break
        if low == "dump":
            dump(ser)
            continue
        lines = transact(ser, user)
        if not lines:
            print("  <无响应>")
        for line in lines:
            print(line)


def main():
    parser = argparse.ArgumentParser(description="FR9180 radio_hw SPI 测试上位机")
    parser.add_argument("port", nargs="?", help="串口名，如 COM3 或 /dev/ttyUSB0")
    parser.add_argument("--baud", type=int, default=115200, help="波特率，默认 115200")
    parser.add_argument("--list", action="store_true", help="列出可用串口")
    parser.add_argument("--test", action="store_true", help="运行自动冒烟测试")
    args = parser.parse_args()

    if args.list:
        list_ports()
        return 0

    if args.port is None:
        list_ports()
        print("\n请指定串口，例如: python radio_hw_host.py COM3")
        return 1

    ser = open_port(args.port, args.baud)
    try:
        if args.test:
            return auto_test(ser)
        return interactive(ser)
    finally:
        ser.close()


if __name__ == "__main__":
    sys.exit(main())
