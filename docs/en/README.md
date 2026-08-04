# Volition

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](../../LICENSE)

Multi-format document **reader / editor** built on [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell) (MPS). Clients are split by tech stack and crash domain (`text` / `pdf`; optional separate `markdown` if preview uses WebEngine).

Phase-1 platform: **Windows (MPS form A)**.

> Canonical docs are Chinese — start at the [root README](../../README.md) and [product plan](../zh/product-plan.md).

## Planned features

- **Multi-format shell**: PDF, Markdown, txt, XML, … as tabs in one Host window
- **Process isolation**: `text` vs `pdf` Clients; heavy Markdown preview may become a third Client
- **Shell hosting**: chrome / tabs / embed / IPC via MPS; document UIs live in Clients
- **Optional look**: [QThemeEngine](https://github.com/yanxijian/QThemeEngine); optional [QFluentRibbon](https://github.com/yanxijian/QFluentRibbon)

## Requirements

| Item | Notes |
|------|--------|
| Qt | **6.8+** Widgets |
| Framework | MultiProcessShell (sibling source or installed prefix) |
| Toolchain | CMake 3.21+, Ninja; MSVC x64 (`vcvars`) on Windows |
| Optional | QThemeEngine / QFluentRibbon; `clang-format` 20 |

## Status

| Capability | Status |
|------------|--------|
| Product name **Volition**, repo bootstrap | Done |
| Early product plan (Client split / Markdown stack) | [product-plan.md](../zh/product-plan.md) (Chinese) |
| Host / `text` / `pdf` Clients | Not started |

## Layout

```text
host/              Volition Host (MPS orchestration; TBD)
clients/text/      text Client (txt / xml / Markdown mode)
clients/pdf/       pdf Client
cmake/             CMake helpers
scripts/           Build / deploy scripts
docs/zh|en/        Chinese + English docs
```

## Documentation

| Topic | 中文（主） | English |
|-------|------------|---------|
| Early product plan | [../zh/product-plan.md](../zh/product-plan.md) | — |
| This overview | [../../README.md](../../README.md) | [README.md](README.md) |

**Policy:** Prefer Chinese docs day-to-day. Product boundaries follow the Chinese product plan until a formal spec exists.

## License

Released under the [MIT License](../../LICENSE).
