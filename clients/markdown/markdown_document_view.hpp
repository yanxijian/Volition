#ifndef __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__
#define __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__

#include "document_view.hpp"

#include <QPlainTextEdit>
#include <QSplitter>
#include <QTextBrowser>

namespace volition
{
	class MarkdownDocumentView final : public DocumentView
	{
		Q_OBJECT
	public:
		explicit MarkdownDocumentView(QWidget* parent = nullptr);

		bool openPath(const QString& path) override;
		bool save() override;
		[[nodiscard]] bool isBlank() const override;

	private:
		void refreshPreview();

		QPlainTextEdit* m_editor = nullptr;
		QTextBrowser* m_preview = nullptr;
	};
} // namespace volition

#endif // __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__
