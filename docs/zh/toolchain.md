# Volition 工具链契约（VolitionToolchain）

Volition 及其依赖（MPS、QTE、QFR、AbseilPin、pdfium 产品路径）共用同一套编译约定。当前以 **Windows** 为准；macOS / Linux 沿用同一 C++/扩展语义，编译器取各平台壳默认。

| 项 | 值 |
|----|-----|
| `CMAKE_CXX_STANDARD` | **20**（`REQUIRED ON`） |
| `CMAKE_CXX_EXTENSIONS` | **OFF** |
| Windows 编译器 | **MSVC `cl`**（产品路径不用 clang-cl） |
| Windows CRT | `MultiThreaded$<$<CONFIG:Debug>:Debug>DLL`（`/MD`） |
| Abseil | [AbseilPin](https://github.com/yanxijian/AbseilPin) **`20260107.1`**（进程内一份） |
| pdfium | **`PDFIUM_ENABLE_V8=OFF`**（同进程）；Acrobat JS 走 OOP |
| 额外全局旗 | 默认不加壳没有的 `/FIwindows.h` 等；优先改源码 |
| Windows 系统库 | 只在 `CMakeLists.txt` 里 `target_link_libraries`；禁止 `#pragma comment(lib, …)`（`.cursor/rules/cmake-windows-libs.mdc`） |

> 壳与 pdfium 统一 **C++20**。

## 旁路仓目录约定（Codes 工作区）

| 角色 | 本地构建目录 | 对外产物 |
|------|--------------|----------|
| Volition（应用） | `<repo>/build` → `build/bin/` | 无 install；消费旁路产物 |
| MPS / QTE / QFR | `<repo>/build-shared` | 可选 `D:/Codes/prefix`；Volition 默认可 embed 旁路源码 |
| AbseilPin | `build/<pin>` | **`prefix/<pin>/`** |
| pdfium_all | `pdfium/out/cmake-msvc`（V8：`cmake-v8`） | **`output/{include,lib,bin}`** |
| 工具缓存 | — | `pdfium_all/.tools/`、`D:/Codes/vcpkg`、本机 Qt |

命名差异有意保留：Abseil 按 pin 版本并存；pdfium `out/` 对齐 GN 习惯；`output/` 为 stage 前缀。应用用 `build`，可安装库用 `build-shared`。

English: [../en/toolchain.md](../en/toolchain.md)
