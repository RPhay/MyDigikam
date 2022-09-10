/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "VideoCapture.h"

// Qt includes

#include <QCoreApplication>
#include <QDir>
#include <QRunnable>
#include <QThreadPool>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"

namespace QtAV
{

Q_GLOBAL_STATIC(QThreadPool, videoCaptureThreadPool)

static bool app_is_dieing = false;

// TODO: cancel if qapp is quit

class Q_DECL_HIDDEN CaptureTask : public QRunnable
{
public:

    explicit CaptureTask(VideoCapture* const c)
        : cap           (c),
          save          (true),
          original_fmt  (false),
          quality       (-1),
          format        (QLatin1String("PNG")),
          qfmt          (QImage::Format_ARGB32)
    {
        setAutoDelete(true);
    }

    virtual void run()
    {
        if (app_is_dieing)
        {
            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("app is dieing. cancel capture task %p", this);

            return;
        }

        QImage image(frame.toImage());

        if (image.isNull())
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Failed to convert to QImage");

            QMetaObject::invokeMethod(cap, "failed");

            return;
        }

        QMetaObject::invokeMethod(cap, "imageCaptured", Q_ARG(QImage, image));

        if (!save)
            return;

        bool main_thread = (QThread::currentThread() == qApp->thread());

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("capture task running in thread %p [main thread=%d]",
                QThread::currentThreadId(), main_thread);

        if (!QDir(dir).exists())
        {
            if (!QDir().mkpath(dir))
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("Failed to create capture dir [%s]",
                        qPrintable(dir));

                QMetaObject::invokeMethod(cap, "failed");

                return;
            }
        }

        name += QString::number(frame.timestamp(), 'f', 3);
        QString path(dir + QLatin1Char('/') + name + QLatin1Char('.'));

        if (original_fmt)
        {
            if (!frame.constBits(0))
            {
                frame = frame.to(frame.format());
            }

            path.append(frame.format().name());

            qCDebug(DIGIKAM_QTAV_LOG).noquote()
                << QString::asprintf("Saving capture to %s",
                    qPrintable(path));

            QFile file(path);

            if (!file.open(QIODevice::WriteOnly))
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("VideoCapture is failed to open file %s",
                        qPrintable(path));

                QMetaObject::invokeMethod(cap, "failed");

                return;
            }

            int sz           = 0;
            const char* data = (const char*)frame.frameDataPtr(&sz);

            if (file.write(data, sz) <= 0)
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                    << QString::asprintf("VideoCapture is failed to write captured frame with original format");

                QMetaObject::invokeMethod(cap, "failed");
                file.close();

                return;
            }

            file.close();
            QMetaObject::invokeMethod(cap, "saved", Q_ARG(QString, path));

            return;
        }

        if (image.isNull())
            return;

        path.append(format.toLower());

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("Saving capture to %s",
                qPrintable(path));

        bool ok = image.save(path, format.toLatin1().constData(), quality);

        if (!ok)
        {
            qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                << QString::asprintf("Failed to save capture");
            QMetaObject::invokeMethod(cap, "failed");
        }

        QMetaObject::invokeMethod(cap, "saved", Q_ARG(QString, path));
    }

    VideoCapture*   cap = nullptr;
    bool            save;
    bool            original_fmt;
    int             quality;
    QString         format;
    QString         dir;
    QString         name;
    QImage::Format  qfmt;
    VideoFrame      frame;

private:

    Q_DISABLE_COPY(CaptureTask);
};

VideoCapture::VideoCapture(QObject* const parent)
    : QObject       (parent),
      async         (true),
      auto_save     (true),
      original_fmt  (false),
      qfmt          (QImage::Format_ARGB32)
{
    dir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);       // cppcheck-suppress useInitializationList

    if (dir.isEmpty())
        dir = qApp->applicationDirPath() + QLatin1String("/capture");

    fmt  = QLatin1String("PNG");
    qual = -1;

    // seems no direct connection is fine too

    connect(qApp, SIGNAL(aboutToQuit()),
            this, SLOT(handleAppQuit()),
            Qt::DirectConnection);
}

void VideoCapture::setAsync(bool value)
{
    if (async == value)
        return;

    async = value;

    Q_EMIT asyncChanged();
}

bool VideoCapture::isAsync() const
{
    return async;
}

void VideoCapture::setAutoSave(bool value)
{
    if (auto_save == value)
        return;

    auto_save = value;

    Q_EMIT autoSaveChanged();
}

bool VideoCapture::autoSave() const
{
    return auto_save;
}

void VideoCapture::setOriginalFormat(bool value)
{
    if (original_fmt == value)
        return;

    original_fmt = value;

    Q_EMIT originalFormatChanged();
}

bool VideoCapture::isOriginalFormat() const
{
    return original_fmt;
}

void VideoCapture::handleAppQuit()
{
    app_is_dieing = true;

    // TODO: how to cancel? since qt5.2, we can use clear()

    videoCaptureThreadPool()->clear();
    videoCaptureThreadPool()->setExpiryTimeout(0);
    videoCaptureThreadPool()->waitForDone();
}

void  VideoCapture::capture()
{
    Q_EMIT requested();
}

void VideoCapture::start()
{
    Q_EMIT frameAvailable(frame);

    if (!frame.isValid() || !frame.constBits(0))
    {
        // if frame is always cloned, then size is at least width*height

        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("Captured frame from hardware decoder surface.");
    }

    CaptureTask* const task = new CaptureTask(this);

    // copy properties so the task will not be affect even if VideoCapture properties changed

    task->save              = autoSave();
    task->original_fmt      = original_fmt;
    task->quality           = qual;
    task->dir               = dir;
    task->name              = name;
    task->format            = fmt;
    task->qfmt              = qfmt;
    task->frame             = frame; // copy here and it's safe in capture thread because start() is called imediatly after setVideoFrame

    if (isAsync())
    {
        videoCaptureThreadPool()->start(task);
    }
    else
    {
        task->run();
        delete task;
    }
}

void VideoCapture::setSaveFormat(const QString& format)
{
    if (format.toLower() == fmt.toLower())
        return;

    fmt = format;

    Q_EMIT saveFormatChanged();
}

QString VideoCapture::saveFormat() const
{
    return fmt;
}

void VideoCapture::setQuality(int value)
{
    if (qual == value)
        return;

    qual = value;

    Q_EMIT qualityChanged();
}

int VideoCapture::quality() const
{
    return qual;
}

void VideoCapture::setCaptureName(const QString& value)
{
    if (name == value)
        return;

    name = value;

    Q_EMIT captureNameChanged();
}

QString VideoCapture::captureName() const
{
    return name;
}

void VideoCapture::setCaptureDir(const QString& value)
{
    if (dir == value)
        return;

    dir = value;

    Q_EMIT captureDirChanged();
}

QString VideoCapture::captureDir() const
{
    return dir;
}

/**
 * If the frame is not created for direct rendering, then the frame data is already deep copied, so detach is enough.
 * TODO: map frame from texture etc.
 */
void VideoCapture::setVideoFrame(const VideoFrame& frame)
{
    // parameter in ready(QtAV::VideoFrame) ensure we can access the frame without lock

    /*
     * clone here may block VideoThread. But if not clone here, the frame may be
     * modified outside and is not safe.
     */

    this->frame = frame.clone(); // TODO: no clone, use detach()
}

} // namespace QtAV

