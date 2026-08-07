#ifndef __VOLITION_CLIENT_RUN_HELPER_H__
#define __VOLITION_CLIENT_RUN_HELPER_H__

#include "client_app.hpp"
#include "document_stack.hpp"
#include "qfluentribbon/ribbon_tokens.hpp"
#include "qfluentribbon/theme_bridge.hpp"
#include "qtheme/api.hpp"
#include "qtheme/engine.hpp"
#include "qtheme/store.hpp"
#include "qtheme/types.hpp"
#include "workspace_content_view.hpp"

#include <QApplication>
#include <QColor>
#include <QCommandLineParser>

#include <memory>

namespace volition
{
	inline void syncRibbonTokensFromEngine(qtheme::Engine* engine, qfluentribbon::ThemeBridge* bridge)
	{
		if (!engine || !bridge)
		{
			return;
		}
		qfluentribbon::tokens::setDpiScale(qtheme::api::dpiScale());
		auto pick = [engine](const QString& role, const QColor& fallback) -> QColor
		{
			if (qtheme::ThemeStore* store = engine->store())
			{
				const qtheme::ColorValue cv = store->color(QStringLiteral("palette"), role, fallback);
				return cv.ok ? cv.value : fallback;
			}
			return fallback;
		};
		bridge->ensureRibbonTokens(pick(QStringLiteral("window"), QColor(QStringLiteral("#F3F3F3"))),
								   pick(QStringLiteral("surface"), QColor(QStringLiteral("#FFFFFF"))),
								   pick(QStringLiteral("stroke"), QColor(QStringLiteral("#D1D1D1"))),
								   pick(QStringLiteral("text"), QColor(QStringLiteral("#1A1A1A"))),
								   pick(QStringLiteral("accent"), QColor(QStringLiteral("#0078D4"))),
								   pick(QStringLiteral("text.tertiary"), QColor(QStringLiteral("#8D8D8D"))),
								   pick(QStringLiteral("accent.text"), QColor(Qt::white)));
	}

	/// Shared ClientApp bootstrap for thin-exe-loaded DLLs.
	inline int runClientPlugin(int argc, char** argv, const QString& appName, DocumentStack::DocumentFactory documentFactory)
	{
		QApplication app(argc, argv);
		QCommandLineParser parser;
		parser.addHelpOption();
		QCommandLineOption fromHost(QStringLiteral("from-host"));
		QCommandLineOption endpoint(QStringLiteral("endpoint"), QString(), QStringLiteral("name"));
		QCommandLineOption token(QStringLiteral("pipe-token"), QString(), QStringLiteral("token"));
		QCommandLineOption protocol(QStringLiteral("protocol"), QString(), QStringLiteral("n"), QStringLiteral("1"));
		QCommandLineOption noHeartbeat(QStringLiteral("no-heartbeat"), QStringLiteral("Disable Client Heartbeat"));
		parser.addOption(fromHost);
		parser.addOption(endpoint);
		parser.addOption(token);
		parser.addOption(protocol);
		parser.addOption(noHeartbeat);
		parser.process(app);

		if (!parser.isSet(fromHost) || !parser.isSet(endpoint))
		{
			qWarning("%s must be started by the Host (--from-host --endpoint=...)", qPrintable(appName));
			return 2;
		}

		qtheme::Engine engine;
		engine.apply(&app);
		qtheme::api::bind(&engine);
		qtheme::Engine::setDefault(&engine);
		qfluentribbon::ThemeBridge bridge;
		syncRibbonTokensFromEngine(&engine, &bridge);

		mps::client::ContentViewFactory factory = [&engine, &bridge, documentFactory](qint64 tabId, const QString& title)
		{
			return std::make_unique<WorkspaceContentView>(tabId, title, &engine, &bridge, documentFactory);
		};

		mps::client::ClientApp client(parser.value(endpoint), parser.value(token), std::move(factory), !parser.isSet(noHeartbeat));
		client.setAppName(appName);
		client.setRequestNewContentViewMethod(QStringLiteral("volition.request_new_window"));
		client.setAppearanceHandler(
			[&engine, &bridge](mps::theme::Scheme scheme)
			{
				const qtheme::ColorScheme cs =
					(scheme == mps::theme::Scheme::Dark) ? qtheme::ColorScheme::Dark : qtheme::ColorScheme::Light;
				(void)engine.setColorScheme(cs, /*force=*/true);
				syncRibbonTokensFromEngine(&engine, &bridge);
			});
		if (!client.connectToHost())
		{
			return 3;
		}
		return app.exec();
	}
} // namespace volition

#endif // __VOLITION_CLIENT_RUN_HELPER_H__
