# Abseil / pdfium 同进程（阶段说明）

Volition PDF 与 MPS 的 `abseil_dll` 冲突处理见旁路仓：

- 仓：[AbseilPin](https://github.com/yanxijian/AbseilPin)（本机旁路 `D:\Codes\AbseilPin`）
- 路线：`AbseilPin/docs/zh/ROADMAP.md`（英文：`docs/en/ROADMAP.md`）
- **当前（阶段 2）**：`volition_pdf.dll` 直链产品路径 pdfium（`PDFIUM_ENABLE_V8=OFF`），与 MPS 共用 AbseilPin `20260107.1` 的 `abseil_dll.dll`
- OOP：默认关闭；调试可 `-DVOLITION_PDF_OOP_RENDER=ON` 且运行时 `VOLITION_PDF_OOP=1`

MPS：FetchContent **protobuf v35.1**，默认/推荐  
`-DMPS_ABSEIL_PIN_PREFIX=<AbseilPin/prefix/20260107.1>`  
（旁路存在时 CMake 会自动探测该 prefix）。

验收：`bin/` 旁只有一份 pin 的 `abseil_dll.dll`，且能打开/缩放 PDF，无需 `render/volition_pdf_render.exe`。
