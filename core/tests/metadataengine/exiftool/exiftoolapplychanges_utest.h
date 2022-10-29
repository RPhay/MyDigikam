/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a unit-test to write metadata with ExifTool and EXV constainer.
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EXIFTOOL_APPLYCHANGES_UTEST_H
#define DIGIKAM_EXIFTOOL_APPLYCHANGES_UTEST_H

// Qt includes

#include <QString>
#include <QObject>

// Local includes

#include "abstractunittest.h"

class ExifToolApplyChangesTest : public AbstractUnitTest
{
    Q_OBJECT

public:

    explicit ExifToolApplyChangesTest(QObject* const parent = nullptr);

private Q_SLOTS:

    void testExifToolApplyChanges();
};

#endif // DIGIKAM_EXIFTOOL_APPLYCHANGES_UTEST_H
