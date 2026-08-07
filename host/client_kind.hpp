#ifndef __VOLITION_HOST_CLIENT_KIND_H__
#define __VOLITION_HOST_CLIENT_KIND_H__

#include <QFileInfo>
#include <QString>

namespace volition::host
{
	/// Map file extension → MPS/Volition appName (clientKind). Empty if unsupported.
	[[nodiscard]] inline QString clientKindForPath(const QString& path)
	{
		const QString suffix = QFileInfo(path).suffix().toLower();
		if (suffix == QLatin1String("txt") || suffix == QLatin1String("xml") || suffix == QLatin1String("json")
			|| suffix == QLatin1String("ini") || suffix == QLatin1String("log") || suffix == QLatin1String("csv"))
		{
			return QStringLiteral("text");
		}
		if (suffix == QLatin1String("md") || suffix == QLatin1String("markdown"))
		{
			return QStringLiteral("markdown");
		}
		if (suffix == QLatin1String("pdf"))
		{
			return QStringLiteral("pdf");
		}
		return {};
	}

	[[nodiscard]] inline QString openFileDialogFilter()
	{
		return QStringLiteral("Documents (*.txt *.xml *.md *.markdown *.pdf);;Text (*.txt *.xml *.json *.ini *.log *.csv);;"
							  "Markdown (*.md *.markdown);;PDF (*.pdf);;All files (*.*)");
	}

	/// Host tab kind label for a registered clientKind (`text` / `markdown` / `pdf`).
	[[nodiscard]] inline QString tabKindLabel(const QString& appName)
	{
		if (appName.compare(QLatin1String("text"), Qt::CaseInsensitive) == 0)
		{
			return QStringLiteral("Text");
		}
		if (appName.compare(QLatin1String("markdown"), Qt::CaseInsensitive) == 0)
		{
			return QStringLiteral("MD");
		}
		if (appName.compare(QLatin1String("pdf"), Qt::CaseInsensitive) == 0)
		{
			return QStringLiteral("PDF");
		}
		if (appName.isEmpty())
		{
			return QStringLiteral("Doc");
		}
		return appName.left(1).toUpper() + appName.mid(1);
	}

	/// Default Host tab title before a path is opened: `{Text|MD|PDF}-File{n}`.
	[[nodiscard]] inline QString defaultTabTitle(const QString& appName, int contentIndex)
	{
		return QStringLiteral("%1-File%2").arg(tabKindLabel(appName)).arg(contentIndex);
	}
} // namespace volition::host

#endif // __VOLITION_HOST_CLIENT_KIND_H__
