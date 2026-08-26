# clients/text/

Volition **text** Client：`volition_text.exe`（薄壳）+ `volition_text.dll`（业务）。

`clientKind` / `appName` = `text`。

## 覆盖类型

`.txt`、`.xml`、`.json`、`.ini`、`.log`、`.csv`；其中 `.xml` 提供基础语法高亮。编辑器支持 `Ctrl+F` 查找。**不含** `.md`（见 `clients/markdown/`）。

## 产物

| 目标 | 说明 |
|------|------|
| `volition_text` | 薄 exe：Host `QProcess` 启动；`LoadLibrary` 同目录 DLL → `VolitionClientRun` |
| `volition_text_lib` | SHARED：`TextContentView` → `WorkspaceWindow` → `DocumentStack` → `TextDocumentView`（`QPlainTextEdit`） |

导出约定见 [../common/](../common/)。产品边界见 [docs/zh/product-plan.md](../../docs/zh/product-plan.md)。
