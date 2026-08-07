#ifndef __VOLITION_CLIENT_DOCUMENT_STACK_H__
#define __VOLITION_CLIENT_DOCUMENT_STACK_H__

#include "document_view.hpp"

#include <QTabWidget>

#include <functional>
#include <memory>

namespace volition
{
	/// Client center-pane document surface container.
	/// One WorkspaceWindow maps to one document in the UI
	/// (`multiDocumentUiEnabled()`). Worksheet-like surfaces belong inside a
	/// DocumentView.
	class DocumentStack final : public QTabWidget
	{
		Q_OBJECT
	public:
		using DocumentFactory = std::function<DocumentView*(QWidget* parent)>;

		/// When false: hide tab chrome / "+" ; open/new reuse a single view.
		[[nodiscard]] static constexpr bool multiDocumentUiEnabled()
		{
			return false;
		}

		explicit DocumentStack(QWidget* parent = nullptr);

		void setDocumentFactory(DocumentFactory factory);
		void setNameFilters(QString filters);
		DocumentView* addNewDocument(const QString& title = QString());
		/// Open path into the current view (reuse blank when possible; create if empty).
		DocumentView* openDocument(const QString& path);
		void openDocumentWithDialog();
		bool saveCurrentDocument();

	signals:
		void documentCountChanged(int count);
		/// Current document label for the Host tab (typically the filename).
		void currentDocumentTitleChanged(const QString& title);

	private:
		DocumentFactory m_factory;
		QString m_nameFilters = QStringLiteral("All files (*.*)");
		int m_nextDocIndex = 1;
	};
} // namespace volition

#endif // __VOLITION_CLIENT_DOCUMENT_STACK_H__
