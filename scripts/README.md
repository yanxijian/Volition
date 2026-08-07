# scripts/

| 脚本 | 说明 |
|------|------|
| `format_source.py` | 按 `.clang-format` 格式化手写 C/C++；写入后恢复 **UTF-8 BOM + CRLF**；`--check` 另验 BOM/换行与行宽 140 |
| `smoke_bin_layout.ps1` | 检查 `build/bin`：扁平布局、单份 `abseil_dll`、可选与 AbseilPin hash 一致、有 pdfium 时校验同进程依赖 |

本地构建示例（旁路仓在 `D:\Codes\*`）：

```text
cmake -S . -B build -G Ninja ^
  -DVOLITION_BUILD_APPS=ON ^
  -DVOLITION_DEV_EMBED_MPS=ON ^
  -DVOLITION_DEV_EMBED_QTE=ON ^
  -DVOLITION_DEV_EMBED_QFR=ON ^
  -DMPS_ABSEIL_PIN_PREFIX=D:/Codes/AbseilPin/prefix/20260107.1 ^
  -DVOLITION_PDFIUM_PREFIX=D:/Codes/pdfium_all/output
cmake --build build
powershell -File scripts\smoke_bin_layout.ps1 -BinDir build\bin -RequirePdfium
```

格式化：

```text
python scripts\format_source.py
python scripts\format_source.py --check
```

产物：运行 **`build/bin/volition_host.exe`**（同目录含 Qt / MPS / QTE / QFR 等运行时；共享 DLL 由 Host POST_BUILD 落盘一份）。
