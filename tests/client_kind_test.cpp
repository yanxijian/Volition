#include "client_kind.hpp"

#include <QTest>

namespace
{
	class ClientKindTest : public QObject
	{
		Q_OBJECT

	private slots:
		void supportedExtensions_data()
		{
			QTest::addColumn<QString>("path");
			QTest::addColumn<QString>("expectedKind");

			QTest::newRow("text") << QStringLiteral("notes.txt") << QStringLiteral("text");
			QTest::newRow("uppercase text") << QStringLiteral("CONFIG.JSON") << QStringLiteral("text");
			QTest::newRow("markdown") << QStringLiteral("guide.markdown") << QStringLiteral("markdown");
			QTest::newRow("pdf") << QStringLiteral("manual.PDF") << QStringLiteral("pdf");
		}

		void supportedExtensions()
		{
			QFETCH(QString, path);
			QFETCH(QString, expectedKind);

			QCOMPARE(volition::host::clientKindForPath(path), expectedKind);
		}

		void unsupportedExtensions()
		{
			QCOMPARE(volition::host::clientKindForPath(QStringLiteral("image.png")), QString());
			QCOMPARE(volition::host::clientKindForPath(QStringLiteral("README")), QString());
		}
	};
} // namespace

QTEST_MAIN(ClientKindTest)
#include "client_kind_test.moc"