#include "workspace_window.hpp"

#include "qfluentribbon/qfluentribbon.hpp"

#include <QAction>
#include <QStyle>
#include <QTabBar>
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace volition
{
	namespace
	{
		QAction* makeAction(QWidget* parent, const QString& id, const QString& text, QStyle::StandardPixmap icon, const QString& tipBody)
		{
			auto* action = new QAction(text, parent);
			action->setObjectName(id);
			action->setIcon(parent->style()->standardIcon(icon));
			qfluentribbon::ScreenTip::set(action, text, tipBody);
			return action;
		}
	} // namespace

	WorkspaceWindow::WorkspaceWindow(qint64 tabId, QString title, qfluentribbon::ThemeBridge* bridge,
									 DocumentStack::DocumentFactory documentFactory, QWidget* parent)
		: qfluentribbon::RibbonWindow(parent)
		, m_tabId(tabId)
		, m_pendingBridge(bridge)
		, m_documentFactory(std::move(documentFactory))
	{
		setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_DeleteOnClose, false);
		setAttribute(Qt::WA_NativeWindow);
		setWindowTitle(title);
		setMinimumSize(0, 0);
		resize(960, 640);
	}

	void WorkspaceWindow::realizeChrome()
	{
		if (m_chromeReady || !m_pendingBridge)
		{
			return;
		}
		setThemeBridge(m_pendingBridge);
		buildRibbon(m_pendingBridge);
		m_pendingBridge = nullptr;
		m_chromeReady = true;
	}

	bool WorkspaceWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
	{
#ifdef Q_OS_WIN
		if (eventType == QByteArrayLiteral("windows_generic_MSG") || eventType == QByteArrayLiteral("windows_dispatcher_MSG"))
		{
			const auto* msg = static_cast<const MSG*>(message);
			if (msg && (msg->message == WM_WINDOWPOSCHANGED || msg->message == WM_SIZE))
			{
				if (!m_embedSyncPending)
				{
					m_embedSyncPending = true;
					QTimer::singleShot(0, this,
									   [this]
									   {
										   m_embedSyncPending = false;
										   syncAfterEmbed();
									   });
				}
			}
		}
#else
		Q_UNUSED(eventType);
		Q_UNUSED(message);
		Q_UNUSED(result);
#endif
		return qfluentribbon::RibbonWindow::nativeEvent(eventType, message, result);
	}

	void WorkspaceWindow::syncAfterEmbed()
	{
#ifdef Q_OS_WIN
		const HWND hwnd = reinterpret_cast<HWND>(winId());
		if (!hwnd || !IsWindow(hwnd) || !GetParent(hwnd))
		{
			return;
		}

		RECT rc{};
		GetClientRect(hwnd, &rc);
		const int physW = qMax(1, static_cast<int>(rc.right - rc.left));
		const int physH = qMax(1, static_cast<int>(rc.bottom - rc.top));
		const qreal dpr = qMax(qreal(1), devicePixelRatioF());
		const int logicalW = qMax(1, qRound(static_cast<qreal>(physW) / dpr));
		const int logicalH = qMax(1, qRound(static_cast<qreal>(physH) / dpr));
		const QSize target(logicalW, logicalH);
		const bool sizeChanged = size() != target;
		if (sizeChanged)
		{
			resize(target);
		}

		if (!m_embedSynced || sizeChanged)
		{
			m_embedSynced = true;
			resize(target + QSize(1, 0));
			resize(target);
			if (ribbonBar())
			{
				ribbonBar()->polishFromStore();
			}
			if (testAttribute(Qt::WA_DontShowOnScreen))
			{
				setAttribute(Qt::WA_DontShowOnScreen, false);
				show();
			}
			repaint();
		}
#endif
	}

	void WorkspaceWindow::buildRibbon(qfluentribbon::ThemeBridge* bridge)
	{
		m_layout = new WorkspaceLayout(this);
		if (m_documentFactory && m_layout->documentStack())
		{
			m_layout->documentStack()->setDocumentFactory(m_documentFactory);
			m_layout->documentStack()->addNewDocument();
		}
		setCentralWidget(m_layout);

		auto* ribbon = ribbonBar();
		auto* home = ribbon->addTab(QStringLiteral("Home"));
		auto* view = ribbon->addTab(QStringLiteral("View"));
		if (QTabBar* tabs = ribbon->tabBar())
		{
			tabs->setTabData(0, QStringLiteral("H"));
			tabs->setTabData(1, QStringLiteral("V"));
		}

		auto* newWindow = makeAction(this, QStringLiteral("window.new"), QStringLiteral("New Window"), QStyle::SP_FileDialogNewFolder,
									 QStringLiteral("New Host workspace tab (CreateSubWindow)."));
		auto* newDoc = makeAction(this, QStringLiteral("document.new"), QStringLiteral("New Document"), QStyle::SP_FileIcon,
								  QStringLiteral("New center-pane document tab."));
		auto* light = makeAction(this, QStringLiteral("theme.light"), QStringLiteral("Light"), QStyle::SP_DialogApplyButton,
								 QStringLiteral("Fluent Light."));
		auto* dark =
			makeAction(this, QStringLiteral("theme.dark"), QStringLiteral("Dark"), QStyle::SP_ComputerIcon, QStringLiteral("Fluent Dark."));

		auto* windowGroup = home->addGroup(QStringLiteral("Window"));
		(void)windowGroup->addAction(newWindow);
		(void)windowGroup->addAction(newDoc);
		connect(newWindow, &QAction::triggered, this, &WorkspaceWindow::requestNewContentView);
		connect(newDoc, &QAction::triggered, this,
				[this]()
				{
					if (documentStack())
					{
						documentStack()->addNewDocument();
					}
				});

		auto* themeGroup = home->addGroup(QStringLiteral("Theme"));
		(void)themeGroup->addAction(light);
		(void)themeGroup->addAction(dark);
		connect(light, &QAction::triggered, this,
				[this]()
				{
					emit requestThemeScheme(mps::theme::Scheme::Light);
				});
		connect(dark, &QAction::triggered, this,
				[this]()
				{
					emit requestThemeScheme(mps::theme::Scheme::Dark);
				});

		auto* show = view->addGroup(QStringLiteral("Panes"));
		auto* toggleNav = makeAction(this, QStringLiteral("pane.nav"), QStringLiteral("Navigation"), QStyle::SP_DirIcon,
									 QStringLiteral("Toggle navigation pane."));
		auto* toggleUtil = makeAction(this, QStringLiteral("pane.util"), QStringLiteral("Utility"), QStyle::SP_FileDialogInfoView,
									  QStringLiteral("Toggle utility pane."));
		(void)show->addAction(toggleNav);
		(void)show->addAction(toggleUtil);
		connect(toggleNav, &QAction::triggered, this,
				[this]()
				{
					if (m_layout && m_layout->navigationPane())
					{
						m_layout->navigationPane()->setVisible(!m_layout->navigationPane()->isVisible());
					}
				});
		connect(toggleUtil, &QAction::triggered, this,
				[this]()
				{
					if (m_layout && m_layout->utilityPane())
					{
						m_layout->utilityPane()->setVisible(!m_layout->utilityPane()->isVisible());
					}
				});

		Q_UNUSED(bridge);
		qfluentribbon::ScreenTip::install(this);
	}
} // namespace volition
