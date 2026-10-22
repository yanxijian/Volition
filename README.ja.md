# Volition

**日本語** · [English](README.md) · [简体中文](README.zh-CN.md) · [한국어](README.ko.md)

[![CI](https://github.com/yanxijian/Volition/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/Volition/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-6.8%2B-green.svg)](https://www.qt.io/)
[![CI](https://github.com/yanxijian/Volition/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/Volition/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

<!-- translated-from: README.md sha256:03e35b8da216 -->


[MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（MPS）をシェルにした、複数形式対応のドキュメント**閲覧／編集**アプリケーションです。形式ごとに Client を分けており（`text`／`markdown`／`pdf`）、各 Client は**薄い exe と業務 DLL**の組で構成されます（形態 A：Host は exe を起動するだけ）。

対象プラットフォーム（第 1 フェーズ）：**Windows（MPS 形態 A）**。

## 予定している機能

- **1 つのシェルで複数形式**：PDF・Markdown・txt・XML などを同じ Host ウィンドウのタブで開く
- **プロセス分離**：Client は 3 プロセス。業務ロジックは DLL に置き、対応する薄い exe が読み込む
- **シェル側のホスティング**：外枠・タブ・ウィンドウ埋め込み・IPC は MPS に任せる
- **外観**：テーマは [QThemeEngine](https://github.com/yanxijian/QThemeEngine)、Client DLL 内は [QFluentRibbon](https://github.com/yanxijian/QFluentRibbon)
- **後回し（未検証）**：大きなテキストと Markdown の非同期／仮想スクロール、PDF ラスタライズの UI スレッド外への移動。依存は引き続き AbseilPin で統一。詳細は [product-plan §10](docs/zh/product-plan.md)

## 動作要件

| 項目 | 内容 |
|------|------|
| Qt | **6.8+** Widgets |
| フレームワーク | [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)（隣接ソースツリー、またはインストール済み prefix） |
| ツールチェーン | CMake 3.21+、Ninja。Windows では MSVC x64（`vcvars`） |
| PDF | [pdfium_all](https://github.com/yanxijian/pdfium_all)（staged な `output`、またはインストール済みパッケージ） |
| 整形 | `clang-format` 20、`python scripts/format_source.py` |

## 現状

| 機能 | 状態 |
|------|------|
| 製品名 **Volition** | 決定済み |
| 製品計画（Client 3 種／薄い exe+DLL／2 階層タブ） | [product-plan.md](docs/zh/product-plan.md) |
| Host + text / markdown / pdf（薄い exe+DLL） | 動作確認済み |
| ファイルを開く（拡張子 → kind → Invoke） | 動作確認済み |
| text の読み書き＋検索＋XML ハイライト | 動作確認済み |
| markdown 段階 A（md4c → QTextBrowser） | 動作確認済み |
| pdf ビューア（同一プロセス内の pdfium） | 動作確認済み（隣接する [pdfium_all](https://github.com/yanxijian/pdfium_all) が必要） |
| CI（整形 + Windows Qt） | [Actions](https://github.com/yanxijian/Volition/actions/workflows/ci.yml) |

テキスト Client は現在、拡張子 `.txt`／`.xml`／`.json`／`.ini`／`.log`／`.csv` に対応しています。`Ctrl+F` で検索バーが開きます。単体テストは `-DVOLITION_BUILD_TESTS=ON` で有効になります。

## リポジトリ構成

```text
host/                 volition_host
clients/common/       VolitionClientRun のエクスポート規約
clients/text/         volition_text.exe + .dll
clients/markdown/     volition_markdown.exe + .dll
clients/pdf/          volition_pdf.exe + .dll
cmake/                依存関係の解決
scripts/              ビルド / 配置
docs/zh|en/
```

## ドキュメント

設計メモは英語と簡体字中国語の 2 言語のみで管理しています。この README が 4 言語で同期を保つ文書です。

| 内容 | English | 简体中文 |
|------|---------|----------|
| 製品計画 | — | [product-plan.md](docs/zh/product-plan.md) |
| ツールチェーンの取り決め | [toolchain.md](docs/en/toolchain.md) | [toolchain.md](docs/zh/toolchain.md) |
| Abseil / PDF 同一プロセス方針 | [abseil-pin.md](docs/en/abseil-pin.md) | [abseil-pin.md](docs/zh/abseil-pin.md) |

## ライセンス

[MIT License](LICENSE) で公開しています。
