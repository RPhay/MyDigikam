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

// C++ includes

#include <cmath>

// Qt includes

#include <QEventLoop>
#include <QStringList>

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

void FFmpegLauncher::encodeFrames()
{
    // Run FFmpeg CLI to encode temporary JPEG frames
    // https://shotstack.io/learn/use-ffmpeg-to-convert-images-to-video/
    // ffmpeg -f concat -i fileslist.txt -b:v 200000 -r 25 -vcodec mpeg4 -y output.mp4

    qCDebug(DIGIKAM_GENERAL_LOG) << "Start encoding with FFmpeg";

    setWorkingDirectory(m_settings->outputDir);
    setProgram(m_settings->ffmpegPath);
    setArguments(QStringList() << QLatin1String("-f")
                               << QLatin1String("concat")
                               << QLatin1String("-i")
                               << m_settings->filesList                                 // File list of frames to encode.
                               << QLatin1String("-b:v")                                 // Video bits-rate/
                               << QString::number(m_settings->videoBitRate())
                               << QLatin1String("-r")                                   // Video frames-rate.
                               << QString::number(ceil(m_settings->videoFrameRate()))
                               << QLatin1String("-vcodec")                              // Video codec.
                               << m_settings->videoCodec()
                               << QLatin1String("-y")                                   // Overwrite target.
                               << m_settings->outputFile);                              // Target video stream.
    startProcess();
}

QMap<QString, QString> FFmpegLauncher::supportedCodecs()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Get FFmpeg supported video codecs";

    setProgram(m_settings->ffmpegPath);
    setArguments(QStringList() << QLatin1String("-codecs"));

    QEventLoop loop;

    connect(this, &ProcessLauncher::signalComplete,
            &loop, &QEventLoop::quit);

    startProcess();
    loop.exec();

    QMap<QString, QString> codecMap;        // name, features
    QString out        = output().section(QLatin1String("-------"), -1);
    QStringList codecs = out.split(QLatin1Char('\n'), Qt::SkipEmptyParts);

    Q_FOREACH (const QString& line, codecs)
    {
        QStringList sections = line.simplified().split(QLatin1Char(' '), Qt::SkipEmptyParts);

        if (sections.size() >= 2)
        {
            codecMap.insert(sections[1], sections[0]);
        }
    }

    return codecMap;
}

} // namespace Digikam

#include "moc_ffmpeglauncher.cpp"
