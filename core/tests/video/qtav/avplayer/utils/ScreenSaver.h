/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef AV_PLAYER_SCREEN_SAVER_H
#define AV_PLAYER_SCREEN_SAVER_H

// Qt includes

#include <QObject>

namespace AVPlayer
{

// TODO: read QtSystemInfo.ScreenSaver

class ScreenSaver : QObject
{
    Q_OBJECT

public:

    static ScreenSaver& instance();

    ScreenSaver();
    ~ScreenSaver();

    /**
     * When true, just restore the previous settings.
     * Settings changed during the object life will ignored.
     */
    bool enable(bool yes);

public Q_SLOTS:

    void enable();
    void disable();

protected:

    virtual void timerEvent(QTimerEvent*);

private:

    // return false if already called

    bool retrieveState();
    bool restoreState();

    // Disable

    ScreenSaver(QObject*);

private:

    bool    state_saved;
    bool    modified;

#ifdef Q_OS_LINUX

    bool    isX11;
    int     timeout;
    int     interval;
    int     preferBlanking;
    int     allowExposures;

#endif // Q_OS_LINUX

    int     ssTimerId;          ///< for mac
    quint32 osxIOPMAssertionId; ///< for mac OSX >= 10.8
};

} // namespace AVPlayer

#endif // AV_PLAYER_SCREEN_SAVER_H
