#include "home_content.hpp"

#include "document_open_service.hpp"
#include "shell_app.hpp"
#include "shell_window.hpp"
#include "theme_origin.hpp"
#include "theme_scheme.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

namespace volition::host
{
	HomeContent::HomeContent(mps::host::ShellApp* app, mps::host::ShellWindow* shell, DocumentOpenService* openService, QWidget* parent)
		: QWidget(parent)
	{
		auto* lay = new QVBoxLayout(this);
		auto* openBtn = new QPushButton(QStringLiteral("Open file…"), this);
		auto* textBtn = new QPushButton(QStringLiteral("Create text"), this);
		auto* mdBtn = new QPushButton(QStringLiteral("Create markdown"), this);
		auto* pdfBtn = new QPushButton(QStringLiteral("Create pdf"), this);
		for (QPushButton* b : {openBtn, textBtn, mdBtn, pdfBtn})
		{
			b->setFixedSize(180, 40);
		}

		auto* lightBtn = new QPushButton(QStringLiteral("Light"), this);
		auto* darkBtn = new QPushButton(QStringLiteral("Dark"), this);
		lightBtn->setFixedSize(80, 32);
		darkBtn->setFixedSize(80, 32);
		auto* themeRow = new QHBoxLayout();
		themeRow->setSpacing(12);
		themeRow->addStretch();
		themeRow->addWidget(lightBtn);
		themeRow->addWidget(darkBtn);
		themeRow->addStretch();

		lay->addStretch();
		lay->addWidget(openBtn, 0, Qt::AlignCenter);
		lay->addSpacing(12);
		lay->addWidget(textBtn, 0, Qt::AlignCenter);
		lay->addSpacing(8);
		lay->addWidget(mdBtn, 0, Qt::AlignCenter);
		lay->addSpacing(8);
		lay->addWidget(pdfBtn, 0, Qt::AlignCenter);
		lay->addSpacing(16);
		lay->addLayout(themeRow);
		lay->addStretch();

		connect(openBtn, &QPushButton::clicked, this,
				[openService, shell]()
				{
					if (openService && shell)
					{
						openService->openWithDialog(shell);
					}
				});
		connect(textBtn, &QPushButton::clicked, this,
				[app, shell]()
				{
					if (app && shell)
					{
						app->createClientOn(shell, QStringLiteral("text"));
					}
				});
		connect(mdBtn, &QPushButton::clicked, this,
				[app, shell]()
				{
					if (app && shell)
					{
						app->createClientOn(shell, QStringLiteral("markdown"));
					}
				});
		connect(pdfBtn, &QPushButton::clicked, this,
				[app, shell]()
				{
					if (app && shell)
					{
						app->createClientOn(shell, QStringLiteral("pdf"));
					}
				});
		connect(lightBtn, &QPushButton::clicked, this,
				[app]()
				{
					if (app)
					{
						app->setScheme(mps::theme::Scheme::Light, mps::host::ThemeOrigin::HostUi);
					}
				});
		connect(darkBtn, &QPushButton::clicked, this,
				[app]()
				{
					if (app)
					{
						app->setScheme(mps::theme::Scheme::Dark, mps::host::ThemeOrigin::HostUi);
					}
				});
	}
} // namespace volition::host
