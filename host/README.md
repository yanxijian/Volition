# host/

Volition **Host**（MPS 壳编排）→ `volition_host.exe`。

## 职责

- 注册 Client 启动器（薄 exe）：`text` → `volition_text.exe`，`markdown` → `volition_markdown.exe`，`pdf` → `volition_pdf.exe`
- 后缀 → clientKind：`.txt`/`.xml` → text；`.md` → markdown；`.pdf` → pdf
- Chrome 式顶栏 Tab：一个 Host Tab = 一次嵌入的 ContentView（工作区）
- Home：打开 / 最近 / 收藏 / 常用目录 + 搜索 / 主题 / 设置（`HomeContent` + `LibraryStore`）
- 多语言：`bin/langs/volition_*.qm`（源文案英文 + `translations/*.ts`；设置里可切换）
- 本地配置：`bin/config/volition.ini`（主题 / 语言 / 最近 / 收藏；IniFormat，不写注册表）
- 顶栏「+」/ 新建工作区 → `CreateSubWindow`

## 不负责

- 文档格式逻辑、Ribbon、中栏文档面（在各 Client **DLL** 的 `WorkspaceWindow` / `DocumentStack`）
- **不**在 Host 内 `LoadLibrary` Client DLL（形态 A：进程内由薄 exe 加载）

见 [docs/zh/product-plan.md](../docs/zh/product-plan.md) §3–§5。
