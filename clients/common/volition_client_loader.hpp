#ifndef __VOLITION_CLIENT_LOADER_H__
#define __VOLITION_CLIENT_LOADER_H__

#include <QString>

#ifdef Q_OS_WIN
#include <cstdio>
#include <windows.h>
#endif

namespace volition
{
	/// Load sibling volition_<kind>.dll next to the thin exe and call VolitionClientRun.
	/// Returns process exit code, or non-zero on load failure.
	inline int loadAndRunClientLibrary(const QString& libraryBaseName, int argc, char** argv)
	{
#ifdef Q_OS_WIN
		const QString dllName = libraryBaseName + QStringLiteral(".dll");
		const HMODULE mod = LoadLibraryW(reinterpret_cast<LPCWSTR>(dllName.utf16()));
		if (!mod)
		{
			const DWORD err = GetLastError();
			fwprintf(stderr, L"[volition] LoadLibraryW(%s) failed err=%lu\n", reinterpret_cast<LPCWSTR>(dllName.utf16()), err);
			return 3;
		}
		using RunFn = int (*)(int, char**);
		const auto run = reinterpret_cast<RunFn>(GetProcAddress(mod, "VolitionClientRun"));
		if (!run)
		{
			FreeLibrary(mod);
			return 4;
		}
		const int rc = run(argc, argv);
		// Keep module loaded for process lifetime (Qt/MPS objects may outlive return).
		(void)mod;
		return rc;
#else
		(void)libraryBaseName;
		(void)argc;
		(void)argv;
		return 2; // non-Windows loader TBD
#endif
	}
} // namespace volition

#endif // __VOLITION_CLIENT_LOADER_H__
