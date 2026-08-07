// Thin Client exe: LoadLibrary sibling DLL then VolitionClientRun.
// Do not construct QApplication here — the business DLL owns the Qt app.
#include "volition_client_loader.hpp"

#include <QDir>
#include <QFileInfo>
#include <QString>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifndef VOLITION_CLIENT_DLL_BASENAME
#error "VOLITION_CLIENT_DLL_BASENAME must be set (e.g. volition_text)"
#endif

namespace
{
	QString executableDirectory()
	{
#ifdef Q_OS_WIN
		wchar_t path[MAX_PATH];
		const DWORD n = GetModuleFileNameW(nullptr, path, MAX_PATH);
		if (n == 0 || n >= MAX_PATH)
		{
			return QString();
		}
		return QFileInfo(QString::fromWCharArray(path)).absolutePath();
#else
		return QDir::currentPath();
#endif
	}

	void prependDllSearchPaths(const QString& exeDir)
	{
		if (exeDir.isEmpty())
		{
			return;
		}
		const QString parentDir = QDir(exeDir).absoluteFilePath(QStringLiteral(".."));
#ifdef Q_OS_WIN
		// exe dir first (pdfium private runtime under bin/pdf/), then parent bin/ (shared Qt/MPS).
		const QByteArray oldPath = qgetenv("PATH");
		const QByteArray prefix =
			(QDir::toNativeSeparators(exeDir) + QLatin1Char(';') + QDir::toNativeSeparators(parentDir) + QLatin1Char(';')).toLocal8Bit();
		qputenv("PATH", prefix + oldPath);
#else
		Q_UNUSED(parentDir);
#endif
	}
} // namespace

int main(int argc, char* argv[])
{
#ifdef Q_OS_WIN
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif
	const QString exeDir = executableDirectory();
	prependDllSearchPaths(exeDir);
	if (!exeDir.isEmpty())
	{
		QDir::setCurrent(exeDir);
	}

	const QString base = QStringLiteral(VOLITION_CLIENT_DLL_BASENAME);
	return volition::loadAndRunClientLibrary(base, argc, argv);
}
