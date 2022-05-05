/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include "common_export.h"
#include <QtCore/QObject>

// TODO: read QtSystemInfo.ScreenSaver

class COMMON_EXPORT ScreenSaver : QObject
{
    Q_OBJECT
public:
    static ScreenSaver& instance();
    ScreenSaver();
    ~ScreenSaver();
    // enable: just restore the previous settings. settings changed during the object life will ignored
    bool enable(bool yes);
public slots:
    void enable();
    void disable();
protected:
    virtual void timerEvent(QTimerEvent *);
private:
    //return false if already called
    bool retrieveState();
    bool restoreState();
    bool state_saved, modified;
#ifdef Q_OS_LINUX
    bool isX11;
    int timeout;
    int interval;
    int preferBlanking;
    int allowExposures;
#endif //Q_OS_LINUX
    int ssTimerId; //for mac
    quint32 osxIOPMAssertionId; // for mac OSX >= 10.8
};

#endif // SCREENSAVER_H
