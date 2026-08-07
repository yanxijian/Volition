#include "client_run_helper.hpp"
#include "text_document_view.hpp"
#include "volition_client_export.hpp"

#include <QVBoxLayout>

namespace
{
	volition::DocumentView* makeTextDocument(QWidget* parent)
	{
		return new volition::TextDocumentView(parent);
	}
} // namespace

extern "C" VOLITION_CLIENT_EXPORT int VolitionClientRun(int argc, char** argv)
{
	return volition::runClientPlugin(argc, argv, QStringLiteral("text"), makeTextDocument);
}
