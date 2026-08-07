# Abseil / in-process pdfium

Volition PDF and MPS share one `abseil_dll`:

- [AbseilPin](https://github.com/yanxijian/AbseilPin) (local sibling `D:\Codes\AbseilPin`)
- Roadmap: `AbseilPin/docs/en/ROADMAP.md` (Chinese: `docs/zh/ROADMAP.md`)
- **Current**: `volition_pdf.dll` links product-path pdfium (`PDFIUM_ENABLE_V8=OFF`) and shares AbseilPin **`20260107.1`** with MPS
- **Optional OOP**: `-DVOLITION_PDF_OOP_RENDER=ON` with runtime `VOLITION_PDF_OOP=1`

MPS: FetchContent **protobuf v35.1**; preferred  
`-DMPS_ABSEIL_PIN_PREFIX=<AbseilPin/prefix/20260107.1>`  
(auto-detected when the sibling prefix exists).

Constraint: a single pin `abseil_dll.dll` under `build/bin/`; open/zoom PDF in-process by default.
