#include "document_open_service.hpp"

#include "client_kind.hpp"
#include "library_store.hpp"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QTimer>

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
		// Do not publish the title/path mapping until the Client confirms the
		// load. A failed open used to leave a filename tab that could never be
		// retried because the Host index still pointed at it.
		const QPointer<DocumentOpenService> self(this);
		m_app->invokeOnTab(tabId, QStringLiteral("volition.open_document"), absolute.toUtf8(),
						   [self, tabId, absolute](bool ok, const QByteArray&, const QString& error)
						   {
							   if (!self)
							   {
								   return;
							   }
							   if (!ok)
							   {
								   self->forgetTab(tabId);
								   if (auto* shell = self->m_app->shellForTab(tabId))
								   {
									   QMessageBox::warning(shell, DocumentOpenService::tr("Volition"),
															DocumentOpenService::tr("Failed to open:\n%1\n\n%2").arg(absolute, error));
								   }
								   return;
							   }
							   self->rememberOpen(tabId, absolute);
							   self->m_app->setTabTitle(tabId, QFileInfo(absolute).fileName());
						   });
	}

	void DocumentOpenService::flushPendingForApp(const QString& appName, qint64 tabId)
	{
		if (!m_app)
		{
			return;
		}
		// Only deliver to the window that produced this ContentView. Using the
		// oldest request for the app regardless of window put documents into the
		// wrong top-level window when several were being opened at once.
		mps::host::ShellWindow* targetShell = m_app->shellForTab(tabId);
		if (!targetShell)
		{
			return;
		}
		int pick = -1;
		for (int i = 0; i < m_pending.size();)
		{
			if (m_pending[i].appName != appName)
			{
				++i;
				continue;
			}
			if (m_pending[i].shell.isNull())
			{
				m_pending.removeAt(i);
				continue;
			}
			if (m_pending[i].shell.data() == targetShell)
			{
				pick = i;
				break;
			}
			++i;
		}
		if (pick < 0)
		{
			return;
		}
		const QString path = m_pending[pick].path;
		m_pending.removeAt(pick);
		sendOpen(tabId, path);
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
		// A launch can fail without ever producing appContentViewReady (no exe,
		// handshake timeout, queue dropped). Do not leave the request pending
		// forever with no feedback.
		const QPointer<DocumentOpenService> self(this);
		QTimer::singleShot(30000, this,
						   [self, kind, absolute]()
						   {
							   if (!self)
							   {
								   return;
							   }
							   for (int i = 0; i < self->m_pending.size(); ++i)
							   {
								   if (self->m_pending[i].appName == kind && self->m_pending[i].path == absolute)
								   {
									   self->m_pending.removeAt(i);
									   QMessageBox::warning(QApplication::activeWindow(), DocumentOpenService::tr("Volition"),
															DocumentOpenService::tr("Timed out opening:\n%1").arg(absolute));
									   return;
								   }
							   }
						   });
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
