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
		[[nodiscard]] QString filePath() const
		{
			return m_filePath;
		}
		/// Load path into this view. Default: unsupported.
		virtual bool openPath(const QString& /*path*/)
		{
			return false;
		}
		/// Persist current content if backed by a path. Default: no-op success.
		virtual bool save()
		{
			return true;
		}
		[[nodiscard]] virtual bool isBlank() const
		{
			return m_filePath.isEmpty();
		}

	protected:
		void setFilePath(QString path)
		{
			m_filePath = std::move(path);
		}

	private:
		QString m_title;
		QString m_filePath;
	};
} // namespace volition

#endif // __VOLITION_CLIENT_DOCUMENT_VIEW_H__
