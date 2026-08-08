#ifndef __VOLITION_HOST_HOME_CONTENT_H__
#define __VOLITION_HOST_HOME_CONTENT_H__

#include <QWidget>

class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QTreeWidget;

namespace mps::host
{
	class ShellApp;
	class ShellWindow;
} // namespace mps::host

namespace volition::host
{
	class DocumentOpenService;
	class LanguageService;
	class LibraryStore;

	/// Host Home: Open / Recent / Favorites / common dirs + search / theme / settings.
	class HomeContent final : public QWidget
	{
		Q_OBJECT
	public:
		HomeContent(mps::host::ShellApp* app, mps::host::ShellWindow* shell, DocumentOpenService* openService, LibraryStore* library,
					LanguageService* language, QWidget* parent = nullptr);

	protected:
		void changeEvent(QEvent* event) override;

	private:
		enum class PaneMode
		{
			Recent,
			Favorites,
			Directory,
		};

		void buildUi();
		void retranslateUi();
		void rebuildCommonFolders();
		void selectNav(PaneMode mode);
		void refreshList();
		void populateDirectoryList(const QString& dirPath);
		void openSelectedPath(const QString& path);
		void showSettingsDialog();
		void showListContextMenu(const QPoint& pos);
		void syncThemeLabel();
		[[nodiscard]] QStringList searchHits() const;
		[[nodiscard]] QString kindLabelForPath(const QString& path) const;

		mps::host::ShellApp* m_app = nullptr;
		mps::host::ShellWindow* m_shell = nullptr;
		DocumentOpenService* m_openService = nullptr;
		LibraryStore* m_library = nullptr;
		LanguageService* m_language = nullptr;

		QLabel* m_brand = nullptr;
		QLineEdit* m_search = nullptr;
		QPushButton* m_newBtn = nullptr;
		QPushButton* m_themeBtn = nullptr;
		QPushButton* m_settingsBtn = nullptr;
		QPushButton* m_openBtn = nullptr;
		QPushButton* m_recentBtn = nullptr;
		QPushButton* m_favoritesBtn = nullptr;
		QLabel* m_dirsLabel = nullptr;
		QLabel* m_listHint = nullptr;
		QTreeWidget* m_dirTree = nullptr;
		QListWidget* m_fileList = nullptr;

		PaneMode m_mode = PaneMode::Recent;
		QString m_directoryPath;
	};
} // namespace volition::host

#endif // __VOLITION_HOST_HOME_CONTENT_H__
