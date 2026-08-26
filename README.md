# Volition

[![CI](https://github.com/yanxijian/Volition/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/Volition/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

多类型文档 **阅读 / 编辑器**：以 [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（MPS）为壳，按技术栈拆 Client（`text` / `markdown` / `pdf`）。每种 Client = **薄 exe + 业务 DLL**（形态 A：Host 只启 exe）。

首期平台：**Windows（MPS 形态 A）**。

> English overview：[docs/en/README.md](docs/en/README.md)

## 特性（规划）

- **多格式同壳**：PDF、Markdown、txt、XML 等在同一 Host 窗口多 Tab 打开
- **进程隔离**：三 Client 进程；业务在 DLL，由对应薄 exe 加载
- **壳托管**：外框 / Tab / 嵌入 / IPC 交给 MPS
- **主题 / Ribbon**：[QThemeEngine](https://github.com/yanxijian/QThemeEngine)；Client 内 [QFluentRibbon](https://github.com/yanxijian/QFluentRibbon)

## 要求

| 项 | 说明 |
|----|------|
| Qt | **6.8+** Widgets |
| 框架 | [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（旁路源码或已安装 prefix） |
| 工具链 | CMake 3.21+、Ninja；Windows 上 MSVC x64（`vcvars`） |
| PDF | [pdfium_all](https://github.com/yanxijian/pdfium_all)（staged `output` 或安装包） |
| 格式 | `clang-format` 20；`python scripts/format_source.py` |

## 现状

| 能力 | 状态 |
|------|------|
| 产品定名 **Volition** | 完成 |
| 产品方案（三 Client / 薄 exe+DLL / 双层 Tab） | [product-plan.md](docs/zh/product-plan.md) |
| Host + text / markdown / pdf（薄 exe+DLL） | 可用 |
| Open 文件（后缀→kind → Invoke 打开） | 可用 |
| text 读写 + 查找 + XML 高亮 | 可用 |
| markdown 档 A（md4c → QTextBrowser） | 可用 |
| pdf 阅读（同进程 pdfium） | 可用（需旁路 [pdfium_all](https://github.com/yanxijian/pdfium_all)） |
| CI（format + Windows Qt） | [Actions](https://github.com/yanxijian/Volition/actions/workflows/ci.yml) |

文本 Client 当前按后缀支持 `.txt`、`.xml`、`.json`、`.ini`、`.log`、`.csv`；按 `Ctrl+F` 可打开查找栏。单元测试可通过 `-DVOLITION_BUILD_TESTS=ON` 启用。

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

| 主题 | 中文（主） | English |
|------|------------|---------|
| 产品方案 | [product-plan.md](docs/zh/product-plan.md) | — |
| Abseil / PDF 同进程路线 | [abseil-pin.md](docs/zh/abseil-pin.md) | — |
| English overview | — | [README.md](docs/en/README.md) |

## License

Released under the [MIT License](LICENSE).
