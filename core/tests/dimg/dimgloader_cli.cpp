/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-23
 * Description : a command line tool to test DImg image loader
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include <QFileInfo>
#include <QApplication>

// Local includes

#include "digikam_debug.h"
#include "metaengine.h"
#include "dimg.h"
#include "drawdecoding.h"
#include "dpluginloader.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "testdimgloader - test DImg image loader";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <image>";
        return -1;
    }

    MetaEngine::initializeExiv2();

    DPluginLoader::instance()->init();

    QFileInfo input(QString::fromUtf8(argv[1]));
    QString   outFilePath(input.baseName() + QLatin1String(".heic"));

    DRawDecoderSettings settings;
    settings.halfSizeColorImage    = false;
    settings.sixteenBitsImage      = false;
    settings.RGBInterpolate4Colors = false;
    settings.RAWQuality            = DRawDecoderSettings::BILINEAR;

    DImg img(input.filePath(), nullptr, DRawDecoding(settings));

    if (!img.isNull())
    {
        img.save(outFilePath, QLatin1String("heic"));
    }

    DPluginLoader::instance()->cleanUp();

    return 0;
}
