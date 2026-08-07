#ifndef __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__
#define __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__

#include "document_view.hpp"

#include <QColor>
#include <QFile>
#include <QFileInfo>
#include <QPlainTextEdit>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVBoxLayout>

namespace volition
{
	class XmlSyntaxHighlighter final : public QSyntaxHighlighter
	{
	public:
		explicit XmlSyntaxHighlighter(QTextDocument* parent)
			: QSyntaxHighlighter(parent)
		{
			m_tag.setForeground(QColor(QStringLiteral("#0000FF")));
			m_attr.setForeground(QColor(QStringLiteral("#FF0000")));
			m_value.setForeground(QColor(QStringLiteral("#008000")));
			m_comment.setForeground(QColor(QStringLiteral("#808080")));
			m_comment.setFontItalic(true);
		}

	protected:
		void highlightBlock(const QString& text) override
		{
			static const QRegularExpression tagRe(QStringLiteral("</?\\w+"));
			static const QRegularExpression attrRe(QStringLiteral("\\w+(?==)"));
			static const QRegularExpression valueRe(QStringLiteral("\"[^\"]*\"|'[^']*'"));
			static const QRegularExpression commentRe(QStringLiteral("<!--.*?-->"));

			for (auto it = tagRe.globalMatch(text); it.hasNext();)
			{
				const auto m = it.next();
				setFormat(m.capturedStart(), m.capturedLength(), m_tag);
			}
			for (auto it = attrRe.globalMatch(text); it.hasNext();)
			{
				const auto m = it.next();
				setFormat(m.capturedStart(), m.capturedLength(), m_attr);
			}
			for (auto it = valueRe.globalMatch(text); it.hasNext();)
			{
				const auto m = it.next();
				setFormat(m.capturedStart(), m.capturedLength(), m_value);
			}
			for (auto it = commentRe.globalMatch(text); it.hasNext();)
			{
				const auto m = it.next();
				setFormat(m.capturedStart(), m.capturedLength(), m_comment);
			}
		}

	private:
		QTextCharFormat m_tag;
		QTextCharFormat m_attr;
		QTextCharFormat m_value;
		QTextCharFormat m_comment;
	};

	class TextDocumentView final : public DocumentView
	{
	public:
		explicit TextDocumentView(QWidget* parent = nullptr)
			: DocumentView(parent)
		{
			auto* lay = new QVBoxLayout(this);
			lay->setContentsMargins(0, 0, 0, 0);
			m_editor = new QPlainTextEdit(this);
			m_editor->setPlaceholderText(QStringLiteral("Text document…"));
			m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
			m_editor->setTabStopDistance(m_editor->fontMetrics().horizontalAdvance(QLatin1Char(' ')) * 4);
			lay->addWidget(m_editor);
		}

		[[nodiscard]] QPlainTextEdit* editor() const
		{
			return m_editor;
		}

		bool openPath(const QString& path) override
		{
			QFile file(path);
			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				return false;
			}
			const QByteArray bytes = file.readAll();
			m_editor->setPlainText(QString::fromUtf8(bytes));
			setFilePath(path);
			updateHighlighter(path);
			return true;
		}

		bool save() override
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

		[[nodiscard]] bool isBlank() const override
		{
			return filePath().isEmpty() && m_editor && m_editor->toPlainText().isEmpty();
		}

	private:
		void updateHighlighter(const QString& path)
		{
			delete m_highlighter;
			m_highlighter = nullptr;
			if (QFileInfo(path).suffix().compare(QLatin1String("xml"), Qt::CaseInsensitive) == 0)
			{
				m_highlighter = new XmlSyntaxHighlighter(m_editor->document());
			}
		}

		QPlainTextEdit* m_editor = nullptr;
		XmlSyntaxHighlighter* m_highlighter = nullptr;
	};
} // namespace volition

#endif // __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__
