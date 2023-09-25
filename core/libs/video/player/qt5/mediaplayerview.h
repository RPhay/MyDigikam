/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-20-12
 * Description : a view to embed QtAv media player.
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MEDIA_PLAYER_VIEW_H
#define DIGIKAM_MEDIA_PLAYER_VIEW_H

// Qt includes

#include <QStackedWidget>
#include <QEvent>
#include <QUrl>

// QtAV includes

#include "AVError.h"
#include "AVPlayerCore.h"

// Local includes

#include "digikam_export.h"
#include "dinfointerface.h"

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

    void slotPlayerStateChanged(QtAV::AVPlayerCore::State state);
    void slotMediaStatusChanged(QtAV::MediaStatus status);
    void slotHandlePlayerError(const QtAV::AVError& err);
    void slotImageCaptured(const QImage& image);
    void slotPlayerConfigChanged();
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

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_MEDIA_PLAYER_VIEW_H
