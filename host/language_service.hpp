#ifndef __VOLITION_HOST_LANGUAGE_SERVICE_H__
#define __VOLITION_HOST_LANGUAGE_SERVICE_H__

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTranslator>

class QCoreApplication;

namespace volition::host
{
	/// Loads `volition_<locale>.qm` from `<appDir>/langs/` and persists the choice.
	class LanguageService final : public QObject
	{
		Q_OBJECT
	public:
		explicit LanguageService(QObject* parent = nullptr);

		void start(QCoreApplication* app);

		[[nodiscard]] static QString langsDir();
		[[nodiscard]] QString currentLanguage() const
		{
			return m_language;
		}
		/// Built-in `en` plus locales that have a `.qm` under langs/.
		[[nodiscard]] QStringList availableLanguages() const;
		[[nodiscard]] QString displayName(const QString& language) const;

		/// Empty / `en` = source English (no translator). Other codes load matching qm.
		bool setLanguage(const QString& language);

	signals:
		void languageChanged(const QString& language);

	private:
		[[nodiscard]] QString loadPersistedOrDefault() const;
		void persist(const QString& language) const;
		[[nodiscard]] bool applyLanguage(const QString& language);

		QCoreApplication* m_app = nullptr;
		QTranslator m_translator;
		QString m_language;
	};
} // namespace volition::host

#endif // __VOLITION_HOST_LANGUAGE_SERVICE_H__
