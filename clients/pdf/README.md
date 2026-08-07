# clients/pdf/

Volition **pdf** Client：`volition_pdf.exe` + `volition_pdf.dll`。

`clientKind` / `appName` = `pdf`。渲染：**pdfium_all**（优先 staged `output`）。

## 产物

| 目标 | 说明 |
|------|------|
| `volition_pdf` | 薄 exe → `LoadLibrary` → `VolitionClientRun` |
| `volition_pdf_lib` | SHARED：`PdfContentView` → `WorkspaceWindow` → `PdfDocumentView` |

导出约定见 [../common/](../common/)。产品边界见 [docs/zh/product-plan.md](../../docs/zh/product-plan.md)。
