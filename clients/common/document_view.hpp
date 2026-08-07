#ifndef __VOLITION_CLIENT_DOCUMENT_VIEW_H__
#define __VOLITION_CLIENT_DOCUMENT_VIEW_H__

#include <QString>
#include <QWidget>

namespace volition
{
	/// One center-tab document surface (local to Client; not a Host Tab).
	class DocumentView : public QWidget
	{
	public:
		explicit DocumentView(QWidget* parent = nullptr)
			: QWidget(parent)
		{
		}

		void setDocumentTitle(QString title)
		{
			m_title = std::move(title);
		}
		[[nodiscard]] QString documentTitle() const
		{
			return m_title;
		}

	private:
		QString m_title;
	};
} // namespace volition

#endif // __VOLITION_CLIENT_DOCUMENT_VIEW_H__
