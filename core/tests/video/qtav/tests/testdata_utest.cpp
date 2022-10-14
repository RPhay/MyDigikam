/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-10-03
 * Description : unit test to verify test data
 *
 * SPDX-FileCopyrightText: 2022 by Steve Robbins <steve at sumost dot ca>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QCoreApplication>
#include <QFile>
#include <QTest>

// Local includes

#include "qtavtestdatadir.h"

class DataTest : public QObject
{
    Q_OBJECT

private:

    QtAVTestDataDir m_testDataDir;

private Q_SLOTS:

    void testRootExists()
    {
        auto tdd = DTestDataDir::TestDataRoot();
        QVERIFY(tdd.isValid());
        QVERIFY(tdd.root().exists());
        QCOMPARE(tdd.root().dirName(), QString::fromUtf8("test-data"));
        QVERIFY(tdd.file(QString::fromUtf8("core/tests/video/qtav/video/mpeg2.mp4")).exists());
    }

    void validSubdirOk()
    {
        auto tdd = DTestDataDir::TestData(QString::fromUtf8("core/tests/video/qtav"));
        QVERIFY(tdd.isValid());
        QVERIFY(tdd.root().exists());
        QCOMPARE(tdd.root().dirName(), QString::fromUtf8("qtav"));
        QVERIFY(tdd.file(QString::fromUtf8("video/mpeg2.mp4")).exists());
    }

    void invalidSubdirError()
    {
        auto tdd = DTestDataDir::TestData(QString::fromUtf8("non-existent dir"));
        QVERIFY(tdd.isValid() == false);
    }

    void verifyDataExists()
    {
        QVERIFY(m_testDataDir.isValid());

        auto f1 = QFile(m_testDataDir.riffMpeg_avi());
        QVERIFY(f1.exists());
        QCOMPARE(f1.size(), 343982);

        auto f2 = QFile(m_testDataDir.basemediav1_mp4());
        QVERIFY(f2.exists());
        QCOMPARE(f2.size(), 56246);
    }
};

QTEST_MAIN(DataTest)

#include "testdata_utest.moc"
