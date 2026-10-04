#ifndef __VOLITION_CLIENT_ASYNC_FILE_LOADER_H__
#define __VOLITION_CLIENT_ASYNC_FILE_LOADER_H__

#include <QByteArray>
#include <QObject>
#include <QString>

namespace volition
{
	/// Load a UTF-8 text file off the UI thread when it exceeds kAsyncThresholdBytes.
	/// Smaller files finish synchronously (still via finished()).
	class AsyncFileLoader final : public QObject
	{
		Q_OBJECT
	public:
		static constexpr qint64 kAsyncThresholdBytes = 512 * 1024;

		explicit AsyncFileLoader(QObject* parent = nullptr);

		void start(const QString& path);
		void cancel();

	signals:
		void finished(const QString& path, const QByteArray& utf8Bytes, const QString& error);

	private slots:
		void deliver(int generation, const QString& path, const QByteArray& utf8Bytes, const QString& error);

	private:
		int m_generation = 0;
	};
} // namespace volition

#endif // __VOLITION_CLIENT_ASYNC_FILE_LOADER_H__
