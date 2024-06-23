/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-20-12
 * Description : a view to host media player based on QtAVPlayer.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QStackedWidget>
#include <QEvent>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "dinfointerface.h"
#include "dvideowidget.h"

namespace Digikam
{

class DIGIKAM_EXPORT MediaPlayerView : public QStackedWidget
{
    Q_OBJECT

public:

    explicit MediaPlayerView(QWidget* const parent);
    ~MediaPlayerView() override;

    void setCurrentItem(const QUrl& url   = QUrl(),
                        bool  hasPrevious = false,
                        bool  hasNext     = false);

    void setInfoInterface(DInfoInterface* const iface);
    void escapePreview();
    void reload();

Q_SIGNALS:

    void signalNextItem();
    void signalPrevItem();
    void signalEscapePreview();

public Q_SLOTS:

    void slotEscapePressed();
    void slotRotateVideo();

private Q_SLOTS:

    void slotPlayerStateChanged(QAVPlayer::State newState);
    void slotMediaStatusChanged(QAVPlayer::MediaStatus newStatus);
    void slotHandlePlayerError(QAVPlayer::Error err, const QString&);
    void slotPlayingStateChanged();
    void slotThemeChanged();

    /// Slidebar slots
    void slotPositionChanged(qint64 position);
    void slotDurationChanged(qint64 duration);
    void slotVolumeChanged(int volume);
    void slotLoopToggled(bool loop);
    void slotPosition(int position);
    void slotPausePlay();
    void slotCapture();

private:

    int  previewMode();
    void setPreviewMode(int mode);

protected:

    bool eventFilter(QObject* watched, QEvent* event) override;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
