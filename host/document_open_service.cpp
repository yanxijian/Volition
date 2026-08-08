#include "document_open_service.hpp"

#include "client_kind.hpp"
#include "library_store.hpp"

#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

namespace volition::host
{
	DocumentOpenService::DocumentOpenService(mps::host::ShellApp* app, LibraryStore* library, QObject* parent)
		: QObject(parent)
		, m_app(app)
		, m_library(library)
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

	qint64 DocumentOpenService::findOpenTabForPath(const QString& absolutePath) const
	{
		if (!m_app || absolutePath.isEmpty())
		{
			return 0;
		}
		const auto it = m_pathToTab.constFind(absolutePath);
		if (it == m_pathToTab.cend())
		{
			return 0;
		}
		const qint64 tabId = it.value();
		if (!m_app->shellForTab(tabId))
		{
			return 0;
		}
		return tabId;
	}

	void DocumentOpenService::forgetTab(qint64 tabId)
	{
		const auto it = m_tabToPath.constFind(tabId);
		if (it == m_tabToPath.cend())
		{
			return;
		}
		m_pathToTab.remove(it.value());
		m_tabToPath.erase(it);
	}

	void DocumentOpenService::rememberOpen(qint64 tabId, const QString& absolutePath)
	{
		if (tabId == 0 || absolutePath.isEmpty())
		{
			return;
		}
		forgetTab(tabId);
		if (const qint64 old = m_pathToTab.value(absolutePath, 0); old != 0 && old != tabId)
		{
			m_tabToPath.remove(old);
		}
		m_pathToTab.insert(absolutePath, tabId);
		m_tabToPath.insert(tabId, absolutePath);
	}

	void DocumentOpenService::sendOpen(qint64 tabId, const QString& path)
	{
		if (!m_app || tabId == 0 || path.isEmpty())
		{
			return;
		}
		const QString absolute = QFileInfo(path).absoluteFilePath();
		rememberOpen(tabId, absolute);
		m_app->setTabTitle(tabId, QFileInfo(absolute).fileName());
		m_app->invokeOnTab(tabId, QStringLiteral("volition.open_document"), absolute.toUtf8());
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
			QMessageBox::warning(shell, tr("Volition"), tr("Unsupported file type:\n%1").arg(QFileInfo(path).fileName()));
			return;
		}
		const QString absolute = QFileInfo(path).absoluteFilePath();
		if (!QFileInfo::exists(absolute))
		{
			QMessageBox::warning(shell, tr("Volition"), tr("File not found:\n%1").arg(path));
			return;
		}

		if (m_library)
		{
			m_library->recordOpened(absolute);
		}

		// Drop stale mappings (tab closed) before reuse check.
		if (const auto it = m_pathToTab.constFind(absolute); it != m_pathToTab.cend() && !m_app->shellForTab(it.value()))
		{
			forgetTab(it.value());
		}

		if (const qint64 existing = findOpenTabForPath(absolute); existing != 0)
		{
			if (mps::host::ShellWindow* owner = m_app->shellForTab(existing))
			{
				m_app->activateTab(owner, existing);
			}
			return;
		}

		m_pending.push_back(Pending{kind, absolute, shell});
		m_app->createClientOn(shell, kind);
	}

	void DocumentOpenService::openWithDialog(mps::host::ShellWindow* shell)
	{
		if (!shell)
		{
			return;
		}
		const QString path = QFileDialog::getOpenFileName(shell, tr("Open Document"), QString(), openFileDialogFilter());
		if (!path.isEmpty())
		{
			openPath(shell, path);
		}
	}
} // namespace volition::host
