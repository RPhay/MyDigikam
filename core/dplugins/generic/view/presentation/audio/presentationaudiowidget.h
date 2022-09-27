/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-14
 * Description : a presentation tool.
 *
 * SPDX-FileCopyrightText: 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * SPDX-FileCopyrightText: 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_PRESENTATION_AUDIO_WIDGET_H
#define DIGIKAM_PRESENTATION_AUDIO_WIDGET_H

// Qt includes

#include <QUrl>
#include <QKeyEvent>

// QtAV includes

#include <AVError.h>
#include <AVPlayerCore.h>

// Local includes

#include "ui_presentationaudiowidget.h"

namespace DigikamGenericPresentationPlugin
{

class PresentationContainer;

class PresentationAudioWidget : public QWidget,
                                public Ui::PresentationAudioWidget
{
    Q_OBJECT

public:

    explicit PresentationAudioWidget(QWidget* const,
                                     const QList<QUrl>&,
                                     PresentationContainer* const);
    ~PresentationAudioWidget() override;

    void enqueue(const QList<QUrl>&);
    bool canHide() const;
    bool isPaused() const;
    void setPaused(bool);
    void keyPressEvent(QKeyEvent*) override;

public Q_SLOTS:

    void slotPlay();
    void slotStop();

Q_SIGNALS:

    void signalPlay();
    void signalPause();

private Q_SLOTS:

    void slotPrev();
    void slotNext();
    void slotTimeUpdaterTimeout();
    void slotError();
    void slotSetVolume(int);
    void slotMediaStateChanged(QtAV::MediaStatus);
    void slotPlayerStateChanged(QtAV::AVPlayerCore::State);
    void slotPlayerError(const QtAV::AVError&);

private:

    void checkSkip();
    void setZeroTime();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericPresentationPlugin

#endif // DIGIKAM_PRESENTATION_AUDIO_WIDGET_H
