#ifndef __VOLITION_HOST_THEME_SERVICE_H__
#define __VOLITION_HOST_THEME_SERVICE_H__

#include "qtheme/engine.hpp"
#include "qtheme/types.hpp"
#include "theme_origin.hpp"
#include "theme_scheme.hpp"

#include <QObject>

#include <memory>

class QApplication;

namespace volition::host
{
	[[nodiscard]] inline qtheme::ColorScheme toColorScheme(mps::theme::Scheme scheme)
	{
		return scheme == mps::theme::Scheme::Dark ? qtheme::ColorScheme::Dark : qtheme::ColorScheme::Light;
	}

	[[nodiscard]] inline mps::theme::Scheme toThemeScheme(qtheme::ColorScheme scheme)
	{
		return scheme == qtheme::ColorScheme::Dark ? mps::theme::Scheme::Dark : mps::theme::Scheme::Light;
	}

	class ThemeService final : public QObject
	{
		Q_OBJECT
	public:
		explicit ThemeService(QObject* parent = nullptr);

		void start(QApplication* app);
		[[nodiscard]] qtheme::Engine* engine() const
		{
			return m_engine.get();
		}
		[[nodiscard]] mps::theme::Scheme scheme() const;
		void applyScheme(mps::theme::Scheme scheme);
		[[nodiscard]] mps::theme::Scheme loadPersistedOrDefault() const;
		void persist(mps::theme::Scheme scheme) const;

	private:
		std::unique_ptr<qtheme::Engine> m_engine;
	};
} // namespace volition::host

#endif // __VOLITION_HOST_THEME_SERVICE_H__
