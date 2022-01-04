/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-04
 * Description : a command line tool to list MacOS apllication bundle
 *               suitable to open a type mime..
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QString>
#include <QUrl>
#include <QList>

// Local includes

#include "digikam_debug.h"

QList<QUrl> MacApplicationForFileExtension(const QString& suffix);

int main(int /*argc*/, char** /*argv*/)
{
    QString suffix;
    QList<QUrl> list;

    suffix = QLatin1String("jpg");
    list   = MacApplicationForFileExtension(suffix);

    qCDebug(DIGIKAM_TESTS_LOG) << "MacOS Bundle urls for" << suffix;
    qCDebug(DIGIKAM_TESTS_LOG) << list;

    suffix = QLatin1String("jpeg");
    list   = MacApplicationForFileExtension(suffix);

    qCDebug(DIGIKAM_TESTS_LOG) << "MacOS Bundle urls for" << suffix;
    qCDebug(DIGIKAM_TESTS_LOG) << list;

    suffix = QLatin1String("png");
    list   = MacApplicationForFileExtension(suffix);

    qCDebug(DIGIKAM_TESTS_LOG) << "MacOS Bundle urls for" << suffix;
    qCDebug(DIGIKAM_TESTS_LOG) << list;

    suffix = QLatin1String("tiff");
    list   = MacApplicationForFileExtension(suffix);

    qCDebug(DIGIKAM_TESTS_LOG) << "MacOS Bundle urls for" << suffix;
    qCDebug(DIGIKAM_TESTS_LOG) << list;

    suffix = QLatin1String("tif");
    list   = MacApplicationForFileExtension(suffix);

    qCDebug(DIGIKAM_TESTS_LOG) << "MacOS Bundle urls for" << suffix;
    qCDebug(DIGIKAM_TESTS_LOG) << list;

    return 0;
}
