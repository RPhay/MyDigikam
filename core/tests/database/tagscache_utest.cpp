/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-20
 * Description : Unit tests for TagsCache class
 *
 * Copyright (C) 2021 by David Haslam, <dch dot code at gmail dot com>
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

#include "tagscache_utest.h"

// C++ includes

#include <set>

// Qt includes

#include <QString>
#include <QDebug>
#include <QtTest>

// Local includes

#include "coredb.h"
#include "coredbaccess.h"


void TagsCacheTest::initTestCase()
{
    qDebug() << "initTestCase()";
    Digikam::DbEngineParameters db_params
        (
            QString::fromUtf8("QSQLITE"),      // databaseType
            QString::fromUtf8(":memory:"),     // databaseNameCore
            QString::fromUtf8("")              // connectOptions
        );

    Digikam::CoreDbAccess::setParameters(db_params);
    QVERIFY(Digikam::CoreDbAccess::checkReadyForUse());
    tags_cache = Digikam::TagsCache::instance();
}

void TagsCacheTest::cleanupTestCase()
{
    qDebug() << "cleanupTestCase()";
}

void TagsCacheTest::init()
{
    QCOMPARE(countTags(), 0);
}

void TagsCacheTest::cleanup()
{
    auto coredb = Digikam::CoreDbAccess().db();
    auto tags   = coredb->getTagShortInfos();

    for (auto tag: tags)
    {
        coredb->deleteTag(tag.id);
    }

    QCOMPARE(countTags(), 0);
}

void TagsCacheTest::testSimpleHierarchy()
{
    auto id = tags_cache->createTag(QLatin1String("Top/Middle/Bottom"));

    dumpTags();
    QCOMPARE(countTags(), 3);

    auto bottom_id = tags_cache->tagForPath(QLatin1String("Top/Middle/Bottom"));
    auto middle_id = tags_cache->parentTag(id);
    auto top_id    = tags_cache->parentTag(middle_id);

    QCOMPARE(id, bottom_id);
    QVERIFY(bottom_id);
    QVERIFY(middle_id);
    QVERIFY(top_id);
    QVERIFY(tags_cache->hasTag(bottom_id));
    QVERIFY(tags_cache->hasTag(middle_id));
    QVERIFY(tags_cache->hasTag(top_id));
    QCOMPARE(tags_cache->tagName(id), QLatin1String("Bottom"));
    QCOMPARE(tags_cache->tagName(middle_id), QLatin1String("Middle"));
    QCOMPARE(tags_cache->tagName(top_id), QLatin1String("Top"));

    auto bottom_ids = tags_cache->tagsForName(QLatin1String("Bottom"));

    QCOMPARE(bottom_ids.size(), 1);
    QCOMPARE(bottom_ids[0], bottom_id);
}

void TagsCacheTest::testComplexHierarchy()
{
    tags_cache->createTag(QLatin1String("Top/Middle"));
    tags_cache->createTag(QLatin1String("Top/Middle/First"));
    tags_cache->createTag(QLatin1String("Top/Middle/Second"));
    tags_cache->createTag(QLatin1String("Super/Top"));
    tags_cache->createTag(QLatin1String("Super/Top/First"));
    tags_cache->createTag(QLatin1String("Super/Top/Second"));
    tags_cache->createTag(QLatin1String("Super/Top/Third"));
    tags_cache->createTag(QLatin1String("Mixed Up/Third/Second"));
    tags_cache->createTag(QLatin1String("Mixed Up/Third/First"));

    QCOMPARE(countTags(), 13);
    auto top_ids = tags_cache->tagsForName(QLatin1String("Top"));
    QCOMPARE(top_ids.size(), 2);
    auto first_ids = tags_cache->tagsForName(QLatin1String("First"));
    QCOMPARE(first_ids.size(), 3);
}

void TagsCacheTest::testRepeatedNames()
{
    tags_cache->createTag(QLatin1String("Repeat Me/Repeat Me/Repeat Me/Repeat Me"));

    auto repeat_me_ids = tags_cache->tagsForName(QLatin1String("Repeat Me"));
    QCOMPARE(repeat_me_ids.size(), 4);

    // all ids should be unique

    std::set<int> set;

    for (auto id: repeat_me_ids)
    {
        auto result = set.insert(id);
        QVERIFY(result.second);
    }
}

void TagsCacheTest::testDuplicateTop()
{
    tags_cache->createTag(QLatin1String("Top/Middle/Bottom"));
    tags_cache->createTag(QLatin1String("Super/Top/Middle/Bottom"));
    tags_cache->createTag(QLatin1String("First/Super/Top"));
    dumpTags();
    QCOMPARE(countTags(), 10);

    // the single word 'Top' should match the top tag

    auto id1 = tags_cache->tagForPath(QLatin1String("Top"));
    QVERIFY(id1);
    QCOMPARE(tags_cache->tagPath(id1), QLatin1String("/Top"));

    // and it doesn't matter if there's a leading slash

    auto id2 = tags_cache->tagForPath(QLatin1String("/Top"));
    QVERIFY(id2);
    QCOMPARE(id1, id2);
    QCOMPARE(tags_cache->tagPath(id2), QLatin1String("/Top"));

    // a more complex request

    auto id3 = tags_cache->tagForPath(QLatin1String("Super/Top"));
    QVERIFY(id3);
    QCOMPARE(tags_cache->tagPath(id3), QLatin1String("/Super/Top"));
}

// utilities

int TagsCacheTest::countTags()
{
    auto coredb = Digikam::CoreDbAccess().db();
    auto tags   = coredb->getTagShortInfos();
    return tags.size();
}

// debug

void TagsCacheTest::dumpTables()
{
    auto sqlnames = QSqlDatabase::connectionNames();
    auto sqldb    = QSqlDatabase::database(sqlnames[0]);
    qDebug() << sqldb.tables();
}

void TagsCacheTest::dumpTags()
{
    auto coredb = Digikam::CoreDbAccess().db();
    auto tags   = coredb->getTagShortInfos();

    for (auto it = tags.begin() ; it != tags.end() ; ++it)
    {
        qDebug() << it->id << it->pid << it->name;
    }
}

QTEST_GUILESS_MAIN(TagsCacheTest)
