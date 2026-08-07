#include "client_run_helper.hpp"
#include "markdown_document_view.hpp"
#include "volition_client_export.hpp"

namespace
{
	volition::DocumentView* makeMarkdownDocument(QWidget* parent)
	{
		return new volition::MarkdownDocumentView(parent);
	}
} // namespace

extern "C" VOLITION_CLIENT_EXPORT int VolitionClientRun(int argc, char** argv)
{
	return volition::runClientPlugin(argc, argv, QStringLiteral("markdown"), makeMarkdownDocument,
									 QStringLiteral("Markdown (*.md *.markdown);;All files (*.*)"));
}
