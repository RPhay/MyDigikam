/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-10-05
 * Description : a tool bar for slideshow
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
 * Copyright (C)      2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_SLIDE_TOOL_BAR_PLUGIN_H
#define DIGIKAM_SLIDE_TOOL_BAR_PLUGIN_H

// Qt includes

#include <QWidget>
#include <QKeyEvent>

// Local includes

#include "digikam_export.h"
#include "slideshowsettings.h"
#include "dlayoutbox.h"

class QAction;

namespace DigikamGenericSlideShowPlugin
{

class SlideToolBar : public DHBox
{
    Q_OBJECT

public:

    explicit SlideToolBar(SlideShowSettings* const settings, QWidget* const parent);
    ~SlideToolBar()                  override;

    bool isPaused() const;
    void pause(bool val);

    void setEnabledPlay(bool val);
    void setEnabledNext(bool val);
    void setEnabledPrev(bool val);

    void closeConfigurationDialog();

protected:

    void keyPressEvent(QKeyEvent* e) override;

Q_SIGNALS:

    void signalNext();
    void signalPrev();
    void signalClose();
    void signalPlay();
    void signalPause();
    void signalUpdateSettings();
    void signalScreenSelected(int);
    void signalRemoveImageFromList();

private Q_SLOTS:

    void slotPlayBtnToggled();
    void slotNexPrevClicked();
    void slotRemoveImage();
    void slotScreenSelected(QAction*);
    void slotMenuSlideShowConfiguration();
    void slotConfigurationAccepted();
    void slotChangeDelayButtonPressed();

private:

    class Private;
    Private* const d;

    friend class SlideShowLoader;
};

} // namespace DigikamGenericSlideShowPlugin

#endif // DIGIKAM_SLIDE_TOOL_BAR_PLUGIN_H
