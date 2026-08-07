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
} // namespace volition::host

#endif // __VOLITION_HOST_CLIENT_KIND_H__
