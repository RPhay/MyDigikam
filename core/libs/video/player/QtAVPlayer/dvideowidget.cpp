/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-01-14
 * Description : QtAVPlayer video integration class.
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dvideowidget.h"

// Qt includes

#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QAbstractVideoSurface>
#include <QVideoRendererControl>
#include <QVideoSurfaceFormat>
#include <QMediaService>

namespace Digikam
{

class Q_DECL_HIDDEN VideoRenderer : public QVideoRendererControl
{
public:

    QAbstractVideoSurface* surface() const override
    {
        return m_surface;
    }

    void setSurface(QAbstractVideoSurface* surface) override
    {
        m_surface = surface;
    }

    QAbstractVideoSurface* m_surface = nullptr;
};

// -------------------------------------------------------

class Q_DECL_HIDDEN MediaService : public QMediaService
{
public:

    MediaService(VideoRenderer* const vr, QObject* const parent = nullptr)
        : QMediaService(parent),
          m_renderer   (vr)
    {
    }

    QMediaControl* requestControl(const char* name) override
    {
        if (qstrcmp(name, QVideoRendererControl_iid) == 0)
        {
            return m_renderer;
        }

        return nullptr;
    }

    void releaseControl(QMediaControl*) override
    {
    }

    VideoRenderer* m_renderer = nullptr;
};

// --------------------------------------------------------

class Q_DECL_HIDDEN MediaObject : public QMediaObject
{
public:

    explicit MediaObject(VideoRenderer* const vr, QObject* const parent = nullptr)
        : QMediaObject(parent, new MediaService(vr, parent))
    {
    }
};

// --------------------------------------------------------

class Q_DECL_HIDDEN DVideoWidget::Private
{

public:

    Private() = default;

    VideoRenderer*  videoRender = nullptr;
    MediaObject*    mediaObject = nullptr;
    QAVPlayer*      player      = nullptr;
    QAVAudioOutput* audioOutput = nullptr;

    QVideoFrame     videoFrame;
    QMutex          mutex;
};

DVideoWidget::DVideoWidget(QWidget* const /*parent*/)
    : QVideoWidget(),
      d           (new Private)
{
    setMouseTracking(true);

    d->videoRender = new VideoRenderer;

    d->mediaObject = new MediaObject(d->videoRender);
    setMediaObject(d->mediaObject);

    d->player      = new QAVPlayer(this);

    d->audioOutput = new QAVAudioOutput(this);

    connect(d->player, &QAVPlayer::audioFrame,
            this, &DVideoWidget::slotAudioFrame,
            Qt::DirectConnection);

    connect(d->player, &QAVPlayer::videoFrame,
            this, &DVideoWidget::slotVideoFrame,
            Qt::DirectConnection);
}

DVideoWidget::~DVideoWidget()
{
    setMediaObject(nullptr);
    delete d;
}

QAVPlayer* DVideoWidget::player() const
{
    return d->player;
}

QAVAudioOutput* DVideoWidget::audioOutput() const
{
    return d->audioOutput;
}

QVideoFrame DVideoWidget::videoFrame() const
{
    QMutexLocker lock(&d->mutex);

    return d->videoFrame;
}

bool DVideoWidget::setMediaObject(QMediaObject* object)
{
    return QVideoWidget::setMediaObject(object);
}

void DVideoWidget::slotAudioFrame(const QAVAudioFrame& frame)
{
    d->audioOutput->play(frame);
}

void DVideoWidget::slotVideoFrame(const QAVVideoFrame& frame)
{
    if (!d->videoRender->m_surface)
    {
        return;
    }

    {
        QMutexLocker lock(&d->mutex);

        d->videoFrame = frame.convertTo(AV_PIX_FMT_RGB32);
    }

    if (
        !d->videoRender->m_surface->isActive() ||
        (d->videoRender->m_surface->surfaceFormat().frameSize() != d->videoFrame.size())
       )
    {
        QVideoSurfaceFormat f(d->videoFrame.size(), d->videoFrame.pixelFormat(), d->videoFrame.handleType());
        d->videoRender->m_surface->start(f);
    }

    if (d->videoRender->m_surface->isActive())
    {
         d->videoRender->m_surface->present(d->videoFrame);
    }

    Q_EMIT positionChanged(d->player->position());
}

int DVideoWidget::videoMediaOrientation() const
{
    int orientation = 0;

    QList<QAVStream> vstream = d->player->currentVideoStreams();

    if (!vstream.isEmpty())
    {
        QMap<QString, QString> vals = vstream.first().metadata();

        if (!vals.isEmpty())
        {
            if (vals.contains(QLatin1String("Orientation")))
            {
                orientation = vals[QLatin1String("Orientation")].toInt();
            }
        }
    }

    return orientation;
}

} // namespace Digikam

#include "moc_dvideowidget.cpp"
