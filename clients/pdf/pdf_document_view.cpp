#include "pdf_document_view.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QProcess>
#include <QToolButton>
#include <QVBoxLayout>

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
	}

	PdfDocumentView::~PdfDocumentView()
	{
		closeDocument();
	}

	QString PdfDocumentView::renderExePath() const
	{
		return QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("render/volition_pdf_render.exe"));
	}

	QByteArray PdfDocumentView::runRenderHelper(const QStringList& args, int timeoutMs, const QString& failStatus)
	{
		const QString exe = renderExePath();
		if (!QFileInfo::exists(exe))
		{
			if (m_status)
			{
				m_status->setText(QStringLiteral("render helper missing"));
			}
			return {};
		}
		QProcess proc;
		proc.setProgram(exe);
		proc.setArguments(args);
		proc.setWorkingDirectory(QFileInfo(exe).absolutePath());
		proc.start();
		if (!proc.waitForFinished(timeoutMs) || proc.exitCode() != 0)
		{
			if (m_status)
			{
				m_status->setText(failStatus);
			}
			return {};
		}
		return proc.readAllStandardOutput();
	}

	void PdfDocumentView::closeDocument()
	{
		m_pageCount = 0;
		m_pageIndex = 0;
		setFilePath({});
		if (m_pageLabel)
		{
			m_pageLabel->clear();
		}
	}

	bool PdfDocumentView::queryPageCount()
	{
		const QByteArray out =
			runRenderHelper({QStringLiteral("--file"), filePath(), QStringLiteral("--info")}, 60000, QStringLiteral("PDF info failed"))
				.trimmed();
		if (!out.startsWith("pages="))
		{
			return false;
		}
		bool ok = false;
		m_pageCount = out.mid(6).toInt(&ok);
		return ok && m_pageCount > 0;
	}

	bool PdfDocumentView::openPath(const QString& path)
	{
		closeDocument();
		if (!QFileInfo::exists(path))
		{
			return false;
		}
		if (!m_renderTempDir.isValid())
		{
			if (m_status)
			{
				m_status->setText(QStringLiteral("temp dir failed"));
			}
			return false;
		}
		setFilePath(path);
		if (!queryPageCount())
		{
			setFilePath({});
			return false;
		}
		m_pageIndex = 0;
		renderCurrentPage();
		updateStatus();
		return true;
	}

	bool PdfDocumentView::isBlank() const
	{
		return filePath().isEmpty() && m_pageCount == 0;
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
		if (filePath().isEmpty() || !m_pageLabel)
		{
			return;
		}
		const QString outBmp = m_renderTempDir.filePath(QStringLiteral("page_%1.bmp").arg(m_pageIndex));
		const QByteArray result =
			runRenderHelper({QStringLiteral("--file"), filePath(), QStringLiteral("--page"), QString::number(m_pageIndex),
							 QStringLiteral("--zoom"), QString::number(m_zoom, 'f', 4), QStringLiteral("--out"), outBmp},
							120000, QStringLiteral("render failed"));
		if (result.isNull() || !QFileInfo::exists(outBmp))
		{
			if (m_status && result.isNull() == false)
			{
				m_status->setText(QStringLiteral("render failed"));
			}
			return;
		}
		QPixmap px(outBmp);
		m_pageLabel->setPixmap(px);
		m_pageLabel->resize(px.size());
	}
} // namespace volition
