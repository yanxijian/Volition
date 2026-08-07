#ifndef __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__
#define __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__

#include "document_view.hpp"

#include <QPlainTextEdit>
#include <QVBoxLayout>

namespace volition
{
	class TextDocumentView final : public DocumentView
	{
	public:
		explicit TextDocumentView(QWidget* parent = nullptr)
			: DocumentView(parent)
		{
			auto* lay = new QVBoxLayout(this);
			lay->setContentsMargins(0, 0, 0, 0);
			m_editor = new QPlainTextEdit(this);
			m_editor->setPlaceholderText(QStringLiteral("Text document…"));
			lay->addWidget(m_editor);
		}

		[[nodiscard]] QPlainTextEdit* editor() const
		{
			return m_editor;
		}

	private:
		QPlainTextEdit* m_editor = nullptr;
	};
} // namespace volition

#endif // __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__
