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
#include "frameutils.h"
#include "vidslidesettings.h"

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
    VidSlideSettings::VidType type = (VidSlideSettings::VidType)m_set.outSize;

    /**
     * NOTE: QIcon depend of X11 under Linux which is not re-rentrant.
     * Load this image here in first from main thread.
     */
    m_broken = QIcon::fromTheme(QLatin1String("view-preview")).pixmap(VidSlideSettings::videoSizeFromType(type)).toImage();
}

MjpegFrameTask::~MjpegFrameTask()
{
}

QByteArray MjpegFrameTask::imageToJPEGArray(const QImage& frame) const
{
    QByteArray outbuf;
    QBuffer buffer(&outbuf);
    buffer.open(QIODevice::WriteOnly);
    frame.save(&buffer, "JPEG", m_set.quality);

    return outbuf;
}

QImage MjpegFrameTask::loadImageFromPreviewCache(const QString& path) const
{
    QImage qimg;
    qCDebug(DIGIKAM_GENERAL_LOG) << "MjpegStream: Generate frame for" << path;

    DImg dimg = PreviewLoadThread::loadHighQualitySynchronously(path);

    if (dimg.isNull())
    {
        // Generate an error frame.

        qimg = m_broken;
        qCWarning(DIGIKAM_GENERAL_LOG) << "MjpegStream: Failed to load" << path;
    }
    else
    {
        // Generate real preview frame.

        qimg = dimg.copyQImage();
    }

    // Resize output image to the wanted dimensions.

    VidSlideSettings::VidType type = (VidSlideSettings::VidType)m_set.outSize;
    qimg                           = FrameUtils::makeScaledImage(qimg, VidSlideSettings::videoSizeFromType(type));

    return qimg;
}

void MjpegFrameTask::run()
{
    QImage qiimg;
    QImage qtimg;

    VidSlideSettings::VidType type = (VidSlideSettings::VidType)m_set.outSize;
    int imgFrames                  = m_set.delay * m_set.rate;

    TransitionMngr transmngr;
    transmngr.setOutputSize(VidSlideSettings::videoSizeFromType(type));

    EffectMngr effmngr;
    effmngr.setOutputSize(VidSlideSettings::videoSizeFromType(type));
    effmngr.setFrames(imgFrames);               // Ex: 30 frames at 10 img/s => 3 s of effect

    do
    {
        for (int i = 0 ; ((i < m_set.inputImages.count() + 1) && !m_cancel) ; ++i)
        {
            if (i == 0)
            {
                qiimg = FrameUtils::makeFramedImage(QString(), VidSlideSettings::videoSizeFromType(type));
            }

            QString ofile;

            if (i < m_set.inputImages.count())
            {
                ofile = m_set.inputImages[i].toLocalFile();
            }

            QImage qoimg = loadImageFromPreviewCache(ofile);

            // Apply transition between images

            transmngr.setInImage(qiimg);
            transmngr.setOutImage(qoimg);
            transmngr.setTransition(m_set.transition);

            int ttmout = 0;

            do
            {
                qtimg = transmngr.currentFrame(ttmout);

                emit signalFrameChanged(imageToJPEGArray(qtimg));

                QThread::msleep((1 / m_set.rate) * 1000);
            }
            while ((ttmout != -1) && !m_cancel);

            // TODO: apply OSD over frame.

            // Apply effect on frame

            int count  = 0;
            int itmout = 0;
            effmngr.setImage(qoimg);
            effmngr.setEffect(m_set.effect);

            do
            {
                qiimg = effmngr.currentFrame(itmout);

                emit signalFrameChanged(imageToJPEGArray(qiimg));

                count++;
                QThread::msleep((1 / m_set.rate) * 1000);
            }
            while ((count < imgFrames) && !m_cancel);
        }
    }
    while (!m_cancel && m_set.loop);

    emit signalDone();
}

} // namespace DigikamGenericMjpegStreamPlugin
