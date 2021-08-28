/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-04
 * Description : an unit-test to detect image quality level
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_IMGQSORT_TEST_H
#define DIGIKAM_IMGQSORT_TEST_H

// Qt includes

#include <QObject>
#include <QDir>
#include <QMultiMap>
#include <QString>

// Local includes

#include "imgqsorttest_shared.h"
#include "digikam_globals.h"
#include "imagequalitycontainer.h"
#include "dpluginloader.h"

// Shared class for utest

using namespace Digikam;

using PairImageQuality = QPair<QString, int>;
using DataTestCases = QMultiMap<QString, PairImageQuality> ;

class ImgQSortTest : public QObject
{
    Q_OBJECT

public:

    explicit ImgQSortTest(QObject* const parent = nullptr):QObject(parent){}

protected:

    QDir imageDir() const
    {
        QDir dir(QFINDTESTDATA("data/"));
        qCDebug(DIGIKAM_TESTS_LOG) << "Images Directory:" << dir;
        return dir;
    }

    template <typename Function, typename Parameter>
    QHash<QString, bool> testParseTestImages(const QString& testcase_name, Function ParseTestFunc, Parameter parameter)
    {
        QStringList imageNames;

        QList<PairImageQuality> dataTest = getDataTestCases().values(testcase_name);

        for (const auto& image_refQuality : dataTest)
        {
            imageNames << image_refQuality.first;
        }

        QFileInfoList list = imageDir().entryInfoList(imageNames,QDir::Files, QDir::Name);

        QHash<QString, int> results_detection = ParseTestFunc(parameter, list);

        QHash<QString, bool> results_test;

        for (const auto& image_refQuality : dataTest)
        {
            results_test.insert(image_refQuality.first, results_detection.value(image_refQuality.first) == image_refQuality.second);
        }

        return results_test;
    }

    DataTestCases getDataTestCases() const
    {
        return m_dataTestCases;
    }

protected Q_SLOTS:

    void initTestCase()
    {
        QDir dir(QFINDTESTDATA("../../dplugins/dimg"));
        qputenv("DK_PLUGIN_PATH", dir.canonicalPath().toUtf8());
        DPluginLoader::instance()->init();
    }

    void cleanupTestCase(){}

protected:

    DataTestCases m_dataTestCases;
};

#endif // DIGIKAM_IMGQSORT_TEST_H
