/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-10-05
 * Description : a presentation tool.
 *
 * Copyright (C) 2008      by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2021      by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_PRESENTATION_CTRL_WIDGET_H
#define DIGIKAM_PRESENTATION_CTRL_WIDGET_H

// Qt includes

#include <QWidget>
#include <QKeyEvent>

// Local includes

#include "ui_presentationctrlwidget.h"
#include "presentationcontainer.h"

namespace DigikamGenericPresentationPlugin
{

class PresentationCtrlWidget : public QWidget,
                               public Ui::PresentationCtrlWidget
{
    Q_OBJECT

public:

    explicit PresentationCtrlWidget(QWidget* const parent,
                                    PresentationContainer* const sharedData);
    ~PresentationCtrlWidget()            override;

    bool canHide()  const;
    bool isPaused() const;
    void setPaused(bool val);

    void setEnabledPlay(bool val);
    void setEnabledNext(bool val);
    void setEnabledPrev(bool val);

Q_SIGNALS:

    void signalNext();
    void signalPrev();
    void signalClose();
    void signalPlay();
    void signalPause();

protected:

    void keyPressEvent(QKeyEvent* event) override;

private Q_SLOTS:

    void slotPlayButtonToggled();
    void slotNexPrevClicked();
    void slotChangeDelayButtonPressed();

private:

    bool                    m_canHide;
    PresentationContainer*  m_sharedData;

    friend class PresentationWidget;
    friend class PresentationGL;
};

} // namespace DigikamGenericPresentationPlugin

#endif // DIGIKAM_PRESENTATION_CTRL_WIDGET_H
