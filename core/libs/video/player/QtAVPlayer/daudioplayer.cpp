/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-01-14
 * Description : QtAVPlayer audio integration class.
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "daudioplayer.h"

// Qt includes

#include <QMediaService>

namespace Digikam
{

class Q_DECL_HIDDEN DAudioPlayer::Private
{

public:

    Private() = default;

    QAVPlayer*      player      = nullptr;
    QAVAudioOutput* audioOutput = nullptr;
};

DAudioPlayer::DAudioPlayer(QObject* const parent)
    : QObject(parent),
      d      (new Private)
{
    d->player      = new QAVPlayer(this);

    d->audioOutput = new QAVAudioOutput(this);

    connect(d->player, &QAVPlayer::audioFrame,
            this, &DAudioPlayer::slotAudioFrame,
            Qt::DirectConnection);
}

DAudioPlayer::~DAudioPlayer()
{
    delete d;
}

QAVPlayer* DAudioPlayer::player() const
{
    return d->player;
}

QAVAudioOutput* DAudioPlayer::audioOutput() const
{
    return d->audioOutput;
}

void DAudioPlayer::slotAudioFrame(const QAVAudioFrame& frame)
{
    d->audioOutput->play(frame);

    Q_EMIT positionChanged(d->player->position());
}

void DAudioPlayer::pause()
{
    d->player->pause();
}

void DAudioPlayer::play()
{
    d->player->play();
}

void DAudioPlayer::stop()
{
    d->player->stop();
}

void DAudioPlayer::setSource(const QUrl& url)
{
    d->player->setSource(url.toLocalFile());
}

qint64 DAudioPlayer::position()
{
    return d->player->position();
}

qint64 DAudioPlayer::duration()
{
    return d->player->duration();
}

} // namespace Digikam

#include "moc_daudioplayer.cpp"
