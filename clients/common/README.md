# clients/common/

各 Client **业务 DLL** 共用的导出约定。薄 exe 通过 `GetProcAddress`（或等价）调用。

## `VolitionClientRun`

```c
typedef int (*VolitionClientRunFn)(int argc, char** argv);
int VolitionClientRun(int argc, char** argv);
```

- 符号名：`VolitionClientRun`（`extern "C"`）
- 返回值：进程退出码（与 `main` 一致）
- DLL 内负责：`QApplication` / MPS `ClientApp` / `ContentViewFactory` / 主题等

导出宏见 `volition_client_export.hpp`（按 `VOLITION_<KIND>_LIB` 定义）。
