# clients/pdf/

Volition **pdf** Client：`volition_pdf.exe` + `volition_pdf.dll`（与 text/markdown 同在 `bin/`）。

`clientKind` / `appName` = `pdf`。

同进程链接产品路径 pdfium（`PDFIUM_ENABLE_V8=OFF`），与 MPS 共用 AbseilPin `abseil_dll`。  
可选 OOP：`-DVOLITION_PDF_OOP_RENDER=ON`，运行时 `VOLITION_PDF_OOP=1` → `bin/render/volition_pdf_render.exe`。

## 产物

| 目标 | 输出位置 | 说明 |
|------|----------|------|
| `volition_pdf` | `bin/` | 薄 exe → `VolitionClientRun` |
| `volition_pdf_lib` | `bin/` | SHARED：Workspace + `PdfDocumentView`（进程内 pdfium） |
| `volition_pdf_render` | `bin/render/` | 仅 `VOLITION_PDF_OOP_RENDER=ON` 时 |

导出约定见 [../common/](../common/)。产品边界见 [docs/zh/product-plan.md](../../docs/zh/product-plan.md)。
