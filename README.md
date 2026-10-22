# Volition

**English** · [简体中文](README.zh-CN.md) · [日本語](README.ja.md) · [한국어](README.ko.md)

[![CI](https://github.com/yanxijian/Volition/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/Volition/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-6.8%2B-green.svg)](https://www.qt.io/)

Multi-format document **reader / editor** on [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell) (MPS). One Client per format — `text`, `markdown`, `pdf`. Every Client is a **thin exe plus a business DLL** (form A: the Host only launches the exe).

Phase-1 platform: **Windows (MPS form A)**.

## Planned features

- **Many formats, one shell**: PDF, Markdown, txt, XML, … as tabs in the same Host window
- **Process isolation**: three Client processes; business logic lives in DLLs loaded by the matching thin exe
- **Shell hosting**: frame, tabs, embedding and IPC are left to MPS
- **Look**: [QThemeEngine](https://github.com/yanxijian/QThemeEngine) for theming, [QFluentRibbon](https://github.com/yanxijian/QFluentRibbon) inside the Client DLLs
- **Later, still unverified**: async/virtualised large text and Markdown; PDF raster off the UI thread; keep AbseilPin. Details: [product-plan §10](docs/zh/product-plan.md)

## Requirements

| Item | Notes |
|------|--------|
| Qt | **6.8+** Widgets |
| Framework | [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell) (sibling source tree or installed prefix) |
| Toolchain | CMake 3.21+, Ninja; MSVC x64 on Windows (`vcvars`) |
| PDF | [pdfium_all](https://github.com/yanxijian/pdfium_all) (staged `output` or installed package) |
| Format | `clang-format` 20; `python scripts/format_source.py` |

## Status

| Capability | Status |
|------------|--------|
| Product name **Volition** | Done |
| Product plan (three Clients / thin exe+DLL / dual-tab) | [product-plan.md](docs/zh/product-plan.md) |
| Host + text / markdown / pdf (thin exe+DLL) | Working |
| Open a file (extension → kind → Invoke) | Working |
| text read/write + search + XML highlighting | Working |
| markdown tier A (md4c → QTextBrowser) | Working |
| pdf viewer (in-process pdfium) | Working (needs sibling [pdfium_all](https://github.com/yanxijian/pdfium_all)) |
| CI (format + Windows Qt) | [Actions](https://github.com/yanxijian/Volition/actions/workflows/ci.yml) |

The text Client handles `.txt`, `.xml`, `.json`, `.ini`, `.log` and `.csv` by extension; `Ctrl+F` opens the search bar. Unit tests are enabled with `-DVOLITION_BUILD_TESTS=ON`.

## Layout

```text
host/                 volition_host
clients/common/       VolitionClientRun export contract
clients/text/         volition_text.exe + .dll
clients/markdown/     volition_markdown.exe + .dll
clients/pdf/          volition_pdf.exe + .dll
cmake/                dependency resolution
scripts/              build / deploy
docs/zh|en/
```

## Documentation

The design notes are written in English and Simplified Chinese only; this README is the document kept in sync across all four languages.

| Topic | English | 简体中文 |
|-------|---------|----------|
| Product plan | — | [product-plan.md](docs/zh/product-plan.md) |
| Toolchain contract | [toolchain.md](docs/en/toolchain.md) | [toolchain.md](docs/zh/toolchain.md) |
| Abseil / pdfium staging | [abseil-pin.md](docs/en/abseil-pin.md) | [abseil-pin.md](docs/zh/abseil-pin.md) |

## License

Released under the [MIT License](LICENSE).
