# clients/markdown/

Volition **markdown** Client：`volition_markdown.exe` + `volition_markdown.dll`。

`clientKind` / `appName` = `markdown`。

## 覆盖类型

`.md`：编辑 + 档 A 预览（md4c → `QTextBrowser`）。

## 产物

| 目标 | 说明 |
|------|------|
| `volition_markdown` | 薄 exe → `LoadLibrary` → `VolitionClientRun` |
| `volition_markdown_lib` | SHARED：`MarkdownContentView` → `WorkspaceWindow` → `MarkdownDocumentView` |

导出约定见 [../common/](../common/)。产品边界见 [docs/zh/product-plan.md](../../docs/zh/product-plan.md)。
