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

#ifndef DIGIKAM_DIMGFILTERACTIONKEYS_UTEST_H
#define DIGIKAM_DIMGFILTERACTIONKEYS_UTEST_H

// Qt includes

#include <QTest>

// local includes

#include "dimg.h"
#include "dimgfiltermanager.h"

using namespace Digikam;

class DImgFilterActionKeysTest : public QObject
{
    Q_OBJECT

public:

    explicit DImgFilterActionKeysTest(QObject* const parent = nullptr);

private Q_SLOTS:

    void initTestCase();
    void cleanupTestCase();
    void testAllFilters();

private:

    bool hasUniqueKeys(const FilterAction& action);
};

#endif // DIGIKAM_DIMGFILTERACTIONKEYS_UTEST_H
