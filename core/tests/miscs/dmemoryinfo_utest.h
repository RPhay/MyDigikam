/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a test for the DMemoryInfo
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DMEMORY_INFO_UTEST_H
#define DIGIKAM_DMEMORY_INFO_UTEST_H

// Qt includes

#include <QObject>

class DMemoryInfoTest : public QObject
{
    Q_OBJECT

public:

    DMemoryInfoTest(QObject* const parent = nullptr);

private Q_SLOTS:

    void isNull();
    void operators();
};

#endif // DIGIKAM_DMEMORY_INFO_UTEST_H
