# Abseil / in-process pdfium (staging notes)

How Volition PDF and MPS share (or avoid sharing) `abseil_dll`:

- Repo: [AbseilPin](https://github.com/yanxijian/AbseilPin) (local sibling `D:\Codes\AbseilPin`)
- Roadmap: `AbseilPin/docs/en/ROADMAP.md` (Chinese canonical: `docs/zh/ROADMAP.md`)
- **Today**: `volition_pdf` does **not** link pdfium; rendering uses `bin/pdf/render/volition_pdf_render.exe`
- **After unification**: one pin-built `abseil_dll` (same STL ABI) → fold pdfium back into `volition_pdf.dll`

MPS (done): FetchContent **protobuf v35.1**, preferred  
`-DMPS_ABSEIL_PIN_PREFIX=<AbseilPin/prefix/20260107.1>`  
(CMake auto-detects that prefix when the sibling tree exists).
