#include "client_run_helper.hpp"
#include "pdf_document_view.hpp"
#include "volition_client_export.hpp"

namespace
{
	volition::DocumentView* makePdfDocument(QWidget* parent)
	{
		return new volition::PdfDocumentView(parent);
	}
} // namespace

extern "C" VOLITION_CLIENT_EXPORT int VolitionClientRun(int argc, char** argv)
{
	return volition::runClientPlugin(argc, argv, QStringLiteral("pdf"), makePdfDocument);
}
