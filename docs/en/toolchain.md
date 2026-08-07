# Volition toolchain contract (VolitionToolchain)

Volition and its dependencies (MPS, QTE, QFR, AbseilPin, pdfium product path) share one compile contract. **Windows** is the reference; macOS / Linux keep the same C++ / extensions rules and use each platform’s default shell compiler.

| Item | Value |
|------|--------|
| `CMAKE_CXX_STANDARD` | **20** (`REQUIRED ON`) |
| `CMAKE_CXX_EXTENSIONS` | **OFF** |
| Windows compiler | **MSVC `cl`** (no clang-cl on the product path) |
| Windows CRT | `MultiThreaded$<$<CONFIG:Debug>:Debug>DLL` (`/MD`) |
| Abseil | [AbseilPin](https://github.com/yanxijian/AbseilPin) **`20260107.1`** (one copy per process) |
| pdfium | **`PDFIUM_ENABLE_V8=OFF`** in-process; Acrobat JS stays OOP |
| Extra global flags | Do not add shell-foreign flags (e.g. `/FIwindows.h`) by default; prefer source fixes |

> Shell and pdfium share **C++20** so the fork does not have to down-level Chromium-style sources to 17.

Canonical Chinese: [../zh/toolchain.md](../zh/toolchain.md)
