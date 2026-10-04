#include "async_file_loader.hpp"

#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QMetaObject>
#include <QPointer>
#include <QThreadPool>

#include <utility>

namespace volition
{
	AsyncFileLoader::AsyncFileLoader(QObject* parent)
		: QObject(parent)
	{
	}

	void AsyncFileLoader::cancel()
	{
		++m_generation;
	}

	void AsyncFileLoader::deliver(int generation, const QString& path, const QByteArray& utf8Bytes, const QString& error)
	{
		if (generation != m_generation)
		{
			return;
		}
		emit finished(path, utf8Bytes, error);
	}

	void AsyncFileLoader::start(const QString& path)
	{
		const int generation = ++m_generation;
		const QFileInfo info(path);
		if (!info.exists() || !info.isFile())
		{
			deliver(generation, path, {}, tr("File not found"));
			return;
		}

		const auto readFile = [path]() -> std::pair<QByteArray, QString>
		{
			QFile file(path);
			if (!file.open(QIODevice::ReadOnly))
			{
				return {{}, file.errorString()};
			}
			return {file.readAll(), {}};
		};

		if (info.size() < kAsyncThresholdBytes)
		{
			const auto [bytes, error] = readFile();
			deliver(generation, path, bytes, error);
			return;
		}

		const QPointer<AsyncFileLoader> self(this);
		QThreadPool::globalInstance()->start(
			[self, path, generation, readFile]()
			{
				const auto [bytes, error] = readFile();
				if (!self)
				{
					return;
				}
				QMetaObject::invokeMethod(self.data(), "deliver", Qt::QueuedConnection, Q_ARG(int, generation),
										  Q_ARG(QString, path), Q_ARG(QByteArray, bytes), Q_ARG(QString, error));
			});
	}
} // namespace volition
