/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : central place for ICC settings
 *
 * SPDX-FileCopyrightText: 2005-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "iccsettings_p.h"

namespace Digikam
{

IccProfile IccSettings::Private::profileFromDesktop(QWidget* const widget)
{
    QScreen* const screen = qApp->primaryScreen();

    if (!screen)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "No screen available for application";

        return IccProfile();
    }

    QScreen* widgetScreen = screen;

    if (widget)
    {
        QWindow* winHandle = widget->windowHandle();

        if (!winHandle)
        {
            if (QWidget* const nativeParent = widget->nativeParentWidget())
            {
                winHandle = nativeParent->windowHandle();
            }
        }

        if (winHandle)
        {
            widgetScreen = winHandle->screen();
        }
    }

    int screenNumber = qMax(qApp->screens().indexOf(widgetScreen), 0);

    // Look in the hash-table cache to see if profile do not exists yet.

    IccProfile profile;
    {
        QMutexLocker lock(&mutex);

        if (screenProfiles.contains(screenNumber))
        {
            return screenProfiles.value(screenNumber);
        }
    }

#ifdef HAVE_X11

    if (!profileFromX11(screen, screenNumber, profile))
    {
        return IccProfile();
    }

#elif defined Q_OS_WIN

    if (!profileFromWindows(screen, screenNumber, profile))
    {
        return IccProfile();
    }

#elif defined Q_OS_MACOS

    // TODO

    return IccProfile();

#else

    // Unsupported platform

    qCWarning(DIGIKAM_DIMG_LOG) << "The Screen color profile platform is not supported!";

    return IccProfile();

#endif

    // Insert the profile to cache even if null.

    {
        QMutexLocker lock(&mutex);
        screenProfiles.insert(screenNumber, profile);
    }

    return profile;
}

} // namespace Digikam
