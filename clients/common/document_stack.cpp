#include "document_stack.hpp"

#include <QLabel>
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
		setTabsClosable(true);
		setDocumentMode(true);
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

	void DocumentStack::setDocumentFactory(DocumentFactory factory)
	{
		if (factory)
		{
			m_factory = std::move(factory);
		}
	}

	DocumentView* DocumentStack::addNewDocument(const QString& title)
	{
		if (!m_factory)
		{
			return nullptr;
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
} // namespace volition
