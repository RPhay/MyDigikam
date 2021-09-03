/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : a tool bar for Showfoto folder view
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef SHOWFOTO_FOLDER_VIEW_TOOL_BAR_H
#define SHOWFOTO_FOLDER_VIEW_TOOL_BAR_H

// Qt includes

#include <QWidget>
#include <QKeyEvent>

// Local includes

#include "slideshowsettings.h"
#include "dlayoutbox.h"

using namespace Digikam;

namespace ShowFoto
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

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_TOOL_BAR_H
