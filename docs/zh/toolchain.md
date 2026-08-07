# Volition 工具链契约（VolitionToolchain）

Volition 及其依赖（MPS、QTE、QFR、AbseilPin、pdfium 产品路径）共用同一套编译约定。当前以 **Windows** 为准；macOS / Linux 沿用同一 C++/扩展语义，编译器取各平台壳默认。

| 项 | 值 |
|----|-----|
| `CMAKE_CXX_STANDARD` | **20**（`REQUIRED ON`） |
| `CMAKE_CXX_EXTENSIONS` | **OFF** |
| Windows 编译器 | **MSVC `cl`**（产品路径不用 clang-cl） |
| Windows CRT | `MultiThreaded$<$<CONFIG:Debug>:Debug>DLL`（`/MD`） |
| Abseil | [AbseilPin](https://github.com/yanxijian/AbseilPin) **`20260107.1`**（进程内一份） |
| pdfium | **`PDFIUM_ENABLE_V8=OFF`**（同进程）；要 Acrobat JS 走 OOP |
| 额外全局旗 | 默认不加壳没有的 `/FIwindows.h` 等；优先改源码 |

> 壳与 pdfium 统一 **C++20**，避免在 fork 里把 Chromium 风格源码降到 17。

English: [../en/toolchain.md](../en/toolchain.md)
