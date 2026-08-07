# cmake/

Volition 依赖解析（旁路源码 `add_subdirectory` 或已安装 `find_package`）。

| 模块 | 用途 |
|------|------|
| `VolitionMultiProcessShell.cmake` | `mps::host` / `mps::client` |
| `VolitionQThemeEngine.cmake` | `QThemeEngine::engine` |
| `VolitionQFluentRibbon.cmake` | `QFluentRibbon::ribbon` |
| `VolitionPdfium.cmake` | `pdfium::pdfium`（优先 [pdfium_all](https://github.com/yanxijian/pdfium_all) staged `output`） |
| `VolitionThinClient.cmake` | 薄 exe：`LoadLibrary` + `VolitionClientRun` |

开发默认：旁路仓可用 `VOLITION_DEV_EMBED_MPS/QTE/QFR=ON`；PDF 优先链旁路 [pdfium_all](https://github.com/yanxijian/pdfium_all) 的 `../pdfium_all/output`（`VOLITION_DEV_EMBED_PDFIUM` 默认 OFF）。
