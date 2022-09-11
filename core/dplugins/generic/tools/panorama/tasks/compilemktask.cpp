/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-03-15
 * Description : a tool to create panorama by fusion of several images.
 *
 * SPDX-FileCopyrightText: 2012-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "compilemktask.h"

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"

namespace DigikamGenericPanoramaPlugin
{

CompileMKTask::CompileMKTask(const QString& workDirPath,
                             const QUrl& mkUrl,
                             const QUrl& /*panoUrl*/,
                             const QString& nonaPath,
                             const QString& enblendPath,
                             const QString& makePath,
                             bool preview)
    : CommandTask(preview ? PANO_STITCHPREVIEW : PANO_STITCH, workDirPath, makePath),
/*
      panoUrl(&panoUrl),
*/
      mkUrl(mkUrl),
      nonaPath(nonaPath),
      enblendPath(enblendPath)
{
}

void CompileMKTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    QStringList args;
    args << QLatin1String("-f");
    args << mkUrl.toLocalFile();
    args << QString::fromLatin1("ENBLEND='%1'").arg(enblendPath);
    args << QString::fromLatin1("NONA='%1'").arg(nonaPath);

    runProcess(args);

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "make command line: " << getCommandLine();

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "make output:" << QT_ENDL << output;
}

} // namespace DigikamGenericPanoramaPlugin
