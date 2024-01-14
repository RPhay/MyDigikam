/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-01-14
 * Description : QtAVPlayer integration class.
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dvideowidget.h"

namespace Digikam
{

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

VideoRenderer* DVideoWidget::videoRender() const
{
    return d->videoRender;
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
    if (d->videoRender->m_surface == nullptr)
    {
        return;
    }

    QVideoFrame videoFrame = frame.convertTo(AV_PIX_FMT_RGB32);

    if (
        !d->videoRender->m_surface->isActive() ||
        (d->videoRender->m_surface->surfaceFormat().frameSize() != videoFrame.size())
       )
    {
        QVideoSurfaceFormat f(videoFrame.size(), videoFrame.pixelFormat(), videoFrame.handleType());
        d->videoRender->m_surface->start(f);
    }

    if (d->videoRender->m_surface->isActive())
    {
         d->videoRender->m_surface->present(videoFrame);
    }

    Q_EMIT positionChanged(d->player->position());
}

} // namespace Digikam

#include "moc_dvideowidget.cpp"
