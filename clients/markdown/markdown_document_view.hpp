#ifndef __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__
#define __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__

#include "document_view.hpp"

#include <QPlainTextEdit>
#include <QSplitter>
#include <QTextBrowser>

class QTimer;

namespace volition
{
	class AsyncFileLoader;

	class MarkdownDocumentView final : public DocumentView
	{
		Q_OBJECT
	public:
		explicit MarkdownDocumentView(QWidget* parent = nullptr);

		bool openPath(const QString& path) override;
		bool save() override;
		[[nodiscard]] bool isBlank() const override;

	private:
		void schedulePreviewRefresh();
		void refreshPreviewNow();

		QPlainTextEdit* m_editor = nullptr;
		QTextBrowser* m_preview = nullptr;
		AsyncFileLoader* m_loader = nullptr;
		QTimer* m_previewTimer = nullptr;
		bool m_loading = false;
	};
} // namespace volition

#endif // __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__
