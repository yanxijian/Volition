#ifndef __VOLITION_CLIENT_UTILITY_PANE_H__
#define __VOLITION_CLIENT_UTILITY_PANE_H__

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

namespace volition
{
	class UtilityPane final : public QFrame
	{
	public:
		explicit UtilityPane(QWidget* parent = nullptr)
			: QFrame(parent)
		{
			setObjectName(QStringLiteral("UtilityPane"));
			setFrameShape(QFrame::StyledPanel);
			auto* lay = new QVBoxLayout(this);
			auto* label = new QLabel(QStringLiteral("Utility"), this);
			label->setAlignment(Qt::AlignCenter);
			lay->addStretch();
			lay->addWidget(label);
			lay->addStretch();
			setMinimumWidth(120);
			setMaximumWidth(280);
		}
	};
} // namespace volition

#endif // __VOLITION_CLIENT_UTILITY_PANE_H__
