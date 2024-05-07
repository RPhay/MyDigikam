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

#include "compilemksteptask.h"

// Qt includes

#include <QFileInfo>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericPanoramaPlugin
{

CompileMKStepTask::CompileMKStepTask(const QString& workDirPath, int id, const QUrl& mkUrl,
                                     const QString& nonaPath, const QString& enblendPath,
                                     const QString& makePath, bool preview)
    : CommandTask(preview ? PANO_NONAFILEPREVIEW : PANO_NONAFILE, workDirPath, makePath),
      id         (id),
      mkUrl      (mkUrl),
      nonaPath   (nonaPath),
      enblendPath(enblendPath)
{
}

void CompileMKStepTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    QFileInfo fi(mkUrl.toLocalFile());

    QString mkFile = fi.completeBaseName()                                   + 
                     QString::number(id).rightJustified(4, QLatin1Char('0')) + 
                     QLatin1String(".tif");

    QStringList args;
    args << QLatin1String("-f");
    args << mkUrl.toLocalFile();
    args << QString::fromLatin1("ENBLEND='%1'").arg(enblendPath);
    args << QString::fromLatin1("NONA='%1'").arg(nonaPath);
    args << mkFile;

    runProcess(args);

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "make job command line: " << getCommandLine();

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "make job output (" << mkFile << "):" << Qt::endl << output;
}

} // namespace DigikamGenericPanoramaPlugin
