# scripts/

| 脚本 | 说明 |
|------|------|
| `format_source.py` | 按 `.clang-format` 格式化手写 C/C++；写入后恢复 **UTF-8 BOM + CRLF**；`--check` 另验 BOM/换行与行宽 140 |

本地构建示例（旁路仓在 `D:\Codes\*`）：

```text
cmake -S . -B build -G Ninja ^
  -DVOLITION_BUILD_APPS=ON ^
  -DVOLITION_DEV_EMBED_MPS=ON ^
  -DVOLITION_DEV_EMBED_QTE=ON ^
  -DVOLITION_DEV_EMBED_QFR=ON
cmake --build build
```

格式化：

```text
python scripts\format_source.py
python scripts\format_source.py --check
```

产物：请运行 **`build/bin/volition_host.exe`**（不要跑 `build/host/` 下可能残留的旧 exe）。同目录含 Qt / MPS / QTE / QFR DLL。
