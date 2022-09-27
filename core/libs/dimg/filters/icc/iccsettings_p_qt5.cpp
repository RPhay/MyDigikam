/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : central place for ICC settings - Qt5 implementations.
 *
 * SPDX-FileCopyrightText: 2005-2006 by F.J. Cruz <fj dot cruz at supercable dot es>
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "iccsettings_p.h"

#if defined(Q_CC_CLANG)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wvariadic-macros"
#endif

// Note must be after all other to prevent broken compilation
#ifdef HAVE_X11
#   include <climits>
#   include <X11/Xlib.h>
#   include <X11/Xatom.h>
#   include <QX11Info>
#endif // HAVE_X11

#if defined(Q_CC_CLANG)
#    pragma clang diagnostic pop
#endif

namespace Digikam
{

/*
 * From koffice/libs/pigment/colorprofiles/KoLcmsColorProfileContainer.cpp
 * SPDX-FileCopyrightText: 2000 Matthias Elter <elter at kde dot org>
 *               2001 John Califf
 *               2004 Boudewijn Rempt <boud at valdyas dot org>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander at kde dot org>
 * SPDX-FileCopyrightText: 2007 Adrian Page <adrian at pagenet dot plus dot com>
*/
IccProfile IccSettings::Private::profileFromWindowSystem(QWidget* const widget)
{

#ifdef HAVE_X11

    if (!QX11Info::isPlatformX11())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Desktop platform is not X11";
        return IccProfile();
    }

    unsigned long appRootWindow;
    QString       atomName;

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

    IccProfile profile;
    {
        QMutexLocker lock(&mutex);

        if (screenProfiles.contains(screenNumber))
        {
            return screenProfiles.value(screenNumber);
        }
    }

    if (screen->virtualSiblings().size() > 1)
    {
        appRootWindow = QX11Info::appRootWindow(QX11Info::appScreen());
        atomName      = QString::fromLatin1("_ICC_PROFILE_%1").arg(screenNumber);
    }
    else
    {
        appRootWindow = QX11Info::appRootWindow(screenNumber);
        atomName      = QLatin1String("_ICC_PROFILE");
    }

    Atom          type;
    int           format;
    unsigned long nitems;
    unsigned long bytes_after;
    quint8*       str = nullptr;

    static Atom icc_atom = XInternAtom(QX11Info::display(), atomName.toLatin1().constData(), True);

    if ((icc_atom != None)                                                &&
        (XGetWindowProperty(QX11Info::display(), appRootWindow, icc_atom,
                           0, INT_MAX, False, XA_CARDINAL,
                           &type, &format, &nitems, &bytes_after,
                           (unsigned char**)& str) == Success)            &&
         nitems
       )
    {
        QByteArray bytes = QByteArray::fromRawData((char*)str, (quint32)nitems);

        if (!bytes.isEmpty())
        {
            profile = IccProfile(bytes);
        }

        qCDebug(DIGIKAM_DIMG_LOG) << "Found X.org XICC monitor profile " << profile.description();
    }
    else
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "No X.org XICC profile installed for screen " << screenNumber;
    }

    // Insert to cache even if null
    {
        QMutexLocker lock(&mutex);
        screenProfiles.insert(screenNumber, profile);
    }

#elif defined Q_OS_WIN

    //TODO
    Q_UNUSED(widget);

#elif defined Q_OS_MACOS

    //TODO
    Q_UNUSED(widget);

#else

    // Unsupported platform
    Q_UNUSED(widget);

#endif

    return IccProfile();
}

} // namespace Digikam
