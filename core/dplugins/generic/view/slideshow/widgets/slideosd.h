/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-18
 * Description : slideshow OSD widget
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_SLIDE_OSD_PLUGIN_H
#define DIGIKAM_SLIDE_OSD_PLUGIN_H

// Qt includes

#include <QWidget>
#include <QUrl>

// Local includes

#include "slideshowsettings.h"

class QEvent;

namespace DigikamGenericSlideShowPlugin
{

class SlideShowLoader;
class SlideToolBar;

class SlideOSD : public QWidget
{
    Q_OBJECT

public:

    explicit SlideOSD(SlideShowSettings* const settings, SlideShowLoader* const parent);
    ~SlideOSD()                                 override;

    void setCurrentUrl(const QUrl& url);

    void pause(bool b);
    void video(bool b);
    bool isPaused()                 const;
    bool isUnderMouse()             const;
    void toggleProperties();
    void setLoadingReady(bool b);


    SlideToolBar* toolBar()         const;
    QSize slideShowSize()           const;

private Q_SLOTS:

    void slotUpdateSettings();
    void slotProgressTimer();
    void slotStart();

private:

    bool eventFilter(QObject* obj, QEvent* ev)  override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericSlideShowPlugin

#endif // DIGIKAM_SLIDE_OSD_PLUGIN_H
