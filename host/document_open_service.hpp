#ifndef __VOLITION_HOST_DOCUMENT_OPEN_SERVICE_H__
#define __VOLITION_HOST_DOCUMENT_OPEN_SERVICE_H__

#include "shell_app.hpp"
#include "shell_window.hpp"

#include <QHash>
#include <QObject>
#include <QString>
#include <QVector>

namespace volition::host
{
	class LibraryStore;

	/// Routes Open path → clientKind → new Host Tab (or activate if path already open).
	class DocumentOpenService final : public QObject
	{
		Q_OBJECT
	public:
		explicit DocumentOpenService(mps::host::ShellApp* app, LibraryStore* library = nullptr, QObject* parent = nullptr);

		void setLibraryStore(LibraryStore* library)
		{
			m_library = library;
		}

		void openPath(mps::host::ShellWindow* shell, const QString& path);
		void openWithDialog(mps::host::ShellWindow* shell);

	private:
		struct Pending
		{
			QString appName;
			QString path;
			mps::host::ShellWindow* shell = nullptr;
		};

		void flushPendingForApp(const QString& appName, qint64 tabId);
		void sendOpen(qint64 tabId, const QString& path);
		[[nodiscard]] qint64 findOpenTabForPath(const QString& absolutePath) const;
		void rememberOpen(qint64 tabId, const QString& absolutePath);
		void forgetTab(qint64 tabId);

		mps::host::ShellApp* m_app = nullptr;
		LibraryStore* m_library = nullptr;
		QVector<Pending> m_pending;
		QHash<QString, qint64> m_pathToTab;
		QHash<qint64, QString> m_tabToPath;
	};
} // namespace volition::host

#endif // __VOLITION_HOST_DOCUMENT_OPEN_SERVICE_H__
