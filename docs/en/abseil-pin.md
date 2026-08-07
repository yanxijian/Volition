# Abseil / in-process pdfium (staging notes)

How Volition PDF and MPS share `abseil_dll`:

- Repo: [AbseilPin](https://github.com/yanxijian/AbseilPin) (local sibling `D:\Codes\AbseilPin`)
- Roadmap: `AbseilPin/docs/en/ROADMAP.md` (Chinese canonical: `docs/zh/ROADMAP.md`)
- **Stage 2 (current)**: `volition_pdf.dll` links product-path pdfium (`PDFIUM_ENABLE_V8=OFF`) and shares AbseilPin `20260107.1` `abseil_dll.dll` with MPS
- OOP: off by default; debug with `-DVOLITION_PDF_OOP_RENDER=ON` and runtime `VOLITION_PDF_OOP=1`

MPS: FetchContent **protobuf v35.1**, preferred  
`-DMPS_ABSEIL_PIN_PREFIX=<AbseilPin/prefix/20260107.1>`  
(CMake auto-detects that prefix when the sibling tree exists).

Accept: a single pin `abseil_dll.dll` under `bin/`, open/zoom PDF without `render/volition_pdf_render.exe`.
