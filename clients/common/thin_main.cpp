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
		const QString parentDir = QFileInfo(QDir(exeDir).absoluteFilePath(QStringLiteral(".."))).absoluteFilePath();
#ifdef Q_OS_WIN
		// exe dir first, then parent (shared layout fallback).
		const QByteArray oldPath = qgetenv("PATH");
		const QByteArray prefix =
			(QDir::toNativeSeparators(exeDir) + QLatin1Char(';') + QDir::toNativeSeparators(parentDir) + QLatin1Char(';')).toLocal8Bit();
		qputenv("PATH", prefix + oldPath);
#else
		Q_UNUSED(parentDir);
#endif
	}

	/// Prefer plugins next to this exe; also search parent (legacy nested layouts).
	void prependQtPluginSearchRoots(const QString& exeDir)
	{
		if (exeDir.isEmpty())
		{
			return;
		}
		const QString parentDir = QFileInfo(QDir(exeDir).absoluteFilePath(QStringLiteral(".."))).absoluteFilePath();
		// QT_PLUGIN_PATH entries are roots that contain platforms/, styles/, etc.
		const QByteArray old = qgetenv("QT_PLUGIN_PATH");
		const QByteArray prefix =
			(QDir::toNativeSeparators(parentDir) + QLatin1Char(';') + QDir::toNativeSeparators(exeDir) + QLatin1Char(';')).toLocal8Bit();
		qputenv("QT_PLUGIN_PATH", prefix + old);
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
	prependQtPluginSearchRoots(exeDir);
	if (!exeDir.isEmpty())
	{
		QDir::setCurrent(exeDir);
	}

	const QString base = QStringLiteral(VOLITION_CLIENT_DLL_BASENAME);
	return volition::loadAndRunClientLibrary(base, argc, argv);
}
