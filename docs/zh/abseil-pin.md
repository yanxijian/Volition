# Abseil / pdfium 同进程（阶段说明）

Volition PDF 与 MPS 的 `abseil_dll` 冲突处理见旁路仓：

- 仓：[AbseilPin](https://github.com/yanxijian/AbseilPin)（本机旁路 `D:\Codes\AbseilPin`）
- 路线：`AbseilPin/docs/zh/ROADMAP.md`（英文：`docs/en/ROADMAP.md`）
- **当前**：`volition_pdf` 不链 pdfium；渲染在 `bin/pdf/render/volition_pdf_render.exe`
- **统一后**：同一 pin 的 `abseil_dll`（且同 STL ABI）→ 再把 pdfium 并回 `volition_pdf.dll`

MPS（已升）：FetchContent **protobuf v35.1**，默认/推荐  
`-DMPS_ABSEIL_PIN_PREFIX=<AbseilPin/prefix/20260107.1>`  
（旁路存在时 CMake 会自动探测该 prefix）。
