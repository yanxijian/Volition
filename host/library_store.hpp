#ifndef __VOLITION_HOST_LIBRARY_STORE_H__
#define __VOLITION_HOST_LIBRARY_STORE_H__

#include <QObject>
#include <QString>
#include <QStringList>

namespace volition::host
{
	/// Persisted recent opens + favorites for Host Home (QSettings).
	class LibraryStore final : public QObject
	{
		Q_OBJECT
	public:
		static constexpr int kMaxRecent = 50;

		explicit LibraryStore(QObject* parent = nullptr);

		[[nodiscard]] QStringList recentPaths() const
		{
			return m_recent;
		}
		[[nodiscard]] QStringList favoritePaths() const
		{
			return m_favorites;
		}

		void recordOpened(const QString& path);
		void addFavorite(const QString& path);
		void removeFavorite(const QString& path);
		[[nodiscard]] bool isFavorite(const QString& path) const;
		void clearRecent();
		void clearFavorites();

	signals:
		void changed();

	private:
		void load();
		void save() const;
		[[nodiscard]] static QString normalizePath(const QString& path);

		QStringList m_recent;
		QStringList m_favorites;
	};
} // namespace volition::host

#endif // __VOLITION_HOST_LIBRARY_STORE_H__
