# Volition

**简体中文** · [English](README.md) · [日本語](README.ja.md) · [한국어](README.ko.md)

[![CI](https://github.com/yanxijian/Volition/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/Volition/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-6.8%2B-green.svg)](https://www.qt.io/)

<!-- translated-from: README.md sha256:03e35b8da216 -->


多类型文档 **阅读 / 编辑器**：以 [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（MPS）为壳，按格式拆 Client（`text` / `markdown` / `pdf`）。每种 Client = **薄 exe + 业务 DLL**（形态 A：Host 只负责启动 exe）。

首期平台：**Windows（MPS 形态 A）**。

## 特性（规划）

- **多格式同壳**：PDF、Markdown、txt、XML 等在同一 Host 窗口的多 Tab 中打开
- **进程隔离**：三个 Client 进程；业务代码在 DLL 里，由各自对应的薄 exe 加载
- **壳托管**：外框、Tab、窗口嵌入与 IPC 都交给 MPS
- **外观**：主题由 [QThemeEngine](https://github.com/yanxijian/QThemeEngine) 驱动，Client DLL 内使用 [QFluentRibbon](https://github.com/yanxijian/QFluentRibbon)
- **以后再做**（未经验证）：大文件异步 / 虚拟滚动、PDF 栅格化移出 UI 线程；仍以 AbseilPin 统一依赖。详见 [product-plan §10](docs/zh/product-plan.md)

## 要求

| 项 | 说明 |
|----|------|
| Qt | **6.8+** Widgets |
| 框架 | [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（旁路源码树或已安装的 prefix） |
| 工具链 | CMake 3.21+、Ninja；Windows 上使用 MSVC x64（`vcvars`） |
| PDF | [pdfium_all](https://github.com/yanxijian/pdfium_all)（staged `output` 或已安装包） |
| 格式 | `clang-format` 20；`python scripts/format_source.py` |

## 现状

| 能力 | 状态 |
|------|------|
| 产品定名 **Volition** | 完成 |
| 产品方案（三 Client / 薄 exe+DLL / 双层 Tab） | [product-plan.md](docs/zh/product-plan.md) |
| Host + text / markdown / pdf（薄 exe+DLL） | 可用 |
| 打开文件（后缀 → kind → Invoke） | 可用 |
| text 读写 + 查找 + XML 高亮 | 可用 |
| markdown 档 A（md4c → QTextBrowser） | 可用 |
| pdf 阅读（同进程 pdfium） | 可用（需旁路 [pdfium_all](https://github.com/yanxijian/pdfium_all)） |
| CI（format + Windows Qt） | [Actions](https://github.com/yanxijian/Volition/actions/workflows/ci.yml) |

文本 Client 目前按后缀支持 `.txt`、`.xml`、`.json`、`.ini`、`.log`、`.csv`；`Ctrl+F` 打开查找栏。单元测试通过 `-DVOLITION_BUILD_TESTS=ON` 启用。

## 仓库布局

```text
host/                 volition_host
clients/common/       VolitionClientRun 导出约定
clients/text/         volition_text.exe + .dll
clients/markdown/     volition_markdown.exe + .dll
clients/pdf/          volition_pdf.exe + .dll
cmake/                依赖解析
scripts/              构建 / 部署
docs/zh|en/
```

## 文档

设计文档目前只有英文与简体中文两版；这份 README 是四种语言之间保持同步的那份。

| 主题 | English | 简体中文 |
|------|---------|----------|
| 产品方案 | — | [product-plan.md](docs/zh/product-plan.md) |
| 工具链约定 | [toolchain.md](docs/en/toolchain.md) | [toolchain.md](docs/zh/toolchain.md) |
| Abseil / PDF 同进程路线 | [abseil-pin.md](docs/en/abseil-pin.md) | [abseil-pin.md](docs/zh/abseil-pin.md) |

## License

Released under the [MIT License](LICENSE).
