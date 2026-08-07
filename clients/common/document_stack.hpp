#ifndef __VOLITION_CLIENT_DOCUMENT_STACK_H__
#define __VOLITION_CLIENT_DOCUMENT_STACK_H__

#include "document_view.hpp"

#include <QTabWidget>

#include <functional>
#include <memory>

namespace volition
{
	/// Center-pane multi-document tabs (Client-local).
	class DocumentStack final : public QTabWidget
	{
		Q_OBJECT
	public:
		using DocumentFactory = std::function<DocumentView*(QWidget* parent)>;

		explicit DocumentStack(QWidget* parent = nullptr);

		void setDocumentFactory(DocumentFactory factory);
		DocumentView* addNewDocument(const QString& title = QString());

	signals:
		void documentCountChanged(int count);

	private:
		DocumentFactory m_factory;
		int m_nextDocIndex = 1;
	};
} // namespace volition

#endif // __VOLITION_CLIENT_DOCUMENT_STACK_H__
