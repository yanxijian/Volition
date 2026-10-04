#ifndef __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__
#define __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__

#include "async_file_loader.hpp"
#include "document_view.hpp"

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QShortcut>
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
		Q_OBJECT
	public:
		explicit TextDocumentView(QWidget* parent = nullptr)
			: DocumentView(parent)
		{
			auto* lay = new QVBoxLayout(this);
			lay->setContentsMargins(0, 0, 0, 0);
			m_editor = new QPlainTextEdit(this);
			m_editor->setPlaceholderText(tr("Text document…"));
			m_editor->setLineWrapMode(QPlainTextEdit::NoWrap);
			m_editor->setTabStopDistance(m_editor->fontMetrics().horizontalAdvance(QLatin1Char(' ')) * 4);
			m_editor->installEventFilter(this);
			qApp->installEventFilter(this);

			auto* findBar = new QWidget(this);
			m_findBar = findBar;
			auto* findLayout = new QHBoxLayout(findBar);
			findLayout->setContentsMargins(4, 4, 4, 4);
			m_findEdit = new QLineEdit(findBar);
			m_findEdit->setPlaceholderText(tr("Find"));
			m_findStatus = new QLabel(findBar);
			auto* previousButton = new QPushButton(tr("Previous"), findBar);
			auto* nextButton = new QPushButton(tr("Next"), findBar);
			auto* closeButton = new QPushButton(tr("Close"), findBar);
			findLayout->addWidget(m_findEdit);
			findLayout->addWidget(m_findStatus);
			findLayout->addWidget(previousButton);
			findLayout->addWidget(nextButton);
			findLayout->addWidget(closeButton);
			findBar->setVisible(false);
			lay->addWidget(findBar);
			lay->addWidget(m_editor);

			connect(m_findEdit, &QLineEdit::textChanged, this,
					[this]()
					{
						findNext();
					});
			connect(previousButton, &QPushButton::clicked, this,
					[this]()
					{
						findPrevious();
					});
			connect(nextButton, &QPushButton::clicked, this,
					[this]()
					{
						findNext();
					});
			connect(closeButton, &QPushButton::clicked, findBar,
					[findBar]()
					{
						findBar->setVisible(false);
					});
			auto* findShortcut = new QShortcut(QKeySequence::Find, m_editor);
			findShortcut->setContext(Qt::WidgetWithChildrenShortcut);
			connect(findShortcut, &QShortcut::activated, this,
					[this]()
					{
						showFindBar();
					});

			m_loader = new AsyncFileLoader(this);
			connect(m_loader, &AsyncFileLoader::finished, this,
					[this](const QString& path, const QByteArray& utf8Bytes, const QString& error)
					{
						if (!error.isEmpty())
						{
							m_editor->setPlainText(tr("Failed to open: %1").arg(error));
							return;
						}
						m_editor->setPlainText(QString::fromUtf8(utf8Bytes));
						setFilePath(path);
						updateHighlighter(path);
					});
		}

		~TextDocumentView() override
		{
			if (m_loader)
			{
				m_loader->cancel();
			}
			qApp->removeEventFilter(this);
		}

		[[nodiscard]] QPlainTextEdit* editor() const
		{
			return m_editor;
		}

		void showFindBar() override
		{
			m_findBar->setVisible(true);
			m_findEdit->setFocus();
			m_findEdit->selectAll();
		}

		void activate() override
		{
			m_editor->setFocus(Qt::OtherFocusReason);
		}

		bool openPath(const QString& path) override
		{
			if (!QFileInfo::exists(path))
			{
				return false;
			}
			m_editor->setPlainText(tr("Loading…"));
			m_loader->start(path);
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
		bool eventFilter(QObject* watched, QEvent* event) override
		{
			if (event->type() == QEvent::KeyPress && isVisible())
			{
				auto* keyEvent = static_cast<QKeyEvent*>(event);
				if (keyEvent->key() == Qt::Key_F && keyEvent->modifiers().testFlag(Qt::ControlModifier))
				{
					showFindBar();
					return true;
				}
			}
			return DocumentView::eventFilter(watched, event);
		}

		void findNext()
		{
			if (m_findEdit->text().isEmpty())
			{
				m_findStatus->clear();
				return;
			}
			if (!m_editor->find(m_findEdit->text()))
			{
				m_editor->moveCursor(QTextCursor::Start);
				if (!m_editor->find(m_findEdit->text()))
				{
					m_findStatus->setText(tr("Not found"));
					return;
				}
			}
			m_findStatus->setText(tr("Found"));
		}

		void findPrevious()
		{
			if (m_findEdit->text().isEmpty())
			{
				m_findStatus->clear();
				return;
			}
			if (!m_editor->find(m_findEdit->text(), QTextDocument::FindBackward))
			{
				m_editor->moveCursor(QTextCursor::End);
				if (!m_editor->find(m_findEdit->text(), QTextDocument::FindBackward))
				{
					m_findStatus->setText(tr("Not found"));
					return;
				}
			}
			m_findStatus->setText(tr("Found"));
		}

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
		QWidget* m_findBar = nullptr;
		QLineEdit* m_findEdit = nullptr;
		QLabel* m_findStatus = nullptr;
		XmlSyntaxHighlighter* m_highlighter = nullptr;
		AsyncFileLoader* m_loader = nullptr;
	};
} // namespace volition

#endif // __VOLITION_TEXT_LIB_TEXT_DOCUMENT_VIEW_H__
