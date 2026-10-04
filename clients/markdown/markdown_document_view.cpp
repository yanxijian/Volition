#include "markdown_document_view.hpp"

#include "async_file_loader.hpp"

#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QVBoxLayout>

#include <md4c-html.h>
#include <string>

namespace volition
{
	namespace
	{
		void md4cAppend(const MD_CHAR* text, MD_SIZE size, void* userdata)
		{
			auto* out = static_cast<std::string*>(userdata);
			out->append(text, size);
		}

		QString markdownToHtml(const QString& markdown)
		{
			const QByteArray utf8 = markdown.toUtf8();
			std::string html;
			const int rc = md_html(utf8.constData(), static_cast<MD_SIZE>(utf8.size()), md4cAppend, &html, MD_DIALECT_GITHUB, 0);
			if (rc != 0)
			{
				return QStringLiteral("<pre>Markdown parse error</pre>");
			}
			return QString::fromUtf8(html.data(), static_cast<int>(html.size()));
		}
	} // namespace

	MarkdownDocumentView::MarkdownDocumentView(QWidget* parent)
		: DocumentView(parent)
	{
		auto* lay = new QVBoxLayout(this);
		lay->setContentsMargins(0, 0, 0, 0);
		auto* split = new QSplitter(Qt::Horizontal, this);
		m_editor = new QPlainTextEdit(split);
		m_editor->setPlaceholderText(QStringLiteral("Markdown…"));
		m_editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
		m_preview = new QTextBrowser(split);
		m_preview->setOpenExternalLinks(true);
		split->addWidget(m_editor);
		split->addWidget(m_preview);
		split->setStretchFactor(0, 1);
		split->setStretchFactor(1, 1);
		lay->addWidget(split);

		m_previewTimer = new QTimer(this);
		m_previewTimer->setSingleShot(true);
		m_previewTimer->setInterval(280);
		connect(m_previewTimer, &QTimer::timeout, this, &MarkdownDocumentView::refreshPreviewNow);

		connect(m_editor, &QPlainTextEdit::textChanged, this, &MarkdownDocumentView::schedulePreviewRefresh);

		m_loader = new AsyncFileLoader(this);
		connect(m_loader, &AsyncFileLoader::finished, this,
				[this](const QString& path, const QByteArray& utf8Bytes, const QString& error)
				{
					m_loading = false;
					if (!error.isEmpty())
					{
						m_editor->setPlainText(QStringLiteral("Failed to open: %1").arg(error));
						return;
					}
					m_editor->setPlainText(QString::fromUtf8(utf8Bytes));
					setFilePath(path);
					refreshPreviewNow();
				});

		refreshPreviewNow();
	}

	bool MarkdownDocumentView::openPath(const QString& path)
	{
		if (!QFileInfo::exists(path))
		{
			return false;
		}
		m_loading = true;
		m_previewTimer->stop();
		m_editor->setPlainText(QStringLiteral("Loading…"));
		m_preview->setHtml(QStringLiteral("<p>Loading…</p>"));
		m_loader->start(path);
		return true;
	}

	bool MarkdownDocumentView::save()
	{
		if (filePath().isEmpty())
		{
			return true;
		}
		QFile file(filePath());
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		{
			return false;
		}
		const QByteArray bytes = m_editor->toPlainText().toUtf8();
		return file.write(bytes) == bytes.size();
	}

	bool MarkdownDocumentView::isBlank() const
	{
		return filePath().isEmpty() && m_editor && m_editor->toPlainText().isEmpty();
	}

	void MarkdownDocumentView::schedulePreviewRefresh()
	{
		if (m_loading)
		{
			return;
		}
		const int chars = m_editor ? m_editor->document()->characterCount() : 0;
		if (chars > AsyncFileLoader::kAsyncThresholdBytes)
		{
			m_previewTimer->start();
			return;
		}
		refreshPreviewNow();
	}

	void MarkdownDocumentView::refreshPreviewNow()
	{
		if (!m_preview || !m_editor || m_loading)
		{
			return;
		}
		m_preview->setHtml(markdownToHtml(m_editor->toPlainText()));
	}
} // namespace volition
