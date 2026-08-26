#include "document_stack.hpp"

#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QMessageBox>
#include <QTabBar>
#include <QToolButton>
#include <QVBoxLayout>

namespace volition
{
	namespace
	{
		DocumentView* makePlaceholderDocument(QWidget* parent)
		{
			auto* view = new DocumentView(parent);
			auto* lay = new QVBoxLayout(view);
			auto* label = new QLabel(QStringLiteral("Document"), view);
			label->setAlignment(Qt::AlignCenter);
			lay->addWidget(label);
			return view;
		}
	} // namespace

	DocumentStack::DocumentStack(QWidget* parent)
		: QTabWidget(parent)
		, m_factory(makePlaceholderDocument)
	{
		setDocumentMode(true);

		if (multiDocumentUiEnabled())
		{
			setTabsClosable(true);
			setMovable(true);

			auto* addBtn = new QToolButton(this);
			addBtn->setText(QStringLiteral("+"));
			addBtn->setAutoRaise(true);
			setCornerWidget(addBtn, Qt::TopRightCorner);
			connect(addBtn, &QToolButton::clicked, this,
					[this]()
					{
						addNewDocument();
					});
			connect(this, &QTabWidget::tabCloseRequested, this,
					[this](int index)
					{
						if (index < 0 || index >= count())
						{
							return;
						}
						QWidget* w = widget(index);
						removeTab(index);
						delete w;
						emit documentCountChanged(count());
					});
		}
		else if (QTabBar* bar = tabBar())
		{
			bar->hide();
			setTabsClosable(false);
			setMovable(false);
		}
	}

	void DocumentStack::setDocumentFactory(DocumentFactory factory)
	{
		if (factory)
		{
			m_factory = std::move(factory);
		}
	}

	void DocumentStack::setNameFilters(QString filters)
	{
		if (!filters.isEmpty())
		{
			m_nameFilters = std::move(filters);
		}
	}

	DocumentView* DocumentStack::addNewDocument(const QString& title)
	{
		if (!m_factory)
		{
			return nullptr;
		}
		if (!multiDocumentUiEnabled() && count() > 0)
		{
			auto* existing = dynamic_cast<DocumentView*>(currentWidget());
			if (existing && !title.isEmpty())
			{
				existing->setDocumentTitle(title);
				const int index = indexOf(existing);
				if (index >= 0)
				{
					setTabText(index, title);
				}
				emit currentDocumentTitleChanged(title);
			}
			return existing;
		}
		DocumentView* view = m_factory(this);
		if (!view)
		{
			return nullptr;
		}
		QString tabTitle = title;
		if (tabTitle.isEmpty())
		{
			tabTitle = QStringLiteral("Document%1").arg(m_nextDocIndex++);
		}
		view->setDocumentTitle(tabTitle);
		const int index = addTab(view, tabTitle);
		setCurrentIndex(index);
		emit documentCountChanged(count());
		return view;
	}

	DocumentView* DocumentStack::openDocument(const QString& path)
	{
		if (path.isEmpty() || !m_factory)
		{
			return nullptr;
		}
		DocumentView* target = dynamic_cast<DocumentView*>(currentWidget());
		if (!target)
		{
			target = addNewDocument(QFileInfo(path).fileName());
		}
		else if (multiDocumentUiEnabled() && !target->isBlank())
		{
			target = addNewDocument(QFileInfo(path).fileName());
		}
		if (!target)
		{
			return nullptr;
		}
		if (!target->openPath(path))
		{
			QMessageBox::warning(this, QStringLiteral("Volition"), QStringLiteral("Failed to open:\n%1").arg(path));
			return nullptr;
		}
		const QString name = QFileInfo(path).fileName();
		target->setDocumentTitle(name);
		const int index = indexOf(target);
		if (index >= 0)
		{
			setTabText(index, name);
			setCurrentIndex(index);
		}
		emit currentDocumentTitleChanged(name);
		return target;
	}

	void DocumentStack::openDocumentWithDialog()
	{
		const QString path = QFileDialog::getOpenFileName(this, QStringLiteral("Open Document"), QString(), m_nameFilters);
		if (!path.isEmpty())
		{
			openDocument(path);
		}
	}

	bool DocumentStack::saveCurrentDocument()
	{
		auto* view = dynamic_cast<DocumentView*>(currentWidget());
		if (!view)
		{
			return false;
		}
		if (!view->save())
		{
			QMessageBox::warning(this, QStringLiteral("Volition"), QStringLiteral("Failed to save document."));
			return false;
		}
		return true;
	}

	void DocumentStack::showFindBar()
	{
		if (auto* view = dynamic_cast<DocumentView*>(currentWidget()))
		{
			view->showFindBar();
		}
	}

	void DocumentStack::activateCurrentDocument()
	{
		if (auto* view = dynamic_cast<DocumentView*>(currentWidget()))
		{
			view->activate();
		}
	}
} // namespace volition
