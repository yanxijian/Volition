# clients/pdf/

Volition **pdf** Client：`volition_pdf.exe` + `volition_pdf.dll`（MPS/Qt），渲染在旁路进程。

`clientKind` / `appName` = `pdf`。

## 为何拆进程渲染

`pdfium_all` 与 MPS FetchContent **protobuf 各带一份 `abseil_dll.dll`**，同进程无法共存（`LoadLibrary` → 127）。  
因此 **Client DLL 不链接 pdfium**；页面由 `bin/pdf/render/volition_pdf_render.exe` 出图（自带 pdfium abseil）。

## 产物

| 目标 | 输出位置 | 说明 |
|------|----------|------|
| `volition_pdf` | `bin/pdf/` | 薄 exe → `VolitionClientRun` |
| `volition_pdf_lib` | `bin/pdf/` | SHARED：Workspace + `PdfDocumentView`（QProcess 调 render） |
| `volition_pdf_render` | `bin/pdf/render/` | 仅 pdfium：`--info` / `--page` / `--out` BMP |

导出约定见 [../common/](../common/)。产品边界见 [docs/zh/product-plan.md](../../docs/zh/product-plan.md)。
