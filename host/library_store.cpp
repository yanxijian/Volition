#include "library_store.hpp"

#include "app_settings.hpp"

#include <QFileInfo>

namespace volition::host
{
	namespace
	{
		constexpr auto kRecentKey = "library/recent";
		constexpr auto kFavoritesKey = "library/favorites";
	} // namespace

	LibraryStore::LibraryStore(QObject* parent)
		: QObject(parent)
	{
		load();
	}

	QString LibraryStore::normalizePath(const QString& path)
	{
		return QFileInfo(path).absoluteFilePath();
	}

	void LibraryStore::load()
	{
		QSettings settings = makeAppSettings();
		m_recent = settings.value(QString::fromUtf8(kRecentKey)).toStringList();
		m_favorites = settings.value(QString::fromUtf8(kFavoritesKey)).toStringList();
	}

	void LibraryStore::save() const
	{
		QSettings settings = makeAppSettings();
		settings.setValue(QString::fromUtf8(kRecentKey), m_recent);
		settings.setValue(QString::fromUtf8(kFavoritesKey), m_favorites);
	}

	void LibraryStore::recordOpened(const QString& path)
	{
		const QString normalizedPath = normalizePath(path);
		if (normalizedPath.isEmpty() || !QFileInfo::exists(normalizedPath))
		{
			return;
		}
		m_recent.removeAll(normalizedPath);
		m_recent.prepend(normalizedPath);
		while (m_recent.size() > kMaxRecent)
		{
			m_recent.removeLast();
		}
		save();
		emit changed();
	}

	void LibraryStore::addFavorite(const QString& path)
	{
		const QString normalizedPath = normalizePath(path);
		if (normalizedPath.isEmpty() || !QFileInfo::exists(normalizedPath))
		{
			return;
		}
		if (m_favorites.contains(normalizedPath))
		{
			return;
		}
		m_favorites.append(normalizedPath);
		save();
		emit changed();
	}

	void LibraryStore::removeFavorite(const QString& path)
	{
		const QString normalizedPath = normalizePath(path);
		if (!m_favorites.removeAll(normalizedPath))
		{
			return;
		}
		save();
		emit changed();
	}

	bool LibraryStore::isFavorite(const QString& path) const
	{
		return m_favorites.contains(normalizePath(path));
	}

	void LibraryStore::clearRecent()
	{
		if (m_recent.isEmpty())
		{
			return;
		}
		m_recent.clear();
		save();
		emit changed();
	}

	void LibraryStore::clearFavorites()
	{
		if (m_favorites.isEmpty())
		{
			return;
		}
		m_favorites.clear();
		save();
		emit changed();
	}
} // namespace volition::host
