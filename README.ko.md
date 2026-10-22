# Volition

**한국어** · [English](README.md) · [简体中文](README.zh-CN.md) · [日本語](README.ja.md)

[![CI](https://github.com/yanxijian/Volition/actions/workflows/ci.yml/badge.svg)](https://github.com/yanxijian/Volition/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-6.8%2B-green.svg)](https://www.qt.io/)

<!-- translated-from: README.md sha256:03e35b8da216 -->


[MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)(MPS)을 셸로 사용하는 멀티 포맷 문서 **뷰어/편집기**입니다. 포맷마다 Client를 나누어 두었고(`text` / `markdown` / `pdf`), 각 Client는 **얇은 exe와 비즈니스 DLL** 한 쌍으로 이루어집니다(형태 A: Host는 exe만 실행).

1단계 대상 플랫폼: **Windows (MPS 형태 A)**.

## 예정 기능

- **하나의 셸, 여러 포맷**: PDF, Markdown, txt, XML 등을 같은 Host 창의 탭에서 연다
- **프로세스 격리**: Client는 3개 프로세스. 비즈니스 로직은 DLL에 두고 대응하는 얇은 exe가 로드한다
- **셸 호스팅**: 외곽 프레임, 탭, 창 임베딩, IPC는 MPS가 맡는다
- **겉모습**: 테마는 [QThemeEngine](https://github.com/yanxijian/QThemeEngine), Client DLL 안에서는 [QFluentRibbon](https://github.com/yanxijian/QFluentRibbon)
- **나중에(미검증)**: 큰 텍스트와 Markdown의 비동기/가상 스크롤, PDF 래스터화를 UI 스레드 밖으로 이동. 의존성은 계속 AbseilPin으로 통일. 자세한 내용은 [product-plan §10](docs/zh/product-plan.md)

## 요구 사항

| 항목 | 내용 |
|------|------|
| Qt | **6.8+** Widgets |
| 프레임워크 | [MultiProcessShell](https://github.com/yanxijian/MultiProcessShell)(이웃 소스 트리 또는 설치된 prefix) |
| 툴체인 | CMake 3.21+, Ninja. Windows에서는 MSVC x64(`vcvars`) |
| PDF | [pdfium_all](https://github.com/yanxijian/pdfium_all)(staged `output` 또는 설치 패키지) |
| 서식 | `clang-format` 20, `python scripts/format_source.py` |

## 현재 상태

| 기능 | 상태 |
|------|------|
| 제품명 **Volition** | 확정 |
| 제품 계획(Client 3종 / 얇은 exe+DLL / 2단 탭) | [product-plan.md](docs/zh/product-plan.md) |
| Host + text / markdown / pdf(얇은 exe+DLL) | 동작함 |
| 파일 열기(확장자 → kind → Invoke) | 동작함 |
| text 읽기/쓰기 + 찾기 + XML 강조 | 동작함 |
| markdown 단계 A(md4c → QTextBrowser) | 동작함 |
| pdf 뷰어(같은 프로세스의 pdfium) | 동작함(이웃 [pdfium_all](https://github.com/yanxijian/pdfium_all) 필요) |
| CI(서식 + Windows Qt) | [Actions](https://github.com/yanxijian/Volition/actions/workflows/ci.yml) |

텍스트 Client는 현재 확장자 `.txt`, `.xml`, `.json`, `.ini`, `.log`, `.csv`를 지원하며, `Ctrl+F`로 찾기 창을 엽니다. 단위 테스트는 `-DVOLITION_BUILD_TESTS=ON`으로 켭니다.

## 저장소 구조

```text
host/                 volition_host
clients/common/       VolitionClientRun 내보내기 규약
clients/text/         volition_text.exe + .dll
clients/markdown/     volition_markdown.exe + .dll
clients/pdf/          volition_pdf.exe + .dll
cmake/                의존성 해석
scripts/              빌드 / 배포
docs/zh|en/
```

## 문서

설계 문서는 영어와 중국어 간체 두 언어로만 관리합니다. 이 README가 네 언어 사이에서 동기화를 유지하는 문서입니다.

| 주제 | English | 简体中文 |
|------|---------|----------|
| 제품 계획 | — | [product-plan.md](docs/zh/product-plan.md) |
| 툴체인 규약 | [toolchain.md](docs/en/toolchain.md) | [toolchain.md](docs/zh/toolchain.md) |
| Abseil / PDF 동일 프로세스 방침 | [abseil-pin.md](docs/en/abseil-pin.md) | [abseil-pin.md](docs/zh/abseil-pin.md) |

## 라이선스

[MIT License](LICENSE)로 배포합니다.
