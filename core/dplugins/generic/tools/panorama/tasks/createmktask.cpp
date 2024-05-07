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

#include "createmktask.h"

// Qt includes

#include <QFileInfo>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericPanoramaPlugin
{

CreateMKTask::CreateMKTask(const QString& workDirPath, const QUrl& input, QUrl& mkUrl,
                           QUrl& panoUrl, PanoramaFileType fileType,
                           const QString& pto2mkPath, bool preview)
    : CommandTask(preview ? PANO_CREATEMKPREVIEW : PANO_CREATEMK, workDirPath, pto2mkPath),
      ptoUrl     (input),
      mkUrl      (mkUrl),
      panoUrl    (panoUrl),
      fileType   (fileType)
{
}

void CreateMKTask::run(ThreadWeaver::JobPointer, ThreadWeaver::Thread*)
{
    panoUrl = tmpDir;
    mkUrl   = tmpDir;
    QFileInfo fi(ptoUrl.toLocalFile());
    mkUrl.setPath(mkUrl.path() + fi.completeBaseName() + QLatin1String(".mk"));

    switch (fileType)
    {
        case JPEG:
        {
            panoUrl.setPath(panoUrl.path() + fi.completeBaseName() + QLatin1String(".jpg"));
            break;
        }

        case TIFF:
        {
            panoUrl.setPath(panoUrl.path() + fi.completeBaseName() + QLatin1String(".tif"));
            break;
        }

        case HDR:
        {
            panoUrl.setPath(panoUrl.path() + fi.completeBaseName() + QLatin1String(".hdr"));
            break;
        }
    }

    QStringList args;
    args << QLatin1String("-o");
    args << mkUrl.toLocalFile();
    args << QLatin1String("-p");
    args << fi.completeBaseName();
    args << ptoUrl.toLocalFile();

    runProcess(args);

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "pto2mk command line: " << getCommandLine();

    qCDebug(DIGIKAM_DPLUGIN_GENERIC_LOG) << "pto2mk output:" << Qt::endl << output;
}

} // namespace DigikamGenericPanoramaPlugin
