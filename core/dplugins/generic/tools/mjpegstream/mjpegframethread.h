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

#ifndef DIGIKAM_MJPEG_FRAME_THREAD_H
#define DIGIKAM_MJPEG_FRAME_THREAD_H

// Qt includes

#include <QImage>
#include <QByteArray>
#include <QColor>
#include <QPoint>
#include <QFont>
#include <QDateTime>

// Local includes

#include "actionthread.h"
#include "mjpegserver.h"
#include "mjpegstreamsettings.h"

using namespace Digikam;

namespace DigikamGenericMjpegStreamPlugin
{

class MjpegFrameThread : public ActionThreadBase
{
    Q_OBJECT

public:

    explicit MjpegFrameThread(QObject* const parent);
    ~MjpegFrameThread() override;

    void createFrameJob(const MjpegStreamSettings&);

Q_SIGNALS:

    void signalFrameChanged(const QByteArray&);
};

// ---------------------------------------------------------------

class MjpegFrameTask : public ActionJob
{
    Q_OBJECT

public:

    explicit MjpegFrameTask(const MjpegStreamSettings& set);
    ~MjpegFrameTask();

Q_SIGNALS:

    /**
     * Output JPEG frames from generator thread (emitted to server thread).
     */
    void signalFrameChanged(const QByteArray& frame);

private:

    /**
     * Helper monvert a QImage to a byte-aaray of JPEG data file.
     */
    QByteArray imageToJPEGArray(const QImage& frame);

    void run();

private:

    MjpegStreamSettings m_set;          ///< The MJPEG stream settings.
    QImage              m_broken;       ///< Image to push as frame if current item from list cannot be loaded.
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_FRAME_THREAD_H
