# Volition

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

多类型文档 **阅读 / 编辑器**：以 [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（MPS）为壳，按技术栈与崩溃域拆 Client（`text` / `pdf`，Markdown 视预览方案定是否独立）。

首期平台：**Windows（MPS 形态 A）**。

> English overview：[docs/en/README.md](docs/en/README.md)

## 特性（规划）

- **多格式同壳**：PDF、Markdown、txt、XML 等在同一 Host 窗口多 Tab 打开
- **进程隔离**：`text`（txt / xml / 轻量 Markdown）与 `pdf` 分 Client；重预览时可再拆 `markdown`
- **壳托管**：外框 / Tab / 嵌入 / IPC 交给 MPS；业务窗口在 Client 进程内
- **主题可选**：外观可接 [QThemeEngine](https://github.com/yanxijian/QThemeEngine)；命令条可选 [QFluentRibbon](https://github.com/yanxijian/QFluentRibbon)

## 要求

| 项 | 说明 |
|----|------|
| Qt | **6.8+** Widgets |
| 框架 | MultiProcessShell（旁路源码或已安装 prefix） |
| 工具链 | CMake 3.21+、Ninja；Windows 上 MSVC x64（`vcvars`） |
| 可选 | QThemeEngine / QFluentRibbon；`clang-format` 20 |

## 现状

| 能力 | 状态 |
|------|------|
| 产品定名 **Volition**、仓库初始化 | 完成 |
| 早期产品方案（Client 拆分 / Markdown 选型） | 见 [product-plan.md](docs/zh/product-plan.md) |
| Host / `text` / `pdf` Client 实现 | 未开始 |

## 仓库布局

```text
host/              Volition Host（MPS 壳编排，待实现）
clients/text/      text Client（txt / xml / Markdown mode）
clients/pdf/       pdf Client
cmake/             CMake 辅助
scripts/           构建 / 部署脚本
docs/zh|en/        中英文文档
```

## 文档

| 主题 | 中文（主） | English |
|------|------------|---------|
| 早期产品方案 | [product-plan.md](docs/zh/product-plan.md) | — |
| English overview | — | [README.md](docs/en/README.md) |

**约定**：日常以中文文档为准；英文为同步译本。产品边界以 [product-plan.md](docs/zh/product-plan.md) 为准，直至正式规格落地。

## License

Released under the [MIT License](LICENSE).
