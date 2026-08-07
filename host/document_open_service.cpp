#include "document_open_service.hpp"

#include "client_kind.hpp"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

namespace volition::host
{
	DocumentOpenService::DocumentOpenService(mps::host::ShellApp* app, QObject* parent)
		: QObject(parent)
		, m_app(app)
	{
		if (!m_app)
		{
			return;
		}
		connect(m_app, &mps::host::ShellApp::appContentViewReady, this,
				[this](const QString& appName, qint64 tabId)
				{
					flushPendingForApp(appName, tabId);
				});
	}

	void DocumentOpenService::sendOpen(qint64 tabId, const QString& path)
	{
		if (!m_app || tabId == 0 || path.isEmpty())
		{
			return;
		}
		m_app->setTabTitle(tabId, QFileInfo(path).fileName());
		m_app->invokeOnTab(tabId, QStringLiteral("volition.open_document"), path.toUtf8());
	}

	void DocumentOpenService::flushPendingForApp(const QString& appName, qint64 tabId)
	{
		for (int i = 0; i < m_pending.size();)
		{
			if (m_pending[i].appName != appName)
			{
				++i;
				continue;
			}
			const QString path = m_pending[i].path;
			m_pending.removeAt(i);
			sendOpen(tabId, path);
			return;
		}
	}

	void DocumentOpenService::openPath(mps::host::ShellWindow* shell, const QString& path)
	{
		if (!m_app || !shell || path.isEmpty())
		{
			return;
		}
		const QString kind = clientKindForPath(path);
		if (kind.isEmpty())
		{
			QMessageBox::warning(shell, QStringLiteral("Volition"),
								 QStringLiteral("Unsupported file type:\n%1").arg(QFileInfo(path).fileName()));
			return;
		}
		if (!QFileInfo::exists(path))
		{
			QMessageBox::warning(shell, QStringLiteral("Volition"), QStringLiteral("File not found:\n%1").arg(path));
			return;
		}

		const qint64 existing = m_app->findTabIdForApp(kind);
		if (existing != 0)
		{
			sendOpen(existing, path);
			if (mps::host::ShellWindow* owner = m_app->shellForTab(existing))
			{
				m_app->activateTab(owner, existing);
			}
			return;
		}

		m_pending.push_back(Pending{kind, path, shell});
		m_app->createClientOn(shell, kind);
	}

	void DocumentOpenService::openWithDialog(mps::host::ShellWindow* shell)
	{
		if (!shell)
		{
			return;
		}
		const QString path = QFileDialog::getOpenFileName(shell, QStringLiteral("Open Document"), QString(), openFileDialogFilter());
		if (!path.isEmpty())
		{
			openPath(shell, path);
		}
	}
} // namespace volition::host
