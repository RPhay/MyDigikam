/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : central place for ICC settings - Qt6 implementations
 *
 * SPDX-FileCopyrightText: 2005-2006 by F.J. Cruz <fj dot cruz at supercable dot es>
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "iccsettings_p.h"

// X11 includes

#if defined(Q_CC_CLANG)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wvariadic-macros"
#endif

// Note must be after all other to prevent broken compilation
#ifdef HAVE_X11
#   include <climits>
#   include <X11/Xlib.h>
#   include <X11/Xatom.h>
#   include <qscreen.h>
#   include <qpa/qplatformnativeinterface.h>
#   include <qpa/qplatformscreen_p.h>
#   include <qpa/qplatformscreen.h>
#endif // HAVE_X11

#if defined(Q_CC_CLANG)
#    pragma clang diagnostic pop
#endif

namespace Digikam
{

#ifdef HAVE_X11

bool IccSettings::Private::isX11()
{
    return QGuiApplication::platformName() == QLatin1String("xcb");
}

QScreen* IccSettings::Private::findScreenForVirtualDesktop(int virtualDesktopNumber)
{
    const auto screens = QGuiApplication::screens();

    for (QScreen* const screen : screens)
    {
        auto* qxcbScreen = dynamic_cast<QNativeInterface::Private::QXcbScreen*>(screen->handle());

        if (qxcbScreen && qxcbScreen->virtualDesktopNumber() == virtualDesktopNumber)
        {
            return screen;
        }
    }

    return nullptr;
}

quint32 IccSettings::Private::getAppRootWindow(int screen)
{
    if (!qApp)
    {
        return 0;
    }

    QPlatformNativeInterface* native = qApp->platformNativeInterface();

    if (!native)
    {
        return 0;
    }

    QScreen* scr = (screen == -1) ?  QGuiApplication::primaryScreen() : findScreenForVirtualDesktop(screen);

    if (!scr)
    {
        return 0;
    }

    return static_cast<uint32_t>(reinterpret_cast<quintptr>(native->nativeResourceForScreen(QByteArrayLiteral("rootwindow"), scr)));
}

int IccSettings::Private::appScreen()
{
    if (!qApp)
    {
        return 0;
    }

    QPlatformNativeInterface* native = qApp->platformNativeInterface();

    if (!native)
    {
        return 0;
    }
    return reinterpret_cast<qintptr>(native->nativeResourceForIntegration(QByteArrayLiteral("x11screen")));
}

Display* IccSettings::Private::display()
{
    if (!qApp)
    {
        return nullptr;
    }

    QPlatformNativeInterface* native = qApp->platformNativeInterface();

    if (!native)
    {
        return nullptr;
    }

    void* display = native->nativeResourceForIntegration(QByteArray("display"));

    return reinterpret_cast<Display*>(display);
}

#endif

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

    if (!isX11())
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
        appRootWindow = getAppRootWindow(appScreen());
        atomName      = QString::fromLatin1("_ICC_PROFILE_%1").arg(screenNumber);
    }
    else
    {
        appRootWindow = getAppRootWindow(screenNumber);
        atomName      = QLatin1String("_ICC_PROFILE");
    }

    Atom          type;
    int           format;
    unsigned long nitems;
    unsigned long bytes_after;
    quint8*       str = nullptr;

    static Atom icc_atom = XInternAtom(display(), atomName.toLatin1().constData(), True);

    if ((icc_atom != None)                                                &&
        (XGetWindowProperty(display(), appRootWindow, icc_atom,
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
