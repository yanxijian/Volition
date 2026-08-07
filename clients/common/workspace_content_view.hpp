#ifndef __VOLITION_CLIENT_WORKSPACE_CONTENT_VIEW_H__
#define __VOLITION_CLIENT_WORKSPACE_CONTENT_VIEW_H__

#include "content_view.hpp"
#include "document_stack.hpp"
#include "workspace_window.hpp"

#include <memory>

namespace qfluentribbon
{
	class ThemeBridge;
}

namespace qtheme
{
	class Engine;
}

namespace volition
{
	/// Shared ContentView adapter around WorkspaceWindow.
	class WorkspaceContentView : public mps::client::ContentView
	{
	public:
		WorkspaceContentView(qint64 tabId, const QString& title, qtheme::Engine* engine, qfluentribbon::ThemeBridge* bridge,
							 DocumentStack::DocumentFactory documentFactory);
		~WorkspaceContentView() override;

		[[nodiscard]] QWidget* widget() override;
		[[nodiscard]] qint64 tabId() const override;
		void realizeChrome() override;
		void syncAfterEmbed() override;
		void applyTheme(mps::theme::Scheme scheme) override;
		bool openDocument(const QString& path) override;

	protected:
		void syncRibbonTokens();

		qtheme::Engine* m_engine = nullptr;
		qfluentribbon::ThemeBridge* m_bridge = nullptr;
		std::unique_ptr<WorkspaceWindow> m_window;
	};
} // namespace volition

#endif // __VOLITION_CLIENT_WORKSPACE_CONTENT_VIEW_H__
