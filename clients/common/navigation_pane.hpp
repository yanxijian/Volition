#ifndef __VOLITION_CLIENT_NAVIGATION_PANE_H__
#define __VOLITION_CLIENT_NAVIGATION_PANE_H__

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

namespace volition
{
	class NavigationPane final : public QFrame
	{
	public:
		explicit NavigationPane(QWidget* parent = nullptr)
			: QFrame(parent)
		{
			setObjectName(QStringLiteral("NavigationPane"));
			setFrameShape(QFrame::StyledPanel);
			auto* lay = new QVBoxLayout(this);
			auto* label = new QLabel(QStringLiteral("Navigation"), this);
			label->setAlignment(Qt::AlignCenter);
			lay->addStretch();
			lay->addWidget(label);
			lay->addStretch();
			setMinimumWidth(120);
			setMaximumWidth(280);
		}
	};
} // namespace volition

#endif // __VOLITION_CLIENT_NAVIGATION_PANE_H__
