#ifndef __VOLITION_HOST_APP_SETTINGS_H__
#define __VOLITION_HOST_APP_SETTINGS_H__

#include <QCoreApplication>
#include <QDir>
#include <QSettings>
#include <QString>

namespace volition::host
{
	/// Host prefs as INI under `<appDir>/config/volition.ini` (not the registry).
	[[nodiscard]] inline QString appConfigDir()
	{
		return QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("config"));
	}

	[[nodiscard]] inline QString appSettingsFilePath()
	{
		return QDir(appConfigDir()).filePath(QStringLiteral("volition.ini"));
	}

	[[nodiscard]] inline QSettings makeAppSettings()
	{
		QDir().mkpath(appConfigDir());
		return QSettings(appSettingsFilePath(), QSettings::IniFormat);
	}
} // namespace volition::host

#endif // __VOLITION_HOST_APP_SETTINGS_H__
