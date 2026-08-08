#include "language_service.hpp"

#include "app_settings.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLocale>

namespace volition::host
{
	namespace
	{
		constexpr auto kLanguageKey = "ui/language";
		constexpr auto kQmPrefix = "volition_";
		constexpr auto kQmSuffix = ".qm";
	} // namespace

	LanguageService::LanguageService(QObject* parent)
		: QObject(parent)
	{
	}

	QString LanguageService::langsDir()
	{
		return QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("langs"));
	}

	void LanguageService::start(QCoreApplication* app)
	{
		m_app = app;
		(void)applyLanguage(loadPersistedOrDefault());
	}

	QString LanguageService::loadPersistedOrDefault() const
	{
		QSettings settings = makeAppSettings();
		const QString saved = settings.value(QString::fromUtf8(kLanguageKey)).toString().trimmed();
		if (!saved.isEmpty())
		{
			return saved;
		}

		const QStringList uiLangs = QLocale::system().uiLanguages();
		for (QString lang : uiLangs)
		{
			lang.replace(QLatin1Char('-'), QLatin1Char('_'));
			if (lang.startsWith(QLatin1String("zh"), Qt::CaseInsensitive))
			{
				const QString qm = QDir(langsDir()).filePath(QStringLiteral("volition_zh_CN.qm"));
				if (QFileInfo::exists(qm))
				{
					return QStringLiteral("zh_CN");
				}
			}
		}
		return QStringLiteral("en");
	}

	void LanguageService::persist(const QString& language) const
	{
		QSettings settings = makeAppSettings();
		settings.setValue(QString::fromUtf8(kLanguageKey), language);
	}

	QStringList LanguageService::availableLanguages() const
	{
		QStringList out;
		out.append(QStringLiteral("en"));
		const QDir dir(langsDir());
		const QFileInfoList files = dir.entryInfoList(QStringList{QStringLiteral("volition_*.qm")}, QDir::Files, QDir::Name);
		for (const QFileInfo& fi : files)
		{
			QString name = fi.completeBaseName(); // volition_zh_CN
			if (!name.startsWith(QString::fromUtf8(kQmPrefix)))
			{
				continue;
			}
			name.remove(0, QStringLiteral("volition_").size());
			if (name.isEmpty() || out.contains(name))
			{
				continue;
			}
			out.append(name);
		}
		return out;
	}

	QString LanguageService::displayName(const QString& language) const
	{
		if (language == QLatin1String("en") || language.isEmpty())
		{
			return QStringLiteral("English");
		}
		if (language == QLatin1String("zh_CN"))
		{
			return QStringLiteral("简体中文");
		}
		const QLocale locale(language);
		const QString native = locale.nativeLanguageName();
		return native.isEmpty() ? language : native;
	}

	bool LanguageService::applyLanguage(const QString& language)
	{
		if (!m_app)
		{
			return false;
		}

		m_app->removeTranslator(&m_translator);

		QString lang = language.trimmed();
		if (lang.isEmpty())
		{
			lang = QStringLiteral("en");
		}

		bool ok = true;
		if (lang != QLatin1String("en"))
		{
			const QString qmPath = QDir(langsDir()).filePath(QString::fromUtf8(kQmPrefix) + lang + QString::fromUtf8(kQmSuffix));
			ok = m_translator.load(qmPath);
			if (ok)
			{
				m_app->installTranslator(&m_translator);
			}
			else
			{
				lang = QStringLiteral("en");
			}
		}

		const bool changed = m_language != lang;
		m_language = lang;
		if (changed)
		{
			emit languageChanged(m_language);
		}
		return ok;
	}

	bool LanguageService::setLanguage(const QString& language)
	{
		const bool ok = applyLanguage(language);
		persist(m_language);
		return ok;
	}
} // namespace volition::host
