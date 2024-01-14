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

    d->videoRender    = new VideoRenderer;

    d->mediaObject    = new MediaObject(d->videoRender);
    setMediaObject(d->mediaObject);

    d->player         = new QAVPlayer(this);

    d->audioOutput    = new QAVAudioOutput(this);
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

} // namespace Digikam

#include "moc_dvideowidget.cpp"
