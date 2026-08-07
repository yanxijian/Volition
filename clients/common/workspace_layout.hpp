#ifndef __VOLITION_CLIENT_WORKSPACE_LAYOUT_H__
#define __VOLITION_CLIENT_WORKSPACE_LAYOUT_H__

#include "document_stack.hpp"
#include "navigation_pane.hpp"
#include "utility_pane.hpp"

#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

namespace volition
{
	class WorkspaceLayout final : public QWidget
	{
		Q_OBJECT
	public:
		explicit WorkspaceLayout(QWidget* parent = nullptr);

		[[nodiscard]] DocumentStack* documentStack() const
		{
			return m_stack;
		}
		[[nodiscard]] NavigationPane* navigationPane() const
		{
			return m_nav;
		}
		[[nodiscard]] UtilityPane* utilityPane() const
		{
			return m_utility;
		}

	private:
		NavigationPane* m_nav = nullptr;
		DocumentStack* m_stack = nullptr;
		UtilityPane* m_utility = nullptr;
	};
} // namespace volition

#endif // __VOLITION_CLIENT_WORKSPACE_LAYOUT_H__
