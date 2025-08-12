/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : central place for ICC settings
 *
 * SPDX-FileCopyrightText: 2000      by Matthias Elter <elter at kde dot org>
 * SPDX-FileCopyrightText: 2001      by John Califf <jcaliff at compuzone dot net>
 * SPDX-FileCopyrightText: 2004      by Boudewijn Rempt <boud at valdyas dot org>
 * SPDX-FileCopyrightText: 2007      by Thomas Zander <zander at kde dot org>
 * SPDX-FileCopyrightText: 2007      by Adrian Page <adrian at pagenet dot plus dot com>
 * SPDX-FileCopyrightText: 2005-2006 by F.J. Cruz <fj dot cruz at supercable dot es>
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2005-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#   if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#       include <private/qtx11extras_p.h>
#   else
#       include <QX11Info>
#   endif
#endif // HAVE_X11

#ifdef Q_OS_WIN
#   include <Windows.h>
#   include <WcsPlugin.h>
#   include <winuser.h>
#   include <ICM.h>
#   include <Wingdi.h>
#endif

#if defined(Q_CC_CLANG)
#    pragma clang diagnostic pop
#endif

namespace Digikam
{

IccProfile IccSettings::Private::profileFromWindowSystem(QWidget* const widget)
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

    /*
     * From koffice/libs/pigment/colorprofiles/KoLcmsColorProfileContainer.cpp
     */

    QString       atomName;
    unsigned long appRootWindow = 0;

    if ((qApp->platformName() == QLatin1String("wayland")) || !QX11Info::isPlatformX11())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Desktop platform is not X11";

        /**
         * @todo Add Wayland support.
         */

        return IccProfile();
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
    unsigned long nitems      = 0;
    unsigned long bytes_after = 0;
    quint8*       str         = nullptr;
    Display* const disp       = QX11Info::display();

    if (disp)
    {
        static Atom icc_atom  = XInternAtom(disp, atomName.toLatin1().constData(), True);

        if (
            (icc_atom != None)                                                      &&
            (XGetWindowProperty(QX11Info::display(), appRootWindow, icc_atom,
                               0, INT_MAX, False, XA_CARDINAL,
                               &type, &format, &nitems, &bytes_after,
                               reinterpret_cast<unsigned char**>(&str)) == Success) &&
             nitems
           )
        {
            QByteArray bytes = QByteArray::fromRawData(reinterpret_cast<char*>(str), (quint32)nitems);

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
    }
    else
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get X.org XICC profile for screen " << screenNumber;
    }

#elif defined Q_OS_WIN

    // Get the handle for the wanted screen device.

    POINT pt;
    pt.x = screenNumber * GetSystemMetrics(SM_CXSCREEN);
    pt.y = 0;
    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);

    if (!GetMonitorInfo(hMonitor, &monitorInfo))
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get the screen information";

        return IccProfile();
    }

    HDC hdcScreen      = CreateDC(NULL, monitorInfo.szDevice, NULL, NULL);

    if (hdcScreen == NULL)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get the screen handle";

        return IccProfile();
    }

    // Get the screen color profile

    DWORD scope      = 0; // WCS_PROFILE_MANAGEMENT_SCOPE_DEFAULT
    DWORD bufferSize = 0;

    // Look at the required buffer size.

    if (!WcsGetDefaultColorProfileSize(scope, NULL, CPT_ICC, CPST_RGB_WORKING_SPACE, 0, &bufferSize))
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get the screen profile size";
        ReleaseDC(NULL, hdcScreen);

        return IccProfile();
    }

    // Buffer alloc.

    std::vector<WCHAR> profilePath(bufferSize);

    if (!WcsGetDefaultColorProfile(scope, NULL, CPT_ICC, CPST_RGB_WORKING_SPACE, 0, profilePath.data(), &bufferSize))
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get the screen profile path";
        ReleaseDC(NULL, hdcScreen);

        return IccProfile();
    }

    // Read the color profile file on disk.

    QFile profileFile(QString::fromWCharArray(profilePath.data()));

    if (!profileFile.open(QIODevice::ReadOnly))
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot open the screen profile file";
        ReleaseDC(NULL, hdcScreen);

        return IccProfile();
    }

    QByteArray profileData = profileFile.readAll();
    profileFile.close();

    if (!profileData.isEmpty())
    {
        profile = IccProfile(profileData);
    }

    qCDebug(DIGIKAM_DIMG_LOG) << "Found Windows monitor profile " << profile.description();

    // Free the memory

    ReleaseDC(NULL, hdcScreen);

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
