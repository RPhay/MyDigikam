/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-06-09
 * Description : a test for verifying unique keys in FilterAction for all filters
 *
 * Copyright (C) 2021 by Anjani Kumar <anjanik012 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dimgfilteractionkeys_utest.h"

// Qt includes

#include <QDir>
#include <QHash>
#include <QSet>
#include <QDebug>

// local includes

#include "digikam_debug.h"
#include "metaengine.h"
#include "dpluginloader.h"
#include "dimgthreadedfilter.h"

QTEST_MAIN(DImgFilterActionKeysTest)

DImgFilterActionKeysTest::DImgFilterActionKeysTest(QObject* const parent)
    : QObject(parent)
{
}

void DImgFilterActionKeysTest::testAllFilters()
{
    DImgFilterManager* const filterManager = DImgFilterManager::instance();
    QStringList identifiers                = filterManager->supportedFilters();

    foreach (const QString& identifier, identifiers)
    {
        QList<int> versions = filterManager->supportedVersions(identifier);

        foreach (int version, versions)
        {
            auto filter = filterManager->createFilter(identifier, version);

            if (filter != nullptr)
            {
                qCDebug(DIGIKAM_TESTS_LOG) << "Name: " << identifier;
                qCDebug(DIGIKAM_TESTS_LOG) << "Version: " << version;

                QVERIFY(hasUniqueKeys(filter->filterAction()));
            }
        }
    }
}

void DImgFilterActionKeysTest::initTestCase()
{
    MetaEngine::initializeExiv2();
    QDir dir(QFINDTESTDATA("../../dplugins/dimg"));
    qputenv("DK_PLUGIN_PATH", dir.canonicalPath().toUtf8());
    DPluginLoader::instance()->init();
}

void DImgFilterActionKeysTest::cleanupTestCase()
{
    DPluginLoader::instance()->cleanUp();
}

bool DImgFilterActionKeysTest::hasUniqueKeys(const FilterAction &action)
{
    QHash<QString, QVariant> params = action.parameters();
    QSet<QString> keys;
    bool uniqueKeys                 = true;

    QHash<QString, QVariant>::iterator it;

    for (it = params.begin() ; it != params.end() ; ++it)
    {
        if (keys.contains(it.key()))
        {
            uniqueKeys = false;
            break;
        }
        else
        {
            keys.insert(it.key());
        }
    }

    return uniqueKeys;
}
