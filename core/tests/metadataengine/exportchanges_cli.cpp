/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-23
 * Description : a command line tool to export metatada changes to EXV
 *
 * Copyright (C) 2012-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QFileInfo>
#include <QString>
#include <QApplication>

// Local includes

#include "dmetadata.h"
#include "digikam_debug.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "dmetadatadiff_cli - CLI tool to export metadata changes to EXV";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <image>";
        return -1;
    }

    MetaEngine::initializeExiv2();

    QFileInfo input(QString::fromUtf8(argv[1]));

    DMetadata meta;
    bool ret = meta.load(input.filePath());

    if (!ret)
    {
        qCWarning(DIGIKAM_TESTS_LOG) << "Cannot load" << meta.getFilePath();
        return -1;
    }

    meta.setImageDateTime(QDateTime::currentDateTime(), true);

    QString     exvPath = input.baseName() + QLatin1String("_changes.exv");
    QStringList removedTags;
    meta.exportChanges(exvPath, removedTags);

    return 0;
}

