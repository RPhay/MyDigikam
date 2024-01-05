/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : FFmpeg launcher to encode frames as video
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "ffmpeglauncher.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

FFmpegLauncher::FFmpegLauncher(QObject* const parent)
    : ProcessLauncher(parent)
{
}

FFmpegLauncher::~FFmpegLauncher()
{
}

void FFmpegLauncher::setSettings(VidSlideSettings* const settings)
{
    m_settings = settings;
}

bool FFmpegLauncher::encodeFrames()
{
    // Run FFmpeg CLI to encode temporary JPEG frames
    // https://shotstack.io/learn/use-ffmpeg-to-convert-images-to-video/
    // ffmpeg -f concat -i fileslist.txt output.mp4

    qCDebug(DIGIKAM_GENERAL_LOG) << "Start encoding with FFmpeg";

    setWorkingDirectory(m_settings->outputDir);
    setProgram(m_settings->ffmpegPath);
    setArguments(QStringList() << QLatin1String("-f")
                               << QLatin1String("concat")
                               << QLatin1String("-i")
                               << m_settings->filesList
                               << m_settings->outputFile);
    bool successFlag = startProcess();


//    QString output   = QString::fromLocal8Bit(d->ffmpegProc->readAll());

//    qCDebug(DIGIKAM_GENERAL_LOG) << "Encoding return:" << successFlag;
//    qCDebug(DIGIKAM_GENERAL_LOG) << "Encoding trace :" << output;

    if (!successFlag)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot generate output video" << m_settings->outputVideo;
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Output video is" << m_settings->outputVideo;
    }

    return true;
}

} // namespace Digikam

#include "moc_ffmpeglauncher.cpp"
