# Abseil / pdfium 同进程

Volition PDF Client 与 MPS 共用一份 `abseil_dll`：

- [AbseilPin](https://github.com/yanxijian/AbseilPin)（本机旁路 `D:\Codes\AbseilPin`）
- 路线图：`AbseilPin/docs/zh/ROADMAP.md`（英文：`docs/en/ROADMAP.md`）
- **现状**：`volition_pdf.dll` 链接产品路径 pdfium（`PDFIUM_ENABLE_V8=OFF`），与 MPS 共用 AbseilPin **`20260107.1`**
- **可选 OOP**：`-DVOLITION_PDF_OOP_RENDER=ON`，运行时 `VOLITION_PDF_OOP=1`

MPS：FetchContent **protobuf v35.1**；推荐  
`-DMPS_ABSEIL_PIN_PREFIX=<AbseilPin/prefix/20260107.1>`  
（旁路存在时 CMake 自动探测）。

约束：`build/bin/` 仅一份 pin 的 `abseil_dll.dll`；默认同进程打开/缩放 PDF。

本地验收：

```text
powershell -File scripts\smoke_bin_layout.ps1 -BinDir build\bin -RequirePdfium
```
