# Abseil / pdfium 同进程（阶段说明）

Volition PDF 与 MPS 的 `abseil_dll` 冲突处理见旁路仓：

- 仓：[AbseilPin](https://github.com/yanxijian/AbseilPin)（本机旁路 `D:\Codes\AbseilPin`）
- 路线：`AbseilPin/docs/ROADMAP.md`
- **当前**：`volition_pdf` 不链 pdfium；渲染在 `bin/pdf/render/volition_pdf_render.exe`
- **统一后**：同一 pin 的 `abseil_dll` → 再把 pdfium 并回 `volition_pdf.dll`

MPS 阶段 1 开关：`-DMPS_ABSEIL_PIN_PREFIX=<AbseilPin/prefix/20240116.0>`（须与 protobuf v29.3 匹配的 pin）。
