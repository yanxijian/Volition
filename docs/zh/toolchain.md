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

## 旁路仓目录约定（Codes 工作区）

| 角色 | 本地构建目录 | 对外产物 |
|------|--------------|----------|
| Volition（应用） | `<repo>/build` → 运行 `build/bin/` | 无 install；消费旁路产物 |
| MPS / QTE / QFR（可安装库） | `<repo>/build-shared` | 可选装到 `D:/Codes/prefix`；Volition 默认 **embed** 旁路源码，可不装 |
| AbseilPin | `build/<pin>` | **`prefix/<pin>/`**（`bin/abseil_dll.dll` + cmake） |
| pdfium_all | `pdfium/out/cmake-msvc`（V8 为 `cmake-v8`） | **`output/{include,lib,bin}`** |
| 工具缓存（勿当产物） | — | `pdfium_all/.tools/`、`D:/Codes/vcpkg`、本机 Qt |

**不要统一成单一名字的原因：** Abseil 按 pin 版本并存；pdfium 的 `out/` 对齐 Chromium/GN 习惯；`output/` 是 stage 前缀而非 `cmake --install`。应用侧统一用 `build`，库侧脚本/Preset 统一用 `build-shared`。

English: [../en/toolchain.md](../en/toolchain.md)
