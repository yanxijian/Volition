# Volition — 多类型文档阅读编辑器（早期产品方案）

> **状态**：早期草案（决议已钉，实现未开）  
> **日期**：2026-07-28（修订 2026-08-07：独立 markdown + 薄 exe/DLL）  
> **定位**：产品方向与进程拆分备忘，**不是**实现规格；实现细节以 MultiProcessShell（MPS）仓库文档为准。  
> **依赖框架**：[MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（壳托管 + 原生窗嵌入 + Protobuf IPC）  
> **外观栈（首期）**：[QThemeEngine](https://github.com/yanxijian/QThemeEngine)（主题）+ [QFluentRibbon](https://github.com/yanxijian/QFluentRibbon)（Client 内 Ribbon）  
> **PDF**：[pdfium_all](https://github.com/yanxijian/pdfium_all)（本机旁路仓 `D:\Codes\pdfium_all`）

---

## 1. 一句话目标

做一个 **以 MPS 为外框** 的桌面文档应用：在同一壳窗口下用多 Tab 打开、阅读、编辑多种文件类型（至少 **PDF、Markdown、txt、XML**），按类型用不同 Client 进程隔离，而不是「一个大进程塞所有格式」。

---

## 2. 产品轮廓

### 2.1 要解决什么

| 诉求 | 说明 |
|------|------|
| 多格式同壳 | 用户在一个窗口里切换不同**工作区** Tab，不必为每种文件开独立 App 窗口 |
| 编辑 + 阅读 | 纯文本 / Markdown 以编辑为主；PDF 以阅读为主（编辑能力可分期） |
| 崩溃隔离 | 重渲染栈（尤其 PDF）挂掉时，尽量不影响正在编辑的文本 / Markdown 会话 |
| 可演进 | 后续可加 json / ini / 更多预览后端，而不推翻进程模型 |

### 2.2 非目标（早期不做）

- 做成完整 IDE / 全功能 Office 套件  
- 一扩展名一个进程（txt、xml 各一个 Client；md / pdf 已按栈拆开）  
- 首期就上「同类型每个文档独立进程」的强隔离（MPS 多实例扩展，属后期）  
- 首期跨平台像素打磨（可跟 MPS：Windows 形态 A 优先）  
- Host 进程内 `LoadLibrary` 业务 DLL（MPS 形态 C；无跨类型崩溃隔离）  
- 首期 WebEngine 预览（档 C）

### 2.3 与 MPS 的关系

| 角色 | 职责 |
|------|------|
| **Host（壳）** | Chrome 式顶栏 Tab、按 kind **QProcess 拉起薄 Client exe**、嵌入容器、IPC；**不**实现文档格式逻辑 |
| **Client** | 薄 exe 加载业务 DLL；DLL 内拥有 `WorkspaceWindow`；按 `clientKind` / `appName` **分进程** |
| **协议** | `shell.ipc.v1`（Hello / caps / 心跳 / `CreateSubWindow` / embed…） |

对齐 MPS **形态 A**：**一 clientKind ↔ 一个 ClientSession（进程）**；跨类型可同壳多 Host Tab。

---

## 3. Client 拆分（核心结论）

**按技术栈 + 崩溃域拆 Client，不要按文件扩展名一人一个。**

### 3.1 定案拆法（首期 3 个 Client）

| Client（clientKind） | 文件类型 | 理由 |
|--------------------|----------|------|
| **text** | `.txt`、`.xml`（可再扩 `.json`、`.ini`…） | 同一编辑器内核；XML **仅语法高亮** |
| **markdown** | `.md` | 独立预览栈（档 A）；与纯文本分进程，便于演进预览 |
| **pdf** | `.pdf` | 渲染栈重；链 **pdfium_all**；必须单独进程 |

### 3.2 打包形态（薄 exe + 业务 DLL）

每种 kind：

| 产物 | 职责 |
|------|------|
| `volition_<kind>.exe` | 薄壳：解析 MPS Client CLI → `LoadLibrary` 同目录 DLL → 调 `VolitionClientRun` |
| `volition_<kind>.dll` | 业务：`*ContentView` / `WorkspaceWindow` / `DocumentView` / 格式栈 |

Host **只启动 exe**（MPS `QProcess`）；**不**在 Host 内加载 Client DLL。

### 3.3 纯文本要不要合并？

**txt / xml 合并进 text；md 独立。**

- txt / xml 同一故障域与编辑交互。  
- Markdown 有预览管线，独立 `markdown` Client。  
- XML **仅语法高亮**（不做树视图 / schema）。

### 3.4 Markdown 预览档位

| 预览方案 | 决议 | Client |
|----------|------|--------|
| **档 A**（`md4c` → HTML → `QTextBrowser`），接受 GFM 上限 | **首期** | **`markdown`** |
| 档 B litehtml | 预览不够时再升 | 仍 `markdown` |
| 档 C WebEngine | **非首期** | 仍可留在 `markdown` 进程 |

### 3.5 明确不要

| 做法 | 原因 |
|------|------|
| PDF / Markdown 并进 text | 栈与崩溃面不同 |
| Host 内 LoadLibrary Client | 形态 C；PDF 崩拖死壳 |
| 一扩展名一进程（txt≠xml） | 违背「同类型共享 Session」 |
| 首期「每个文档互不影响崩溃」 | 多实例扩展属后期 |

### 3.6 起步落地顺序

```text
1. Host 注册启动器（text / markdown / pdf 薄 exe）；后缀 → clientKind
2. text DLL：WorkspaceWindow + TextDocumentView（QPlainTextEdit）
3. markdown DLL：WorkspaceWindow + MarkdownDocumentView（编辑 + 档 A 预览）
4. pdf DLL：WorkspaceWindow + PdfDocumentView（pdfium_all）
5. 左右栏 NavigationPane / UtilityPane 占位
```

---

## 4. 命名

### 4.1 产品与产物

**已定（2026-08-04）**：产品名、仓库名、显示名均为 **Volition**。

| 用途 | 名称 |
|------|------|
| 仓库 / 工程 | `Volition` |
| Host | `volition_host.exe` |
| Client 薄 exe / 业务 DLL | `volition_text` / `volition_markdown` / `volition_pdf`（各一对 `.exe`+`.dll`） |
| `appName` / `clientKind` | `text` / `markdown` / `pdf` |
| DLL 导出入口 | `VolitionClientRun` |
| 对外显示名 | **Volition** |

### 4.2 Client 类型词表（与 MPS ContentView 对齐）

| 层级 | 推荐名 | 说明 |
|------|--------|------|
| MPS 适配 | `TextContentView` / `MarkdownContentView` / `PdfContentView` | 实现 `mps::client::ContentView` |
| 嵌入根窗 | `WorkspaceWindow` | QFR `RibbonWindow`；被 Host SetParent 的 HWND |
| 三栏装配 | `WorkspaceLayout` | 左 / 中 / 右 |
| 左栏 | `NavigationPane` | 占位 |
| 右栏 | `UtilityPane` | 占位 |
| 中栏容器 | `DocumentStack` | 中栏文档面（Client 本地） |
| 中栏一页 | `DocumentView` | 基类 |
| 文本 | `TextDocumentView` | `QPlainTextEdit` + 高亮 |
| Markdown | `MarkdownDocumentView` | 编辑 + 档 A 预览 |
| PDF | `PdfDocumentView` | pdfium_all |
| Host 侧 | `ShellWindow` / `TabInfo` / `sessionId` / `tabId` | 沿用 MPS |

**刻意不用**（业务层）：`Page` / `ClientPage` / `SubWindow`；左右栏用 **Pane**。

---

## 5. UI 结构与双层 Tab

与 MPS Demo 同构：**Host 顶栏 Tab = 一次嵌入的 ContentView（工作区）**；中栏为 Client 本地文档视图（一窗一文）。

```text
┌─ ShellWindow（volition_host / MPS）─────────────────────────┐
│  Host TabStrip（Chrome 式）  [工作区1] [工作区2] [+]  [_][□][×] │
├─────────────────────────────────────────────────────────────┤
│  EmbedContainer → 当前 Tab 的 WorkspaceWindow HWND            │
│  ┌─ WorkspaceWindow（QFR RibbonWindow；在 Client DLL 内）──┐ │
│  │  Ribbon                                                 │ │
│  ├──────────┬────────────────────────────┬────────────────┤ │
│  │ Navigation│  DocumentStack             │ UtilityPane   │ │
│  │ Pane      │  DocumentView …            │ （占位）       │ │
│  └──────────┴────────────────────────────┴────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

| 交互 | 行为 |
|------|------|
| 打开文件 | 按后缀选 kind → 对应薄 exe 会话；进当前工作区中栏 `DocumentStack` |
| Host 顶栏「+」/ 新建工作区 | `CreateSubWindow` → 新 ContentView / Host Tab |
| 中栏「+」 | 仅 Client 本地新文档视图 |

---

## 6. Markdown 编辑与预览（技术选型）

> **已定**：编辑与预览分开；解析不自研；**档 A**；落在独立 **`markdown` Client**。

### 6.1 编辑端

| 方案 | 状态 |
|------|------|
| **`QPlainTextEdit` + 语法高亮** | **P0**（markdown / text 均可） |
| KSyntaxHighlighting | P0/P1 可选 |
| QScintilla | 以后 |

### 6.2 预览端

| 档 | 栈 | 状态 |
|----|-----|------|
| **A** | md4c → HTML → `QTextBrowser` | **首期** |
| B | litehtml / qlitehtml | 不够再升 |
| C | Qt WebEngine | **非首期** |

### 6.3 分层

| 层 | 自研 vs 开源 |
|----|----------------|
| Host / IPC / 薄 exe 加载 | **自研** + MPS |
| WorkspaceWindow / DocumentStack | **自研** + QFR |
| 文本 / Markdown 编辑 | `QPlainTextEdit` |
| Markdown → HTML | **md4c** |
| PDF | **pdfium_all** |

---

## 7. 能力分期（粗线条）

| 阶段 | 交付重心 | 验收感 |
|------|----------|--------|
| **P0** | Host + 三 Client（exe+dll）骨架；QFR；中栏；左右占位；txt/xml；md 档 A；pdf 阅读 | 同壳多工作区；三进程；PDF 崩不影响 text/md |
| **P1** | 查找替换、编码、高亮；PDF 缩放/翻页 | 日常可读可改 |
| **P2** | 文件关联 / 最近文件；预览升档 B（若需要） | 更像产品 |
| **P3** | 偏好；拖出 Tab；左右栏真实功能 | 成品感 |
| **以后** | PDF 批注、WebEngine、同类型多实例 | 按需求再开 |

---

## 8. 架构草图

```text
volition_host
  ├─ QProcess → volition_text.exe     → LoadLibrary volition_text.dll
  ├─ QProcess → volition_markdown.exe → LoadLibrary volition_markdown.dll
  └─ QProcess → volition_pdf.exe      → LoadLibrary volition_pdf.dll
         │                                    │
         │                                    ├─ *ContentView / WorkspaceWindow
         │                                    └─ DocumentStack / *DocumentView
```

**打开文件分流**：

```text
用户打开 path
  → 扩展名 → clientKind（text | markdown | pdf）
  → Host 确保对应 ClientSession（薄 exe）
  → CreateSubWindow（若需新工作区）
  → DLL 内 DocumentStack 打开 DocumentView 并加载
```

---

## 9. 已决议摘要

| # | 议题 | 决议 |
|---|------|------|
| 1 | Markdown | 档 A；**独立 `markdown` Client**（2026-08-07 修订） |
| 2 | PDF 引擎 | **pdfium_all** |
| 3 | 文本内核 | 先 **QPlainTextEdit** |
| 4 | XML | **仅语法高亮** |
| 5 | 首期 UI | **接 QFR Ribbon**（Client DLL 内 `WorkspaceWindow`） |
| 6 | 显示名 / 仓库 | **Volition** |
| 7 | Client 打包 | **每 kind 薄 exe + 业务 DLL**；Host 只启 exe（形态 A） |

---

## 10. 择机改进（已对照代码核实，以后再做）

来源：2026-10 外部点评。只收录与现状相符、且未做的项。

| 项 | 核实 | 拟做 |
|----|------|------|
| 大文件加载 | **已落地阈值异步读（2026-10-04）**：`AsyncFileLoader`（≥512KB 进线程池）；Markdown 大文预览 debounce | 完整虚拟滚动仍待做 |
| PDF 栅格线程 | PDF 已是独立 Client 进程；`pdfium` 默认同进程画在该 Client 里；可选 `VOLITION_PDF_OOP=1` | 页栅格离 UI 线程；像素走 MPS 共享内存数据面（见 MPS 计划）。**不要**再拆一个「PDF Client」——已经是 |
| Abseil | `volition_pdf.dll` 与 MPS 共用 AbseilPin `abseil_dll`，避免同进程 ODR | 继续 pin；OOP 渲染作为隔离升级，而非默认再引一份 Abseil |

**不收录**：另建聚合仓（已有 `codes-workspace`）；「pdfium 必须单独 Client」——kind=`pdf` 已是独立进程。

---

## 11. 相关材料

| 材料 | 说明 |
|------|------|
| MPS 规格 | `MultiProcessShell/docs/zh/multiprocess-shell-spec.md` |
| Demo 形态 / IPC | MPS `demo-morphology.md`、`demo-ipc.md`、`proto/shell/ipc/v1/ipc.proto` |
| pdfium_all | `https://github.com/yanxijian/pdfium_all` |

---

## 12. 修订记录

| 日期 | 说明 |
|------|------|
| 2026-07-28 | 初稿 |
| 2026-08-01 | Markdown 技术选型 |
| 2026-08-04 | 产品名 **Volition**；仓库初始化 |
| 2026-08-07 | 五项决议；双层 Tab；命名表；PDF=pdfium_all；QFR |
| 2026-08-07 | 独立 markdown；薄 exe + DLL（形态 A） |
| 2026-10-04 | 择机改进：大文件异步加载、PDF 栅格线程、Abseil pin（对照代码核实） |

