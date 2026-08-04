# Volition — 多类型文档阅读编辑器（早期产品方案）

> **状态**：早期草案（整理自 2026-07-25 前后讨论）  
> **日期**：2026-07-28（修订 2026-08-04：定名 Volition）  
> **定位**：产品方向与进程拆分备忘，**不是**实现规格；实现细节以 MultiProcessShell（MPS）仓库文档为准。  
> **依赖框架**：[MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（壳托管 + 原生窗嵌入 + Protobuf IPC）  
> **可选外观栈**：QThemeEngine（主题）/ QFluentRibbon（Ribbon UI，若产品壳需要 Office-like 命令条）

---

## 1. 一句话目标

做一个 **以 MPS 为外框** 的桌面文档应用：在同一壳窗口下用多 Tab 打开、阅读、编辑多种文件类型（至少 **PDF、Markdown、txt、XML**），按类型用不同 Client 进程隔离，而不是「一个大进程塞所有格式」。

---

## 2. 产品轮廓

### 2.1 要解决什么

| 诉求 | 说明 |
|------|------|
| 多格式同壳 | 用户在一个窗口里切换不同文档 Tab，不必为每种文件开独立 App 窗口 |
| 编辑 + 阅读 | 纯文本 / Markdown 以编辑为主；PDF 以阅读为主（编辑能力可分期） |
| 崩溃隔离 | 重渲染栈（尤其 PDF）挂掉时，尽量不影响正在编辑的文本会话 |
| 可演进 | 后续可加 json / ini / 更多预览后端，而不推翻进程模型 |

### 2.2 非目标（早期不做）

- 做成完整 IDE / 全功能 Office 套件  
- 一扩展名一个进程（txt、xml、md、pdf… 各起一个 Client）  
- 首期就上「同类型每个文档独立进程」的强隔离（那是 MPS 多实例扩展，属后期产品策略）  
- 首期跨平台像素打磨（可跟 MPS：Windows 形态 A 优先）

### 2.3 与 MPS 的关系

| 角色 | 职责 |
|------|------|
| **Host（壳）** | 外框、Tab、启停 Client、嵌入容器、IPC 编排；**不**实现具体文档格式逻辑 |
| **Client** | 真正拥有业务窗口；按 `pageType` / `appName` 分进程；同类型多文档默认同进程多子窗 |
| **协议** | `shell.ipc.v1`（Hello / caps / 心跳 / createWindow / embed…） |

对齐 MPS 默认约定：**一 pageType ↔ 一个 ClientSession**；跨类型可同壳多 Tab。

---

## 3. Client 拆分（核心结论）

**按技术栈 + 崩溃域拆 Client，不要按文件扩展名一人一个。**

### 3.1 建议拆法

| Client（pageType） | 文件类型 | 理由 |
|--------------------|----------|------|
| **text** | `.txt`、`.xml`（可再扩 `.json`、`.ini`…） | 同一套编辑器内核 + 语法高亮 / 校验 / 折叠插件即可；同故障域、同 UI 模型（缓冲、撤销、查找、编码） |
| **markdown** | `.md` | **取决于预览实现**（见 §3.3、§5）；未定前可先作为 text 的一种 mode |
| **pdf** | `.pdf` | 渲染栈重、内存与崩溃面大，**必须单独进程**，禁止与文本绑在一起 |

默认做成 **2～3 个 Client**，不要 5 个（txt / xml / md / pdf / … 各一个）。

### 3.2 纯文本要不要合并？

**要合并。**

- txt / xml 属于同一故障域、同一编辑器交互模型。  
- 拆成多个 Client 只会重复：启动器、Hello caps、嵌入路径、部署与调试成本。  
- 差异用 Client **内部** 的语言 / 模式插件解决（高亮、schema、折叠），**不要用进程边界解决**。

### 3.3 Markdown：单独还是并进 text？

| 预览方案 | 建议 | Client 数量 |
|----------|------|-------------|
| **轻量预览**（`md4c` → HTML → `QTextBrowser` 等，无 Chromium） | **并进 text** | **2**：`text` + `pdf` |
| **重预览**（Qt WebEngine / Chromium） | **独立 `markdown` Client**，避免预览崩掉拖死正在编 XML 的会话 | **3**：`text` + `markdown` + `pdf` |

### 3.4 明确不要

| 做法 | 原因 |
|------|------|
| PDF 并进 text | 栈不同、崩溃特征不同，浪费 MPS「跨类型隔离」收益 |
| 一扩展名一进程 | 违背 MPS「同类型共享 Session」默认；起步成本高、收益低 |
| 首期就「每个文档互不影响崩溃」 | 用规格里的多实例扩展（如 `appName#instance`）即可，属产品策略，不是默认起步形态 |

### 3.5 起步落地顺序

```text
1. Host 注册启动器 + caps（只认 text / pdf）
2. text Client：txt + xml（Markdown 先当 mode，轻量预览可同进程）
3. pdf Client：只读打开 / 翻页 / 缩放（编辑能力后置）
4. 预览方案定案后：决定 Markdown 是否拆出第三个 Client
5. 再考虑 json/ini 等并入 text；PDF 批注/表单等分期
```

Host 侧 Tab 核心**不必**为每种后缀改一遍逻辑；后缀 → pageType 的映射放在打开文件 / 启动器配置层。

---

## 4. 命名

**已定（2026-08-04）**：产品名、仓库名、显示名均为 **Volition**。

| 用途 | 名称 |
|------|------|
| 仓库 / 工程 | `Volition`（`https://github.com/yanxijian/Volition`） |
| Host / Client 目标（规划） | `volition_host` / `volition_text` / `volition_pdf` |
| 对外显示名 | **Volition** |

早期候选（Leaf / Folio / DocShell / Quill / Inkframe 等）仅作历史记录，不再使用。

---

## 5. Markdown 编辑与预览（技术选型）

> **结论**：编辑器和预览器分开选；**CommonMark/GFM 解析与 HTML 渲染不要自研**；预览优先走轻量栈，**WebEngine 仅作可选升级**。  
> 与 §3.3 一致：轻量预览 → Markdown 并进 `text` Client；只有重预览才值得独立 `markdown` Client。

### 5.1 编辑端：壳自研，内核借开源

Markdown **源文本编辑**不需要自研编辑器引擎。

| 方案 | 适合阶段 | 说明 |
|------|----------|------|
| **`QPlainTextEdit` + 语法高亮** | **P0 推荐** | 与 txt/xml 同一 text Client；撤销、查找、编码、Tab 行为一致 |
| **KSyntaxHighlighting**（KDE） | P0/P1 | 现成 Markdown 高亮定义，比手写正则稳 |
| **QScintilla** | 需要强编辑能力时 | 折叠、多光标、大文件；依赖更重，与 QTE 集成需多测 |

**不建议**：自研 Markdown 词法/高亮（CommonMark + GFM 边界多，维护成本高）。  
编辑端本质是 **text Client 的一种 mode**，不是第三个编辑器产品。

### 5.2 预览端：三档，由轻到重

#### 档 A — 轻量（**起步首选**）

**`md4c`（或 `cmark-gfm`）→ HTML → `QTextBrowser` / `QTextDocument`**

| 组件 | 说明 |
|------|------|
| **md4c** | 纯 C、体积小、快，CommonMark 合规；[MarkdownEditorQt](https://github.com/SideFx/MarkdownEditorQt) 即此路线 |
| **cmark-gfm** | 偏 GitHub 语义（表格、任务列表、删除线等 GFM 扩展）时可考虑 |
| **Qt 内置** | Qt 6 `QTextDocument` Markdown 读写可先 PoC，验证分栏编辑+预览是否够用 |

**优点**：无 Qt WebEngine → Markdown **不必拆第三个 Client**；预览走 **QThemeStyle / QTE**；崩溃面小、包体小、CI 简单。  
**缺点**：复杂 CSS、Mermaid、数学公式、完整 GFM 支持有限；`QTextBrowser` 对 HTML/CSS 弱于浏览器（参考实现里常有标签兼容补丁，如 `<del>` → `<s>`）。

**适用**：阅读向编辑器、基础 Markdown 写作、Volition 早期目标。

#### 档 B — 中等（预览不够时再上）

**`md4c` → HTML → `litehtml` / `qlitehtml`**

- Qt Creator 文档渲染同类思路：比 `QTextBrowser` 更接近真 HTML 布局  
- 仍是 Widgets + 原生控件，不必 WebEngine  
- 集成成本高于档 A，但可控  

**适用**：需要更好表格/列表/图片排版，仍不想背 Chromium。

#### 档 C — 重量（**明确需要再选**）

**`Qt WebEngine` + JS 栈（如 markdown-it / marked + highlight.js）**

- 预览最接近 Typora / VS Code 网页预览  
- **建议独立 `markdown` Client**；主题与 QTE 双轨（Web 内容 CSS 单独维护）  
- 包体、内存、CI、许可排查更重  

**适用**：强 GFM、插件、Mermaid/KaTeX、自定义 CSS 主题为硬需求。

### 5.3 整仓 fork vs 库级借用

| 类型 | 建议 |
|------|------|
| **整 app fork**（Obsidian、Typora、MarkText 等） | **不适合**作 MPS Client 底座；Electron/架构与壳模型不对齐 |
| **库级借用**（md4c、cmark-gfm、KSyntaxHighlighting、litehtml） | **推荐** |
| **参考实现**（MarkdownEditorQt、Textosaurus） | **推荐**抄集成方式，不要整仓并入 |
| **md4qt**（KDE，C++ AST） | 仅当要在 C++ 做 AST 级操作（大纲、块编辑）时考虑；一般预览用不上 |

**不要自研**：CommonMark/GFM 解析器、完整 HTML 渲染引擎。

### 5.4 对 Volition 的落地建议

```text
P0（建议定案）
  编辑：QPlainTextEdit + KSyntaxHighlighting（或简单高亮）
  预览：md4c → HTML → QTextBrowser（或先试 Qt 内置 setMarkdown）
  布局：QSplitter 左右/上下分栏，debounce 300–500ms 刷新预览
  Client：并进 text，不拆 markdown

P1（预览投诉变多）
  换 litehtml/qlitehtml，或 md4c 开 GFM flags + 补 CSS

P2（产品明确要求）
  可选 WebEngine 预览 → 独立 markdown Client
```

**PoC 最小集**：`QPlainTextEdit` + `md4c` + `QTextBrowser` 分栏，可快速验证是否满足多数阅读/编辑场景；不够再升档 B/C，而非一上来 WebEngine。

### 5.5 分层拍板（自研 vs 开源）

| 层 | 自研 vs 开源 |
|----|----------------|
| Tab / 分栏 / 打开保存 / MPS 嵌入 | **自研**（Host + text Client 壳） |
| 纯文本编辑交互 | **自研薄壳 + Qt 控件** |
| 语法高亮 | **借 KSyntaxHighlighting 或同类** |
| Markdown → HTML | **借 md4c 或 cmark-gfm** |
| HTML 预览 | **先用 QTextBrowser；不够再 litehtml；最后才 WebEngine** |

---

## 6. 能力分期（粗线条）

| 阶段 | 交付重心 | 验收感 |
|------|----------|--------|
| **P0** | MPS Host 壳 + `text`（打开/编辑 txt、xml）+ `pdf`（打开/阅读） | 同壳多 Tab；切类型不串进程；PDF 崩不影响 text |
| **P1** | 文本：查找替换、编码、基础语法高亮；PDF：缩放/翻页/书签（若栈支持） | 日常可读可改 |
| **P2** | Markdown 预览按 §5 定案（轻量合并 or WebEngine 独立 Client）；文件关联 / 最近文件 | 预览策略钉死 |
| **P3** | 主题（QTE）、可选 Ribbon（QFR）；偏好持久化；拖出 Tab 跟 MPS 能力 | 更像「成品」 |
| **以后** | PDF 批注、同类型多实例隔离、更多格式插件 | 按真实需求再开 |

---

## 7. 架构草图

```text
┌─────────────────────────────────────────────────────────┐
│  Volition Host（MPS）                                     │
│  外框 · Tab · 启动器 · Embed · shell.ipc.v1               │
├───────────────┬─────────────────────┬───────────────────┤
│  Client:text  │  Client:markdown?   │  Client:pdf       │
│  txt/xml/(md) │  （仅重预览时独立）   │  PDF 渲染栈        │
│  编辑器内核    │  WebEngine 预览      │  独立崩溃域        │
└───────────────┴─────────────────────┴───────────────────┘
         ▲ 同类型多文档：默认同进程多子窗 / 多 Tab
```

**打开文件时的分流（示意）**：

```text
用户打开 path
  → 按扩展名映射 pageType
  → Host 确保对应 ClientSession 存活
  → CreateWindow / embed
  → Client 内按 mode 打开缓冲或文档模型
```

---

## 8. 开放问题（下次讨论再钉）

1. **Markdown 预览**：§5 倾向档 A（md4c + QTextBrowser）起步；是否接受 GFM/插件上限，还是首期就要 WebEngine？  
2. **PDF 引擎**：Qt PDF / PDFium / 其它商业栈？许可与崩溃域需一并评估。  
3. **文本编辑器内核**：`QPlainTextEdit` 起步，还是一上来就 Scintilla / QScintilla？  
4. **XML**：仅语法高亮，还是要树视图 / schema 校验？  
5. ~~显示名最终拍板~~：**Volition**（已定，2026-08-04）。  
6. ~~工程落点~~：独立仓 **Volition**（已建，`D:\Codes\Volition`）。  
7. **首期 UI**：纯 MPS Demo 式壳，还是一开始就接 QFR Ribbon？

---

## 9. 相关材料

| 材料 | 说明 |
|------|------|
| MPS 规格 | `MultiProcessShell/docs/zh/multiprocess-shell-spec.md` |
| Demo 形态 / IPC | `docs/zh/demo-morphology.md`、`demo-ipc.md`、`proto/shell/ipc/v1/ipc.proto` |
| 讨论出处 | 2026-07-25 会话：Client 拆分 + 命名；2026-08-01：Markdown 技术选型 |

---

## 10. 修订记录

| 日期 | 说明 |
|------|------|
| 2026-07-28 | 初稿：整理早期讨论为产品方案备忘，供后续开仓 / 规格细化 |
| 2026-08-04 | 产品名定为 **Volition**；仓库初始化 |
| 2026-08-01 | 新增 §5 Markdown 编辑与预览技术选型（md4c / QTextBrowser 起步，WebEngine 可选） |
