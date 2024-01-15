/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-14
 * Description : QtAVPlayer audio integration class.
 *
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DAUDIO_PLAYER_H
#define DIGIKAM_DAUDIO_PLAYER_H

// Qt includes

#include <QObject>
#include <QUrl>

// QtAVPlayer includes

#include "qavplayer.h"
#include "qavaudioframe.h"
#include "qavaudiooutput.h"

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DAudioPlayer : public QObject
{
    Q_OBJECT

public:

    explicit DAudioPlayer(QObject* const parent);
    ~DAudioPlayer();

    QAVPlayer*      player()             const;
    QAVAudioOutput* audioOutput()        const;

    void pause();
    void play();
    void stop();
    void setSource(const QUrl&);
    qint64 position()                    const;
    qint64 duration()                    const;
    void setVolume(qreal v);
    QAVPlayer::State state()             const;
    QAVPlayer::MediaStatus mediaStatus() const;

Q_SIGNALS:

    void positionChanged(qint64);

private Q_SLOTS:

    void slotAudioFrame(const QAVAudioFrame& frame);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DAUDIO_PLAYER_H
