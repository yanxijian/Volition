#ifndef __VOLITION_PDF_LIB_PDF_DOCUMENT_VIEW_H__
#define __VOLITION_PDF_LIB_PDF_DOCUMENT_VIEW_H__

#include "document_view.hpp"

#include <QLabel>
#include <QVBoxLayout>

namespace volition
{
	/// Placeholder until pdfium_all rendering lands.
	class PdfDocumentView final : public DocumentView
	{
	public:
		explicit PdfDocumentView(QWidget* parent = nullptr)
			: DocumentView(parent)
		{
			auto* lay = new QVBoxLayout(this);
			auto* label = new QLabel(QStringLiteral("PdfDocumentView — pdfium render TBD"), this);
			label->setAlignment(Qt::AlignCenter);
			lay->addWidget(label);
		}
	};
} // namespace volition

#endif // __VOLITION_PDF_LIB_PDF_DOCUMENT_VIEW_H__
