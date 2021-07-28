/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : a MJPEG frame generator.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "mjpegframethread.h"

// Qt includes

#include <QString>
#include <QBuffer>
#include <QApplication>
#include <QIcon>

// Local includes

#include "digikam_debug.h"
#include "previewloadthread.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegFrameThread::MjpegFrameThread(QObject* const parent)
    : ActionThreadBase(parent)
{
}

MjpegFrameThread::~MjpegFrameThread()
{
    // cancel the thread

    cancel();

    // wait for the thread to finish

    wait();
}

void MjpegFrameThread::createFrameJob(const MjpegStreamSettings& set)
{
    ActionJobCollection collection;

    MjpegFrameTask* const t = new MjpegFrameTask(set);

    connect(t, SIGNAL(signalFrameChanged(QByteArray)),
            this, SIGNAL(signalFrameChanged(QByteArray)));

    collection.insert(t, 0);

    appendJobs(collection);
}

// -----------------------------------------------------------

MjpegFrameTask::MjpegFrameTask(const MjpegStreamSettings& set)
    : ActionJob(nullptr),
      m_set    (set)
{
    m_broken = QIcon::fromTheme(QLatin1String("view-preview")).pixmap(1920).toImage();
}

MjpegFrameTask::~MjpegFrameTask()
{
}

QByteArray MjpegFrameTask::imageToJPEGArray(const QImage& frame)
{
    QByteArray outbuf;
    QBuffer buffer(&outbuf);
    buffer.open(QIODevice::WriteOnly);
    frame.save(&buffer, "JPEG", m_set.quality);

    return outbuf;
}

void MjpegFrameTask::run()
{
    QImage img;
    DImg dimg;

    do
    {
        foreach (const QUrl& url, m_set.urlsList)
        {
            if (m_cancel)
            {
                break;
            }

            qCDebug(DIGIKAM_GENERAL_LOG) << "MjpegStream: Generate frame for" << url.toLocalFile();

            dimg = PreviewLoadThread::loadSynchronously(url.toLocalFile(), PreviewSettings(PreviewSettings::FastPreview), 1920);

            if (dimg.isNull())
            {
                // Generate an error frame.
                img = m_broken;
            }
            else
            {
                img = dimg.copyQImage();

                emit signalFrameChanged(imageToJPEGArray(img));

                QThread::sleep(m_set.delay);
            }
        }
    }
    while (!m_cancel && m_set.loop);

    emit signalDone();
}

} // namespace DigikamGenericMjpegStreamPlugin
