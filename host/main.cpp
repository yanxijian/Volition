#include "client_kind.hpp"
#include "document_open_service.hpp"
#include "home_content.hpp"
#include "language_service.hpp"
#include "library_store.hpp"
#include "shell_app.hpp"
#include "theme_service.hpp"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace
{
	QString besideHost(const QString& fileName)
	{
		return QDir(QCoreApplication::applicationDirPath()).filePath(fileName);
	}
} // namespace

int main(int argc, char* argv[])
{
#ifdef Q_OS_WIN
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName(QStringLiteral("yanxijian"));
	QCoreApplication::setApplicationName(QStringLiteral("volition_host"));

	volition::host::LanguageService language;
	language.start(&app);

	volition::host::ThemeService theme;
	theme.start(&app);

	const QString textExe = besideHost(QStringLiteral("volition_text.exe"));
	const QString mdExe = besideHost(QStringLiteral("volition_markdown.exe"));
	const QString pdfExe = besideHost(QStringLiteral("volition_pdf.exe"));
	for (const QString& exe : {textExe, mdExe, pdfExe})
	{
		if (!QFileInfo::exists(exe))
		{
			qWarning("Client executable not found: %s", qPrintable(exe));
		}
	}

	mps::host::ShellApp shellApp(textExe, QStringLiteral("volition"));
	shellApp.registerClientLauncher(QStringLiteral("text"), textExe);
	shellApp.registerClientLauncher(QStringLiteral("markdown"), mdExe);
	shellApp.registerClientLauncher(QStringLiteral("pdf"), pdfExe);
	shellApp.setShellWindowTitle(QStringLiteral("Volition"));
	shellApp.setRequestNewContentViewMethod(QStringLiteral("volition.request_new_window"));
	shellApp.setTabTitleFactory(
		[](const QString& appName, int contentIndex)
		{
			return volition::host::defaultTabTitle(appName, contentIndex);
		});

	volition::host::LibraryStore library;
	volition::host::DocumentOpenService openService(&shellApp, &library);
	shellApp.setHomeContentFactory(
		[&shellApp, &openService, &library, &language](mps::host::ShellWindow* shell) -> QWidget*
		{
			return new volition::host::HomeContent(&shellApp, shell, &openService, &library, &language);
		});
	QObject::connect(&shellApp, &mps::host::ShellApp::schemeChanged, &theme,
					 [&theme](mps::theme::Scheme scheme, mps::host::ThemeOrigin origin)
					 {
						 theme.applyScheme(scheme);
						 if (origin != mps::host::ThemeOrigin::Startup)
						 {
							 theme.persist(scheme);
						 }
					 });
	shellApp.setScheme(theme.scheme(), mps::host::ThemeOrigin::Startup);
	(void)shellApp.createShell();
	return app.exec();
}
