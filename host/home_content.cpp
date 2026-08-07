#include "home_content.hpp"

#include "shell_app.hpp"
#include "shell_window.hpp"
#include "theme_origin.hpp"
#include "theme_scheme.hpp"

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

namespace volition::host
{
	HomeContent::HomeContent(mps::host::ShellApp* app, mps::host::ShellWindow* shell, QWidget* parent)
		: QWidget(parent)
	{
		auto* lay = new QVBoxLayout(this);
		auto* textBtn = new QPushButton(QStringLiteral("Create text"), this);
		auto* mdBtn = new QPushButton(QStringLiteral("Create markdown"), this);
		auto* pdfBtn = new QPushButton(QStringLiteral("Create pdf"), this);
		for (QPushButton* b : {textBtn, mdBtn, pdfBtn})
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
		lay->addWidget(textBtn, 0, Qt::AlignCenter);
		lay->addSpacing(8);
		lay->addWidget(mdBtn, 0, Qt::AlignCenter);
		lay->addSpacing(8);
		lay->addWidget(pdfBtn, 0, Qt::AlignCenter);
		lay->addSpacing(16);
		lay->addLayout(themeRow);
		lay->addStretch();

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
