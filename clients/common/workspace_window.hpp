#ifndef __VOLITION_CLIENT_WORKSPACE_WINDOW_H__
#define __VOLITION_CLIENT_WORKSPACE_WINDOW_H__

#include "document_stack.hpp"
#include "qfluentribbon/ribbon_window.hpp"
#include "theme_scheme.hpp"
#include "workspace_layout.hpp"

#include <QString>

namespace qfluentribbon
{
	class ThemeBridge;
}

namespace volition
{
	/// Embedded root window: QFR Ribbon + three-pane workspace.
	class WorkspaceWindow final : public qfluentribbon::RibbonWindow
	{
		Q_OBJECT
	public:
		WorkspaceWindow(qint64 tabId, QString title, qfluentribbon::ThemeBridge* bridge, DocumentStack::DocumentFactory documentFactory,
						QWidget* parent = nullptr);

		void setDocumentNameFilters(QString filters);

		[[nodiscard]] qint64 tabId() const
		{
			return m_tabId;
		}
		[[nodiscard]] WorkspaceLayout* workspaceLayout() const
		{
			return m_layout;
		}
		[[nodiscard]] DocumentStack* documentStack() const
		{
			return m_layout ? m_layout->documentStack() : nullptr;
		}

		void realizeChrome();
		void syncAfterEmbed();

	signals:
		void requestNewContentView();
		void requestThemeScheme(mps::theme::Scheme scheme);
		/// Host tab label for the current document (typically the filename).
		void documentTitleChanged(const QString& title);

	protected:
		bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

	private:
		void buildRibbon(qfluentribbon::ThemeBridge* bridge);

		qint64 m_tabId = 0;
		bool m_embedSyncPending = false;
		bool m_embedSynced = false;
		bool m_chromeReady = false;
		qfluentribbon::ThemeBridge* m_pendingBridge = nullptr;
		DocumentStack::DocumentFactory m_documentFactory;
		QString m_nameFilters;
		WorkspaceLayout* m_layout = nullptr;
	};
} // namespace volition

#endif // __VOLITION_CLIENT_WORKSPACE_WINDOW_H__
