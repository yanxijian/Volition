#ifndef __VOLITION_HOST_HOME_CONTENT_H__
#define __VOLITION_HOST_HOME_CONTENT_H__

#include <QWidget>

namespace mps::host
{
	class ShellApp;
	class ShellWindow;
} // namespace mps::host

namespace volition::host
{
	class DocumentOpenService;

	/// Home client area: Open + Create text/markdown/pdf + Light/Dark.
	class HomeContent final : public QWidget
	{
		Q_OBJECT
	public:
		HomeContent(mps::host::ShellApp* app, mps::host::ShellWindow* shell, DocumentOpenService* openService, QWidget* parent = nullptr);
	};
} // namespace volition::host

#endif // __VOLITION_HOST_HOME_CONTENT_H__
