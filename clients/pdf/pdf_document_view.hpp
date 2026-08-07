#ifndef __VOLITION_PDF_LIB_PDF_DOCUMENT_VIEW_H__
#define __VOLITION_PDF_LIB_PDF_DOCUMENT_VIEW_H__

#include "document_view.hpp"

#include <QLabel>
#include <QScrollArea>
#include <QStringList>
#include <QTemporaryDir>
#include <QToolBar>

namespace volition
{
	/// PDF viewer that shells out to volition_pdf_render (isolated pdfium / abseil).
	class PdfDocumentView final : public DocumentView
	{
		Q_OBJECT
	public:
		explicit PdfDocumentView(QWidget* parent = nullptr);
		~PdfDocumentView() override;

		bool openPath(const QString& path) override;
		[[nodiscard]] bool isBlank() const override;

	private slots:
		void goPrevPage();
		void goNextPage();
		void zoomIn();
		void zoomOut();

	private:
		[[nodiscard]] QString renderExePath() const;
		/// Run render helper; returns stdout on success, empty on failure (updates m_status).
		QByteArray runRenderHelper(const QStringList& args, int timeoutMs, const QString& failStatus);
		bool queryPageCount();
		void closeDocument();
		void renderCurrentPage();
		void updateStatus();

		QLabel* m_pageLabel = nullptr;
		QLabel* m_status = nullptr;
		QScrollArea* m_scroll = nullptr;
		QTemporaryDir m_renderTempDir;
		int m_pageCount = 0;
		int m_pageIndex = 0;
		double m_zoom = 1.25;
	};
} // namespace volition

#endif // __VOLITION_PDF_LIB_PDF_DOCUMENT_VIEW_H__
