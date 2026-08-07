#include "workspace_layout.hpp"

namespace volition
{
	WorkspaceLayout::WorkspaceLayout(QWidget* parent)
		: QWidget(parent)
	{
		auto* outer = new QVBoxLayout(this);
		outer->setContentsMargins(0, 0, 0, 0);
		outer->setSpacing(0);

		auto* split = new QSplitter(Qt::Horizontal, this);
		m_nav = new NavigationPane(split);
		m_stack = new DocumentStack(split);
		m_utility = new UtilityPane(split);
		split->addWidget(m_nav);
		split->addWidget(m_stack);
		split->addWidget(m_utility);
		split->setStretchFactor(0, 0);
		split->setStretchFactor(1, 1);
		split->setStretchFactor(2, 0);
		split->setSizes({160, 640, 160});
		outer->addWidget(split);
	}
} // namespace volition
