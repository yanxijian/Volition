# Volition

[![CI](https://github.com/yanxijian/Volition/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/Volition/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](../../LICENSE)

Multi-format document **reader / editor** on [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell) (MPS). Clients: `text` / `markdown` / `pdf`. Each Client = **thin exe + business DLL** (form A: Host only launches the exe).

Phase-1 platform: **Windows (MPS form A)**.

> Canonical docs are Chinese — [root README](../../README.md) and [product plan](../zh/product-plan.md).

## Planned features

- **Multi-format shell**: PDF, Markdown, txt, XML, … as Host tabs
- **Process isolation**: three Client processes; business UI in DLLs loaded by thin exes
- **Shell hosting**: chrome / tabs / embed / IPC via MPS
- **Look**: QThemeEngine; QFluentRibbon inside Client DLLs

## Requirements

| Item | Notes |
|------|--------|
| Qt | **6.8+** Widgets |
| Framework | MultiProcessShell |
| Toolchain | CMake 3.21+, Ninja; MSVC x64 on Windows |
| PDF | pdfium_all (staged `output` or install) |
| Format | `clang-format` 20; `python scripts/format_source.py` |

## Status

| Capability | Status |
|------------|--------|
| Product name **Volition** | Done |
| Product plan (3 clients / thin exe+DLL / dual-tab) | [product-plan.md](../zh/product-plan.md) |
| Host + text / markdown / pdf (thin exe+DLL) | Working |
| Open file (ext→kind → Invoke) | Working |
| text R/W + XML highlight | Working |
| markdown tier A (md4c → QTextBrowser) | Working |
| pdf viewer (pdfium under `bin/pdf/`) | Working (needs sibling pdfium_all) |
| CI (format + Windows Qt) | [Actions](https://github.com/yanxijian/Volition/actions/workflows/ci.yml) |

## Layout

```text
host/                 volition_host
clients/common/       VolitionClientRun export
clients/text/         volition_text.exe + .dll
clients/markdown/     volition_markdown.exe + .dll
clients/pdf/          volition_pdf.exe + .dll
cmake/                Dep helpers
scripts/              Build / deploy
docs/zh|en/
```

## Documentation

| Topic | 中文（主） | English |
|-------|------------|---------|
| Product plan | [../zh/product-plan.md](../zh/product-plan.md) | — |
| Toolchain contract | [../zh/toolchain.md](../zh/toolchain.md) | [toolchain.md](toolchain.md) |
| Abseil / pdfium staging | [../zh/abseil-pin.md](../zh/abseil-pin.md) | [abseil-pin.md](abseil-pin.md) |
| This overview | [../../README.md](../../README.md) | [README.md](README.md) |

## License

Released under the [MIT License](../../LICENSE).
