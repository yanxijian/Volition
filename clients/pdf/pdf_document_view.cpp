#include "pdf_document_view.hpp"

#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <QToolButton>
#include <QVBoxLayout>

#if defined(VOLITION_HAS_PDFIUM)
#include "public/fpdfview.h"
#endif

namespace volition
{
	PdfDocumentView::PdfDocumentView(QWidget* parent)
		: DocumentView(parent)
	{
		auto* lay = new QVBoxLayout(this);
		lay->setContentsMargins(0, 0, 0, 0);
		auto* bar = new QToolBar(this);
		auto* prev = new QToolButton(bar);
		prev->setText(QStringLiteral("Prev"));
		auto* next = new QToolButton(bar);
		next->setText(QStringLiteral("Next"));
		auto* zin = new QToolButton(bar);
		zin->setText(QStringLiteral("Zoom+"));
		auto* zout = new QToolButton(bar);
		zout->setText(QStringLiteral("Zoom-"));
		m_status = new QLabel(QStringLiteral("No PDF"), bar);
		bar->addWidget(prev);
		bar->addWidget(next);
		bar->addWidget(zin);
		bar->addWidget(zout);
		bar->addWidget(m_status);
		connect(prev, &QToolButton::clicked, this, &PdfDocumentView::goPrevPage);
		connect(next, &QToolButton::clicked, this, &PdfDocumentView::goNextPage);
		connect(zin, &QToolButton::clicked, this, &PdfDocumentView::zoomIn);
		connect(zout, &QToolButton::clicked, this, &PdfDocumentView::zoomOut);

		m_scroll = new QScrollArea(this);
		m_scroll->setWidgetResizable(true);
		m_pageLabel = new QLabel(m_scroll);
		m_pageLabel->setAlignment(Qt::AlignCenter);
		m_pageLabel->setMinimumSize(200, 200);
		m_scroll->setWidget(m_pageLabel);

		lay->addWidget(bar);
		lay->addWidget(m_scroll, 1);

#if defined(VOLITION_HAS_PDFIUM)
		static bool libraryReady = false;
		if (!libraryReady)
		{
			FPDF_InitLibrary();
			libraryReady = true;
		}
#else
		m_status->setText(QStringLiteral("pdfium not linked (build with staged pdfium_all)"));
#endif
	}

	PdfDocumentView::~PdfDocumentView()
	{
		closeDocument();
	}

	void PdfDocumentView::closeDocument()
	{
#if defined(VOLITION_HAS_PDFIUM)
		if (m_doc)
		{
			FPDF_CloseDocument(static_cast<FPDF_DOCUMENT>(m_doc));
			m_doc = nullptr;
		}
#endif
		m_pageCount = 0;
		m_pageIndex = 0;
		setFilePath({});
		if (m_pageLabel)
		{
			m_pageLabel->clear();
		}
	}

	bool PdfDocumentView::openPath(const QString& path)
	{
#if !defined(VOLITION_HAS_PDFIUM)
		Q_UNUSED(path);
		if (m_status)
		{
			m_status->setText(QStringLiteral("pdfium unavailable"));
		}
		return false;
#else
		closeDocument();
		const QByteArray pathUtf8 = path.toUtf8();
		FPDF_DOCUMENT doc = FPDF_LoadDocument(pathUtf8.constData(), nullptr);
		if (!doc)
		{
			if (m_status)
			{
				m_status->setText(QStringLiteral("Failed to open PDF"));
			}
			return false;
		}
		m_doc = doc;
		m_pageCount = FPDF_GetPageCount(doc);
		m_pageIndex = 0;
		setFilePath(path);
		renderCurrentPage();
		updateStatus();
		return true;
#endif
	}

	bool PdfDocumentView::isBlank() const
	{
		return filePath().isEmpty() && m_doc == nullptr;
	}

	void PdfDocumentView::goPrevPage()
	{
		if (m_pageIndex <= 0)
		{
			return;
		}
		--m_pageIndex;
		renderCurrentPage();
		updateStatus();
	}

	void PdfDocumentView::goNextPage()
	{
		if (m_pageIndex + 1 >= m_pageCount)
		{
			return;
		}
		++m_pageIndex;
		renderCurrentPage();
		updateStatus();
	}

	void PdfDocumentView::zoomIn()
	{
		m_zoom = qMin(4.0, m_zoom * 1.25);
		renderCurrentPage();
		updateStatus();
	}

	void PdfDocumentView::zoomOut()
	{
		m_zoom = qMax(0.25, m_zoom / 1.25);
		renderCurrentPage();
		updateStatus();
	}

	void PdfDocumentView::updateStatus()
	{
		if (!m_status)
		{
			return;
		}
		if (m_pageCount <= 0)
		{
			m_status->setText(QStringLiteral("No PDF"));
			return;
		}
		m_status->setText(QStringLiteral("Page %1 / %2  Zoom %3%").arg(m_pageIndex + 1).arg(m_pageCount).arg(qRound(m_zoom * 100.0)));
	}

	void PdfDocumentView::renderCurrentPage()
	{
#if !defined(VOLITION_HAS_PDFIUM)
		return;
#else
		if (!m_doc || !m_pageLabel)
		{
			return;
		}
		FPDF_PAGE page = FPDF_LoadPage(static_cast<FPDF_DOCUMENT>(m_doc), m_pageIndex);
		if (!page)
		{
			return;
		}
		const double pageW = FPDF_GetPageWidth(page);
		const double pageH = FPDF_GetPageHeight(page);
		const int width = qMax(1, qRound(pageW * m_zoom));
		const int height = qMax(1, qRound(pageH * m_zoom));
		QImage image(width, height, QImage::Format_RGBA8888);
		image.fill(Qt::white);
		FPDF_BITMAP bitmap = FPDFBitmap_CreateEx(width, height, FPDFBitmap_BGRA, image.bits(), image.bytesPerLine());
		if (bitmap)
		{
			FPDF_RenderPageBitmap(bitmap, page, 0, 0, width, height, 0, FPDF_ANNOT);
			FPDFBitmap_Destroy(bitmap);
			// pdfium BGRA → Qt RGBA8888 on little-endian is often BGRA physically; swap to RGB.
			m_pageLabel->setPixmap(QPixmap::fromImage(image.rgbSwapped()));
		}
		FPDF_ClosePage(page);
#endif
	}
} // namespace volition
