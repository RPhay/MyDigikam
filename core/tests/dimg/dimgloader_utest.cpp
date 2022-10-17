/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-23
 * Description : an unit-test to check DImg image loader
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dimgloader_utest.h"

// Qt includes

#include <QApplication>
#include <QTest>
#include <QFileInfo>
#include <QList>
#include <QDir>

// Local includes

#include "digikam_debug.h"
#include "metaengine.h"
#include "dimg.h"
#include "dpluginloader.h"
#include "dtestdatadir.h"
#include "drawdecoding.h"

QTEST_GUILESS_MAIN(DImgLoaderTest)

DImgLoaderTest::DImgLoaderTest(QObject* const parent)
    : QObject(parent)
{
}

void DImgLoaderTest::testDimgLoader()
{
    MetaEngine::initializeExiv2();
    QDir dir(qApp->applicationDirPath());
    qputenv("DK_PLUGIN_PATH", dir.canonicalPath().toUtf8());
    DPluginLoader::instance()->init();

    QVERIFY2(DPluginLoader::instance()->allPlugins().isEmpty(),
             "Not able to found digiKam plugin in standard paths. Test is aborted...");

    QString fname = DTestDataDir::TestData(QString::fromUtf8("core/tests/dimg"))
                  .root().path() + QLatin1String("/DSC00636.JPG");
    qCDebug(DIGIKAM_TESTS_LOG) << "Test Data File:" << fname;

    LoadingDescription desc(m_fname, DRawDecoding(settings));

    m_thread->load(desc);

    int i = 0;

    do
    {
        QTest::qWait(1000);
        i++;
    }
    while ((i < 10) && !(m_loaded && m_saved));

    QVERIFY2(m_loadedProgress, "Failed to progress image loading");
    QVERIFY2(m_savedProgress,  "Failed to progress image saving");
    QVERIFY2(m_loaded,         "Failed to load image");
    QVERIFY2(m_saved,          "Failed to save image");

    DPluginLoader::instance()->cleanUp();
}
