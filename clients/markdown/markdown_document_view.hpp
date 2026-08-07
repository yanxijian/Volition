#ifndef __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__
#define __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__

#include "document_view.hpp"

#include <QLabel>
#include <QVBoxLayout>

namespace volition
{
	/// Placeholder until md4c → QTextBrowser lands.
	class MarkdownDocumentView final : public DocumentView
	{
	public:
		explicit MarkdownDocumentView(QWidget* parent = nullptr)
			: DocumentView(parent)
		{
			auto* lay = new QVBoxLayout(this);
			auto* label = new QLabel(QStringLiteral("MarkdownDocumentView — md4c preview TBD"), this);
			label->setAlignment(Qt::AlignCenter);
			lay->addWidget(label);
		}
	};
} // namespace volition

#endif // __VOLITION_MARKDOWN_LIB_MARKDOWN_DOCUMENT_VIEW_H__
