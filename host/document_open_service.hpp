#ifndef __VOLITION_HOST_DOCUMENT_OPEN_SERVICE_H__
#define __VOLITION_HOST_DOCUMENT_OPEN_SERVICE_H__

#include "shell_app.hpp"
#include "shell_window.hpp"

#include <QObject>
#include <QString>
#include <QVector>

namespace volition::host
{
	/// Routes Open path → clientKind → create/reuse session → Invoke volition.open_document.
	class DocumentOpenService final : public QObject
	{
		Q_OBJECT
	public:
		explicit DocumentOpenService(mps::host::ShellApp* app, QObject* parent = nullptr);

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

		mps::host::ShellApp* m_app = nullptr;
		QVector<Pending> m_pending;
	};
} // namespace volition::host

#endif // __VOLITION_HOST_DOCUMENT_OPEN_SERVICE_H__
