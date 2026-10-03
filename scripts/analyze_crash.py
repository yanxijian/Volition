#!/usr/bin/env python3
"""Analyze a Windows crash dump (WER LocalDumps / MiniDump format).

Parses the minidump directly, then symbolicates addresses with dbghelp
against the binaries and PDBs recorded in the dump (paths embedded in the
binaries resolve when run on the machine that produced the dump). Works
best with full dumps (DumpType=2): those include stack memory, which
enables the return-address scan.

Usage:
  python scripts/analyze_crash.py <dump.dmp> [--all]   analyze one dump
  python scripts/analyze_crash.py                       newest dump of any
                                                        volition/mps process
  python scripts/analyze_crash.py --exe volition_host   newest dump of one exe

Dump location and symbol search path are environment-based, never hardcoded:
  %LOCALAPPDATA%\\CrashDumps     default WER LocalDumps folder (see below)
  _NT_SYMBOL_PATH                extra dbghelp symbol search path (optional)

One-time WER setup for full dumps (per-user registry, no admin required;
replace <exe> as needed, e.g. volition_host.exe):
  reg add "HKCU\\SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\<exe>" ^
      /v DumpType /t REG_DWORD /d 2 /f
  reg add "HKCU\\SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\<exe>" ^
      /v DumpCount /t REG_DWORD /d 10 /f
"""

from __future__ import annotations

import ctypes
import os
import struct
import sys

src = b""
streams: dict[int, list[tuple[int, int]]] = {}
mods: list[tuple[int, int, str]] = []  # (base, size, path)
mem_ranges: list[tuple[int, int, int]] = []  # (start, size, data offset in src)


def load(path: str) -> None:
    global src
    src = open(path, "rb").read()
    sig, _, nstreams, rva_dir = struct.unpack_from("<IIII", src, 0)
    if sig != 0x504D444D:
        raise SystemExit("not a minidump file")
    for i in range(nstreams):
        stype, size, rva = struct.unpack_from("<III", src, rva_dir + i * 12)
        streams.setdefault(stype, []).append((size, rva))
    # ModuleListStream = 4
    if 4 in streams:
        _, rva = streams[4][0]
        n = struct.unpack_from("<I", src, rva)[0]
        off = rva + 4
        for _ in range(n):
            base, msize, _chk, _ts, namerva = struct.unpack_from("<QIIII", src, off)
            nlen = struct.unpack_from("<I", src, namerva)[0]
            name = src[namerva + 4 : namerva + 4 + nlen].decode("utf-16-le")
            mods.append((base, msize, name))
            off += 108
    # Memory64ListStream = 9
    if 9 in streams:
        _, rva = streams[9][0]
        nranges, base_rva = struct.unpack_from("<QQ", src, rva)
        off = rva + 16
        cur = base_rva
        for _ in range(nranges):
            start, dsz = struct.unpack_from("<QQ", src, off)
            mem_ranges.append((start, dsz, cur))
            cur += dsz
            off += 16


def find_mod(addr: int):
    for base, size, path in mods:
        if base <= addr < base + size:
            return path, addr - base
    return None, 0


def read_mem(addr: int, n: int):
    for start, size, datoff in mem_ranges:
        if start <= addr and addr + n <= start + size:
            return src[datoff + (addr - start) : datoff + (addr - start) + n]
    return None


def exception_info():
    """-> (code, fault_addr, rip, rsp, tid) or None"""
    if 6 not in streams:
        return None
    _, rva = streams[6][0]
    tid, _ = struct.unpack_from("<II", src, rva)
    code, _n, _rec, fault = struct.unpack_from("<IIQQ", src, rva + 8)
    ctx_size, ctx_rva = struct.unpack_from("<II", src, rva + 8 + 152)
    rip = struct.unpack_from("<Q", src, ctx_rva + 0xF8)[0]
    rsp = struct.unpack_from("<Q", src, ctx_rva + 0x98)[0]
    return code, fault, rip, rsp, tid


# ---------------------------------------------------------------- dbghelp

class SYMBOL_INFOW(ctypes.Structure):
    _fields_ = [
        ("SizeOfStruct", ctypes.c_ulong),
        ("TypeIndex", ctypes.c_ulong),
        ("Reserved", ctypes.c_uint64 * 2),
        ("Index", ctypes.c_ulong),
        ("Size", ctypes.c_ulong),
        ("ModBase", ctypes.c_uint64),
        ("Flags", ctypes.c_ulong),
        ("Value", ctypes.c_uint64),
        ("Address", ctypes.c_uint64),
        ("Register", ctypes.c_ulong),
        ("Scope", ctypes.c_ulong),
        ("Tag", ctypes.c_ulong),
        ("NameLen", ctypes.c_ulong),
        ("MaxNameLen", ctypes.c_ulong),
        ("Name", ctypes.c_wchar * 2000),
    ]


class IMAGEHLP_LINEW64(ctypes.Structure):
    _fields_ = [
        ("SizeOfStruct", ctypes.c_ulong),
        ("Key", ctypes.c_void_p),
        ("LineNumber", ctypes.c_ulong),
        ("FileName", ctypes.c_void_p),
        ("Address", ctypes.c_uint64),
    ]


dbh = ctypes.WinDLL("dbghelp.dll")
k32 = ctypes.WinDLL("kernel32.dll")
_hproc = ctypes.c_void_p(k32.GetCurrentProcess())
_sym_ok = False

# Explicit signatures: module base addresses are 64-bit; ctypes' default
# 32-bit int conversion would overflow.
dbh.SymSetOptions.argtypes = [ctypes.c_uint32]
dbh.SymSetOptions.restype = ctypes.c_uint32
dbh.SymInitializeW.argtypes = [ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_int]
dbh.SymInitializeW.restype = ctypes.c_int
dbh.SymLoadModuleExW.argtypes = [
    ctypes.c_void_p, ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_wchar_p,
    ctypes.c_uint64, ctypes.c_uint32, ctypes.c_void_p, ctypes.c_uint32,
]
dbh.SymLoadModuleExW.restype = ctypes.c_uint64
dbh.SymFromAddrW.argtypes = [
    ctypes.c_void_p, ctypes.c_uint64,
    ctypes.POINTER(ctypes.c_uint64), ctypes.c_void_p,
]
dbh.SymFromAddrW.restype = ctypes.c_int
dbh.SymGetLineFromAddrW64.argtypes = [
    ctypes.c_void_p, ctypes.c_uint64,
    ctypes.POINTER(ctypes.c_uint32), ctypes.c_void_p,
]
dbh.SymGetLineFromAddrW64.restype = ctypes.c_int


def sym_init():
    global _sym_ok
    SYMOPT_UNDNAME = 0x2
    SYMOPT_LOAD_LINES = 0x10
    dbh.SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES)
    if not dbh.SymInitializeW(_hproc, None, False):
        return False
    for base, size, path in mods:
        low = path.lower()
        if "windows\\" in low and ("system32" in low or "syswow64" in low):
            continue  # skip system DLLs, no symbols available
        dbh.SymLoadModuleExW(_hproc, None, path, None, base, size, None, 0)
    _sym_ok = True
    return True


def sym_from_addr(addr: int):
    """-> (name, "file:line") or (None, None)"""
    if not _sym_ok:
        return None, None
    si = SYMBOL_INFOW()
    si.SizeOfStruct = 88  # fixed ABI size on x64 (see dbghelp docs)
    si.MaxNameLen = 2000
    disp = ctypes.c_uint64(0)
    if not dbh.SymFromAddrW(_hproc, addr, ctypes.byref(disp), ctypes.byref(si)):
        return None, None
    loc = None
    line = IMAGEHLP_LINEW64()
    line.SizeOfStruct = ctypes.sizeof(IMAGEHLP_LINEW64)
    ldisp = ctypes.c_uint32(0)
    if dbh.SymGetLineFromAddrW64(_hproc, addr, ctypes.byref(ldisp), ctypes.byref(line)):
        if line.FileName:
            try:
                loc = f"{ctypes.wstring_at(line.FileName)}:{line.LineNumber}"
            except Exception:
                loc = None
    name = si.Name[: si.NameLen] if si.NameLen else si.Name
    return f"{name}+0x{disp.value:X}", loc


# ---------------------------------------------------------------- main

CODES = {
    0xC0000005: "ACCESS_VIOLATION",
    0xC0000409: "STACK_BUFFER_OVERRUN / FAIL_FAST (abort/terminate)",
    0xC00000FD: "STACK_OVERFLOW",
    0xC0000374: "HEAP_CORRUPTION",
    0xE06D7363: "C++ EXCEPTION",
    0x80000003: "BREAKPOINT",
    0xC0000135: "DLL_NOT_FOUND",
    0xC0000142: "DLL_INIT_FAILED",
}


def newest_dump(exe_hint: str | None):
    """Newest .dmp under %LOCALAPPDATA%\\CrashDumps, optionally filtered by exe
    name prefix. Environment-based: no hardcoded machine paths."""
    base = os.path.join(os.path.expandvars("%LOCALAPPDATA%"), "CrashDumps")
    if not os.path.isdir(base):
        raise SystemExit(f"crash dump folder not found: {base}")
    dumps = [
        f
        for f in os.listdir(base)
        if f.lower().endswith(".dmp")
        and (exe_hint is None or f.lower().startswith(exe_hint.lower() + "."))
    ]
    if not dumps:
        raise SystemExit(
            f"no dumps{f' for {exe_hint}' if exe_hint else ''} in {base}"
        )
    dumps.sort(key=lambda f: os.path.getmtime(os.path.join(base, f)))
    return os.path.join(base, dumps[-1])


def main() -> None:
    argv = sys.argv[1:]
    show_all = "--all" in argv
    exe_hint = None
    if "--exe" in argv:
        i = argv.index("--exe")
        if i + 1 < len(argv):
            exe_hint = argv[i + 1]
            del argv[i : i + 2]  # flag + value out of positional args
    dump_path = argv[0] if argv else newest_dump(exe_hint)
    if not argv:
        print(f"analyzing newest dump: {dump_path}")
    load(dump_path)

    print("== exception ==")
    info = exception_info()
    if not info:
        print("  (no exception stream: dump of a live/hung process?)")
        return
    code, fault, rip, rsp, tid = info
    print(f"  code 0x{code:08X} ({CODES.get(code, 'unknown')})  thread {tid}")
    print(f"  fault address 0x{fault:X}   rip 0x{rip:X}   rsp 0x{rsp:X}")
    mpath, moff = find_mod(rip)
    print(f"  rip -> {mpath}+0x{moff:X}" if mpath else "  rip -> unknown module")
    sym_init()
    name, loc = sym_from_addr(rip)
    if name:
        print(f"  rip symbol: {name}" + (f"  ({loc})" if loc else ""))

    print("\n== stack return-address scan ==")
    stk = read_mem(rsp, 0x8000)
    if not stk:
        print("  (stack memory absent - configure WER LocalDumps DumpType=2)")
        return
    shown = 0
    seen = set()
    for i in range(0, len(stk) - 8, 8):
        v = struct.unpack_from("<Q", stk, i)[0]
        mpath, moff = find_mod(v)
        if not mpath:
            continue
        low = mpath.lower()
        if not show_all and not any(k in low for k in ("volition", "mps", "qte", "qfr")):
            continue  # skip Qt/system frames unless --all
        key = (mpath, moff)
        if key in seen:
            continue
        seen.add(key)
        name, loc = sym_from_addr(v)
        tail = f"  {name}" + (f"  ({loc})" if loc else "") if name else ""
        print(f"  rsp+0x{i:04X}  {mpath}+0x{moff:X}{tail}")
        shown += 1
        if shown >= 60:
            print("  ... (capped; use --all to include Qt/system frames)")
            break
    if shown == 0:
        print("  (no in-repo return addresses found on the stack)")


if __name__ == "__main__":
    main()
