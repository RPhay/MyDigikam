/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to write metadata with ExifTool and EXV constainer
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt includes

#include <QString>
#include <QCoreApplication>
#include <QVariant>

// Local includes

#include "digikam_debug.h"
#include "dmetadata.h"
#include "dpluginloader.h"
#include "metaengine.h"
#include "exiftoolparser.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "exiftoolapplychanges_cli - CLI tool to write metadata with ExifTool in image using EXV constainer";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <image to patch>";
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

    QString exvPath = input.baseName() + QLatin1String("_changes.exv");
    meta.exportChanges(exvPath);

    ExifToolParser* const parser = new ExifToolParser(qApp);

    if (!parser->applyChanges(input.filePath(), exvPath))
    {
        qCWarning(DIGIKAM_TESTS_LOG) << "Cannot apply changes with ExifTool on" << input.filePath();
        return -1;
    }

    return 0;
}
