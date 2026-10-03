#include "home_content.hpp"

#include "client_kind.hpp"
#include "document_open_service.hpp"
#include "language_service.hpp"
#include "library_store.hpp"
#include "shell_app.hpp"
#include "shell_window.hpp"
#include "qtheme/api.hpp"
#include "theme_origin.hpp"
#include "theme_scheme.hpp"

#include <QApplication>
#include <QAbstractItemView>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QStandardPaths>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <functional>

namespace volition::host
{
	namespace
	{
		constexpr int kPathRole = Qt::UserRole;
		constexpr int kLeftNavWidth = 220;

		[[nodiscard]] QColor mixRgb(const QColor& a, const QColor& b, int aParts, int bParts)
		{
			const int t = qMax(1, aParts + bParts);
			return QColor((a.red() * aParts + b.red() * bParts) / t, (a.green() * aParts + b.green() * bParts) / t,
						  (a.blue() * aParts + b.blue() * bParts) / t);
		}

		void configureNavButton(QPushButton* btn, bool checkable)
		{
			btn->setCheckable(checkable);
			btn->setFlat(true);
			btn->setFocusPolicy(Qt::NoFocus);
			btn->setAutoDefault(false);
			btn->setDefault(false);
			btn->setCursor(Qt::PointingHandCursor);
		}

		void applyNavChrome(QPushButton* openBtn, QPushButton* recentBtn, QPushButton* favoritesBtn, QLabel* dirsLabel, QLabel* listHint,
							QTreeWidget* dirTree)
		{
			const QPalette pal = QApplication::palette();
			const QColor window = pal.color(QPalette::Window);
			const QColor windowText = pal.color(QPalette::WindowText);

			const QColor idleFg = qtheme::api::color(QStringLiteral("palette"), QStringLiteral("windowText"), windowText);
			const QColor hoverBg = qtheme::api::color(QStringLiteral("button"), QStringLiteral("bg.hover"), mixRgb(window, idleFg, 7, 1));
			const QColor selectedBg =
				qtheme::api::color(QStringLiteral("button"), QStringLiteral("bg.checked"), mixRgb(window, idleFg, 5, 2));
			const QColor selectedFg = qtheme::api::color(QStringLiteral("button"), QStringLiteral("fg"), idleFg);
			const QColor muted = qtheme::api::color(QStringLiteral("palette"), QStringLiteral("text.tertiary"), mixRgb(idleFg, window, 3, 2));
			const QColor treeHover = qtheme::api::color(QStringLiteral("view"), QStringLiteral("bg.hover"), hoverBg);
			const QColor treeSelected = qtheme::api::color(QStringLiteral("view"), QStringLiteral("bg.selected"), selectedBg);
			const QColor treeSelectedFg = qtheme::api::color(QStringLiteral("view"), QStringLiteral("fg.selected"), selectedFg);

			const QString navQss = QStringLiteral("QPushButton {"
												  " text-align: left; padding: 8px 12px; border: none; outline: none;"
												  " border-radius: 4px; background: transparent; color: %1;"
												  "}"
												  "QPushButton:hover { background: %2; color: %1; border: none; outline: none; }"
												  "QPushButton:checked { background: %3; color: %4; border: none; outline: none; font-weight: 600; }"
												  "QPushButton:pressed { background: %3; color: %4; border: none; outline: none; }")
									   .arg(idleFg.name(QColor::HexRgb), hoverBg.name(QColor::HexRgb), selectedBg.name(QColor::HexRgb),
											selectedFg.name(QColor::HexRgb));

			for (QPushButton* btn : {openBtn, recentBtn, favoritesBtn})
			{
				if (btn)
				{
					btn->setStyleSheet(navQss);
				}
			}

			if (dirsLabel)
			{
				dirsLabel->setStyleSheet(QStringLiteral("color: %1; padding: 8px 12px 2px 12px;").arg(muted.name(QColor::HexRgb)));
			}
			if (listHint)
			{
				listHint->setStyleSheet(QStringLiteral("color: %1;").arg(muted.name(QColor::HexRgb)));
			}
			if (dirTree)
			{
				dirTree->setStyleSheet(QStringLiteral("QTreeWidget { background: transparent; border: none; color: %1; }"
													  "QTreeWidget::item { padding: 4px 6px; color: %1; }"
													  "QTreeWidget::item:hover { background: %2; color: %1; }"
													  "QTreeWidget::item:selected { background: %3; color: %4; }")
										   .arg(idleFg.name(QColor::HexRgb), treeHover.name(QColor::HexRgb),
												treeSelected.name(QColor::HexRgb), treeSelectedFg.name(QColor::HexRgb)));
			}
		}

		[[nodiscard]] bool isSupportedDocument(const QString& path)
		{
			return !clientKindForPath(path).isEmpty();
		}
	} // namespace

	HomeContent::HomeContent(mps::host::ShellApp* app, mps::host::ShellWindow* shell, DocumentOpenService* openService,
							 LibraryStore* library, LanguageService* language, QWidget* parent)
		: QWidget(parent)
		, m_app(app)
		, m_shell(shell)
		, m_openService(openService)
		, m_library(library)
		, m_language(language)
	{
		buildUi();
		selectNav(PaneMode::Recent);
		if (m_library)
		{
			connect(m_library, &LibraryStore::changed, this, &HomeContent::refreshList);
		}
		if (m_language)
		{
			connect(m_language, &LanguageService::languageChanged, this,
					[this](const QString&)
					{
						retranslateUi();
					});
		}
	}

	void HomeContent::changeEvent(QEvent* event)
	{
		QWidget::changeEvent(event);
		if (event && (event->type() == QEvent::LanguageChange || event->type() == QEvent::PaletteChange))
		{
			if (event->type() == QEvent::LanguageChange)
			{
				retranslateUi();
			}
			applyNavChrome(m_openBtn, m_recentBtn, m_favoritesBtn, m_dirsLabel, m_listHint, m_dirTree);
		}
	}

	QString HomeContent::kindLabelForPath(const QString& path) const
	{
		const QString kind = clientKindForPath(path);
		if (kind.isEmpty())
		{
			const QFileInfo fi(path);
			return fi.isDir() ? tr("Folder") : fi.suffix().toUpper();
		}
		return tabKindLabel(kind);
	}

	void HomeContent::syncThemeLabel()
	{
		if (!m_app || !m_themeBtn)
		{
			return;
		}
		const bool dark = m_app->scheme() == mps::theme::Scheme::Dark;
		m_themeBtn->setText(dark ? tr("Light") : tr("Dark"));
	}

	void HomeContent::buildUi()
	{
		auto* root = new QVBoxLayout(this);
		root->setContentsMargins(16, 12, 16, 12);
		root->setSpacing(12);

		auto* top = new QHBoxLayout();
		top->setSpacing(10);

		m_brand = new QLabel(QStringLiteral("Volition"), this);
		QFont brandFont = m_brand->font();
		brandFont.setPointSizeF(brandFont.pointSizeF() + 4);
		brandFont.setBold(true);
		m_brand->setFont(brandFont);

		m_search = new QLineEdit(this);
		m_search->setClearButtonEnabled(true);
		m_search->setMinimumWidth(280);

		m_newBtn = new QPushButton(this);
		auto* newMenu = new QMenu(m_newBtn);
		newMenu->addAction(QString(), this,
						   [this]()
						   {
							   if (m_app && m_shell)
							   {
								   m_app->createClientOn(m_shell, QStringLiteral("text"));
							   }
						   });
		newMenu->addAction(QString(), this,
						   [this]()
						   {
							   if (m_app && m_shell)
							   {
								   m_app->createClientOn(m_shell, QStringLiteral("markdown"));
							   }
						   });
		newMenu->addAction(QString(), this,
						   [this]()
						   {
							   if (m_app && m_shell)
							   {
								   m_app->createClientOn(m_shell, QStringLiteral("pdf"));
							   }
						   });
		m_newBtn->setMenu(newMenu);

		m_themeBtn = new QPushButton(this);
		m_settingsBtn = new QPushButton(this);

		if (m_app)
		{
			connect(m_app, &mps::host::ShellApp::schemeChanged, this,
					[this](mps::theme::Scheme, mps::host::ThemeOrigin)
					{
						syncThemeLabel();
						applyNavChrome(m_openBtn, m_recentBtn, m_favoritesBtn, m_dirsLabel, m_listHint, m_dirTree);
					});
		}
		connect(m_themeBtn, &QPushButton::clicked, this,
				[this]()
				{
					if (!m_app)
					{
						return;
					}
					const auto next = m_app->scheme() == mps::theme::Scheme::Dark ? mps::theme::Scheme::Light : mps::theme::Scheme::Dark;
					m_app->setScheme(next, mps::host::ThemeOrigin::HostUi);
					syncThemeLabel();
				});
		connect(m_settingsBtn, &QPushButton::clicked, this, &HomeContent::showSettingsDialog);
		connect(m_search, &QLineEdit::textChanged, this, &HomeContent::refreshList);

		top->addWidget(m_brand);
		top->addSpacing(16);
		top->addWidget(m_search, 1);
		top->addWidget(m_themeBtn);
		top->addWidget(m_settingsBtn);
		top->addWidget(m_newBtn);

		auto* split = new QSplitter(Qt::Horizontal, this);
		split->setChildrenCollapsible(false);

		auto* left = new QWidget(split);
		auto* leftLay = new QVBoxLayout(left);
		leftLay->setContentsMargins(0, 0, 8, 0);
		leftLay->setSpacing(4);

		m_openBtn = new QPushButton(left);
		configureNavButton(m_openBtn, false);
		connect(m_openBtn, &QPushButton::clicked, this,
				[this]()
				{
					if (m_openService && m_shell)
					{
						m_openService->openWithDialog(m_shell);
					}
				});

		m_recentBtn = new QPushButton(left);
		m_favoritesBtn = new QPushButton(left);
		configureNavButton(m_recentBtn, true);
		configureNavButton(m_favoritesBtn, true);
		connect(m_recentBtn, &QPushButton::clicked, this,
				[this]()
				{
					selectNav(PaneMode::Recent);
				});
		connect(m_favoritesBtn, &QPushButton::clicked, this,
				[this]()
				{
					selectNav(PaneMode::Favorites);
				});

		m_dirsLabel = new QLabel(left);

		m_dirTree = new QTreeWidget(left);
		m_dirTree->setHeaderHidden(true);
		m_dirTree->setRootIsDecorated(true);
		m_dirTree->setIndentation(14);
		m_dirTree->setAnimated(true);
		m_dirTree->setFocusPolicy(Qt::StrongFocus);
		connect(m_dirTree, &QTreeWidget::itemClicked, this,
				[this](QTreeWidgetItem* item, int)
				{
					if (!item)
					{
						return;
					}
					const QString path = item->data(0, kPathRole).toString();
					if (path.isEmpty())
					{
						return;
					}
					m_directoryPath = path;
					selectNav(PaneMode::Directory);
				});

		leftLay->addWidget(m_openBtn);
		leftLay->addWidget(m_recentBtn);
		leftLay->addWidget(m_favoritesBtn);
		leftLay->addWidget(m_dirsLabel);
		leftLay->addWidget(m_dirTree, 1);
		left->setMinimumWidth(kLeftNavWidth);
		left->setMaximumWidth(320);

		auto* right = new QWidget(split);
		auto* rightLay = new QVBoxLayout(right);
		rightLay->setContentsMargins(8, 0, 0, 0);
		rightLay->setSpacing(6);

		m_listHint = new QLabel(right);

		m_fileList = new QListWidget(right);
		m_fileList->setAlternatingRowColors(true);
		m_fileList->setContextMenuPolicy(Qt::CustomContextMenu);
		m_fileList->setSelectionMode(QAbstractItemView::SingleSelection);
		connect(m_fileList, &QListWidget::itemActivated, this,
				[this](QListWidgetItem* item)
				{
					if (item)
					{
						openSelectedPath(item->data(kPathRole).toString());
					}
				});
		connect(m_fileList, &QListWidget::customContextMenuRequested, this, &HomeContent::showListContextMenu);

		rightLay->addWidget(m_listHint);
		rightLay->addWidget(m_fileList, 1);

		split->addWidget(left);
		split->addWidget(right);
		split->setStretchFactor(0, 0);
		split->setStretchFactor(1, 1);
		split->setSizes({kLeftNavWidth, 640});

		root->addLayout(top);
		root->addWidget(split, 1);

		applyNavChrome(m_openBtn, m_recentBtn, m_favoritesBtn, m_dirsLabel, m_listHint, m_dirTree);
		retranslateUi();
	}

	void HomeContent::rebuildCommonFolders()
	{
		if (!m_dirTree)
		{
			return;
		}
		const QString selectedPath = m_directoryPath;
		m_dirTree->clear();

		const auto addRootDir = [this](const QString& title, QStandardPaths::StandardLocation loc)
		{
			const QString path = QStandardPaths::writableLocation(loc);
			if (path.isEmpty() || !QDir(path).exists())
			{
				return;
			}
			auto* item = new QTreeWidgetItem(m_dirTree, QStringList{title});
			item->setData(0, kPathRole, path);
			item->setToolTip(0, path);
			const QFileInfoList subs = QDir(path).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
			for (const QFileInfo& fi : subs)
			{
				auto* child = new QTreeWidgetItem(item, QStringList{fi.fileName()});
				child->setData(0, kPathRole, fi.absoluteFilePath());
				child->setToolTip(0, fi.absoluteFilePath());
			}
		};
		addRootDir(tr("Documents"), QStandardPaths::DocumentsLocation);
		addRootDir(tr("Desktop"), QStandardPaths::DesktopLocation);
		addRootDir(tr("Downloads"), QStandardPaths::DownloadLocation);

		if (!selectedPath.isEmpty())
		{
			std::function<QTreeWidgetItem*(QTreeWidgetItem*)> findPath;
			findPath = [&](QTreeWidgetItem* parent) -> QTreeWidgetItem*
			{
				const int n = parent ? parent->childCount() : m_dirTree->topLevelItemCount();
				for (int i = 0; i < n; ++i)
				{
					QTreeWidgetItem* item = parent ? parent->child(i) : m_dirTree->topLevelItem(i);
					if (item->data(0, kPathRole).toString() == selectedPath)
					{
						return item;
					}
					if (QTreeWidgetItem* hit = findPath(item))
					{
						return hit;
					}
				}
				return nullptr;
			};
			if (QTreeWidgetItem* hit = findPath(nullptr))
			{
				m_dirTree->setCurrentItem(hit);
			}
		}
	}

	void HomeContent::retranslateUi()
	{
		if (m_search)
		{
			m_search->setPlaceholderText(tr("Search recent or favorites…"));
		}
		if (m_newBtn)
		{
			m_newBtn->setText(tr("New"));
			if (QMenu* menu = m_newBtn->menu(); menu && menu->actions().size() >= 3)
			{
				menu->actions().at(0)->setText(tr("Text Document"));
				menu->actions().at(1)->setText(tr("Markdown"));
				menu->actions().at(2)->setText(tr("PDF"));
			}
		}
		if (m_settingsBtn)
		{
			m_settingsBtn->setText(tr("Settings"));
		}
		if (m_openBtn)
		{
			m_openBtn->setText(tr("Open"));
		}
		if (m_recentBtn)
		{
			m_recentBtn->setText(tr("Recent"));
		}
		if (m_favoritesBtn)
		{
			m_favoritesBtn->setText(tr("Favorites"));
		}
		if (m_dirsLabel)
		{
			m_dirsLabel->setText(tr("Common folders"));
		}
		if (m_listHint)
		{
			m_listHint->setText(tr("Files"));
		}
		syncThemeLabel();
		rebuildCommonFolders();
		refreshList();
	}

	void HomeContent::selectNav(PaneMode mode)
	{
		m_mode = mode;
		if (m_recentBtn)
		{
			m_recentBtn->setChecked(mode == PaneMode::Recent);
		}
		if (m_favoritesBtn)
		{
			m_favoritesBtn->setChecked(mode == PaneMode::Favorites);
		}
		if (mode != PaneMode::Directory && m_dirTree)
		{
			m_dirTree->clearSelection();
		}
		refreshList();
	}

	QStringList HomeContent::searchHits() const
	{
		if (!m_library || !m_search)
		{
			return {};
		}
		const QString q = m_search->text().trimmed();
		if (q.isEmpty())
		{
			return {};
		}
		QStringList out;
		const auto consider = [&](const QStringList& paths)
		{
			for (const QString& p : paths)
			{
				if (p.contains(q, Qt::CaseInsensitive) || QFileInfo(p).fileName().contains(q, Qt::CaseInsensitive))
				{
					if (!out.contains(p))
					{
						out.append(p);
					}
				}
			}
		};
		consider(m_library->recentPaths());
		consider(m_library->favoritePaths());
		return out;
	}

	void HomeContent::refreshList()
	{
		if (!m_fileList)
		{
			return;
		}
		m_fileList->clear();

		if (m_search && !m_search->text().trimmed().isEmpty())
		{
			const QStringList hits = searchHits();
			if (hits.isEmpty())
			{
				auto* empty = new QListWidgetItem(tr("No matching recent or favorite items"));
				empty->setFlags(empty->flags() & ~Qt::ItemIsEnabled);
				m_fileList->addItem(empty);
				return;
			}
			for (const QString& path : hits)
			{
				const QFileInfo fi(path);
				auto* item = new QListWidgetItem(QStringLiteral("%1    %2    %3").arg(fi.fileName(), kindLabelForPath(path), path));
				item->setData(kPathRole, path);
				item->setToolTip(path);
				m_fileList->addItem(item);
			}
			return;
		}

		if (m_mode == PaneMode::Directory)
		{
			populateDirectoryList(m_directoryPath);
			return;
		}

		if (!m_library)
		{
			return;
		}

		const QStringList paths = m_mode == PaneMode::Favorites ? m_library->favoritePaths() : m_library->recentPaths();
		if (paths.isEmpty())
		{
			auto* empty =
				new QListWidgetItem(m_mode == PaneMode::Favorites ? tr("No favorites yet. Right-click a file in the list to favorite it.")
																  : tr("No recent files. Use Open on the left to get started."));
			empty->setFlags(empty->flags() & ~Qt::ItemIsEnabled);
			m_fileList->addItem(empty);
			return;
		}

		for (const QString& path : paths)
		{
			const QFileInfo fi(path);
			const QString missing = fi.exists() ? QString() : tr(" (missing)");
			auto* item = new QListWidgetItem(QStringLiteral("%1%2    %3    %4").arg(fi.fileName(), missing, kindLabelForPath(path), path));
			item->setData(kPathRole, path);
			item->setToolTip(path);
			m_fileList->addItem(item);
		}
	}

	void HomeContent::populateDirectoryList(const QString& dirPath)
	{
		if (dirPath.isEmpty())
		{
			return;
		}
		QDir dir(dirPath);
		if (!dir.exists())
		{
			auto* empty = new QListWidgetItem(tr("Folder not found"));
			empty->setFlags(empty->flags() & ~Qt::ItemIsEnabled);
			m_fileList->addItem(empty);
			return;
		}

		const QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);
		int shown = 0;
		for (const QFileInfo& fi : entries)
		{
			if (!isSupportedDocument(fi.absoluteFilePath()))
			{
				continue;
			}
			const QString path = fi.absoluteFilePath();
			auto* item = new QListWidgetItem(QStringLiteral("%1    %2").arg(fi.fileName(), kindLabelForPath(path)));
			item->setData(kPathRole, path);
			item->setToolTip(path);
			m_fileList->addItem(item);
			++shown;
		}
		if (shown == 0)
		{
			auto* empty = new QListWidgetItem(tr("No openable documents in this folder"));
			empty->setFlags(empty->flags() & ~Qt::ItemIsEnabled);
			m_fileList->addItem(empty);
		}
	}

	void HomeContent::openSelectedPath(const QString& path)
	{
		if (path.isEmpty() || !m_openService || !m_shell)
		{
			return;
		}
		const QFileInfo fi(path);
		if (fi.isDir())
		{
			m_directoryPath = path;
			selectNav(PaneMode::Directory);
			return;
		}
		m_openService->openPath(m_shell, path);
	}

	void HomeContent::showSettingsDialog()
	{
		QDialog dlg(this);
		dlg.setWindowTitle(tr("Settings"));
		dlg.setModal(true);
		auto* lay = new QVBoxLayout(&dlg);

		auto* form = new QFormLayout();
		auto* languageCombo = new QComboBox(&dlg);
		if (m_language)
		{
			const QString current = m_language->currentLanguage();
			for (const QString& code : m_language->availableLanguages())
			{
				languageCombo->addItem(m_language->displayName(code), code);
				if (code == current)
				{
					languageCombo->setCurrentIndex(languageCombo->count() - 1);
				}
			}
		}
		form->addRow(tr("Language"), languageCombo);
		lay->addLayout(form);

		lay->addWidget(new QLabel(tr("Use Light/Dark in the top bar to change appearance. Your choice is remembered."), &dlg));
		auto* clearRecent = new QPushButton(tr("Clear recent files"), &dlg);
		auto* clearFav = new QPushButton(tr("Clear favorites"), &dlg);
		lay->addWidget(clearRecent);
		lay->addWidget(clearFav);
		auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dlg);
		lay->addWidget(buttons);
		connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

		connect(languageCombo, &QComboBox::currentIndexChanged, &dlg,
				[this, languageCombo](int index)
				{
					if (!m_language || index < 0)
					{
						return;
					}
					const QString code = languageCombo->itemData(index).toString();
					m_language->setLanguage(code);
				});
		connect(clearRecent, &QPushButton::clicked, this,
				[this]()
				{
					if (m_library)
					{
						m_library->clearRecent();
					}
				});
		connect(clearFav, &QPushButton::clicked, this,
				[this]()
				{
					if (m_library)
					{
						m_library->clearFavorites();
					}
				});
		dlg.exec();
	}

	void HomeContent::showListContextMenu(const QPoint& pos)
	{
		if (!m_fileList || !m_library)
		{
			return;
		}
		QListWidgetItem* item = m_fileList->itemAt(pos);
		if (!item || !(item->flags() & Qt::ItemIsEnabled))
		{
			return;
		}
		const QString path = item->data(kPathRole).toString();
		if (path.isEmpty())
		{
			return;
		}

		QMenu menu(this);
		menu.addAction(tr("Open"), this,
					   [this, path]()
					   {
						   openSelectedPath(path);
					   });
		if (m_library->isFavorite(path))
		{
			menu.addAction(tr("Remove from favorites"), this,
						   [this, path]()
						   {
							   m_library->removeFavorite(path);
						   });
		}
		else
		{
			menu.addAction(tr("Add to favorites"), this,
						   [this, path]()
						   {
							   m_library->addFavorite(path);
						   });
		}
		menu.exec(m_fileList->mapToGlobal(pos));
	}
} // namespace volition::host
