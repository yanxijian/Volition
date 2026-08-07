#ifndef __VOLITION_CLIENT_EXPORT_H__
#define __VOLITION_CLIENT_EXPORT_H__

// Shared C ABI for thin Client exes loading volition_<kind>.dll.

#if defined(_WIN32)
#if defined(VOLITION_CLIENT_LIB_BUILD)
#define VOLITION_CLIENT_EXPORT __declspec(dllexport)
#else
#define VOLITION_CLIENT_EXPORT __declspec(dllimport)
#endif
#else
#if defined(VOLITION_CLIENT_LIB_BUILD)
#define VOLITION_CLIENT_EXPORT __attribute__((visibility("default")))
#else
#define VOLITION_CLIENT_EXPORT
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	/// Entry point implemented by each volition_<kind> shared library.
	/// Thin exe LoadLibrary + GetProcAddress("VolitionClientRun") then calls this.
	VOLITION_CLIENT_EXPORT int VolitionClientRun(int argc, char** argv);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __VOLITION_CLIENT_EXPORT_H__
