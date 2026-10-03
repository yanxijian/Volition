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
| Windows system libs | `target_link_libraries` in `CMakeLists.txt` only; no `#pragma comment(lib, …)` (`.cursor/rules/cmake-windows-libs.mdc`) |

> Shell and pdfium share **C++20**.

## Sibling layout (Codes workspace)

| Role | Local build dir | Published artifacts |
|------|-----------------|---------------------|
| Volition (app) | `<repo>/build` → `build/bin/` | No install; consumes sibling stages |
| MPS / QTE / QFR | `<repo>/build-shared` | Optional `D:/Codes/prefix`; Volition may embed siblings |
| AbseilPin | `build/<pin>` | **`prefix/<pin>/`** |
| pdfium_all | `pdfium/out/cmake-msvc` (or `cmake-v8`) | **`output/{include,lib,bin}`** |
| Tool caches | — | `pdfium_all/.tools/`, `D:/Codes/vcpkg`, local Qt |

Names differ on purpose: Abseil pins coexist by version; pdfium `out/` follows GN; `output/` is a stage prefix. Apps use `build`; installable libs use `build-shared`.

Canonical Chinese: [../zh/toolchain.md](../zh/toolchain.md)
