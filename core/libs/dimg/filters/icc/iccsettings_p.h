/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : central place for ICC settings - Private header.
 *
 * SPDX-FileCopyrightText: 2005-2006 by F.J. Cruz <fj dot cruz at supercable dot es>
 * SPDX-FileCopyrightText: 2005-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "iccsettings.h"
#include "digikam_config.h"

// Qt includes

#include <QApplication>
#include <QScreen>
#include <QPointer>
#include <QWidget>
#include <QWindow>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>

#ifdef HAVE_DBUS
#   include <QDBusConnection>
#   include <QDBusConnectionInterface>
#   include <QDBusInterface>
#endif

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"
#include "iccprofile.h"
#include "icctransform.h"

namespace Digikam
{

class Q_DECL_HIDDEN IccSettings::Private
{
public:

    Private();

    /**
     * Method to scan profile from the disk.
     */
    QList<IccProfile>    scanDirectories(const QStringList& dirs);
    void                 scanDirectory(const QString& path, const QStringList& filter, QList<IccProfile>* const profiles);

    /**
     * @return the desktop profile used to render the @param widget on screen.
     */
    IccProfile           profileFromDesktop(QWidget* const widget);

    bool                 profileFromWayland(QScreen* const screen, int screenNumber, IccProfile& profile);
    bool                 profileFromX11(QScreen* const screen, int screenNumber, IccProfile& profile);
    bool                 profileFromWindows(QScreen* const screen, int screenNumber, IccProfile& profile);

#ifdef Q_OS_DARWIN

    bool                 profileFromMacos(QScreen* const screen, int screenNumber, IccProfile& profile);

#endif

    /**
     * Methods to manage the configuration file.
     */
    ICCSettingsContainer readFromConfig()               const;
    void                 writeToConfig()                const;
    void                 writeManagedViewToConfig()     const;
    void                 writeManagedPreviewsToConfig() const;

public:

    ICCSettingsContainer   settings;            ///< The color profiles configuration container.
    QMutex                 mutex;               ///< Access protection to the @param settings container.

    QList<IccProfile>      profiles;            ///< List of all profiles available from disk.

    QHash<int, IccProfile> screenProfiles;      ///< Hash-table of Color Profile by screen ID populated by @method profileFromWindowSystem().

    const QString          configGroup;         ///< The configuration group used to store the settings on disk.

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

#   ifdef HAVE_X11

private:

    // X11 helper functions for Qt6 support.
    // Imported from https://code.qt.io/cgit/qt/qtx11extras.git/tree/src/x11extras/qx11info_x11.cpp?h=v5.15.2#n92

    // TODO: these functions use private API from Qt6 (QPlatformNativeInterface) and need to be ported later to XCB API.
    //       See this url for details: https://www.x.org/releases/X11R7.6/doc/libxcb/tutorial/index.html#screenofdisplay

    bool     isX11()                            const;
    quint32  getAppRootWindow(int)              const;
    QScreen* findScreenForVirtualDesktop(int)   const;
    int      appScreen()                        const;
    Display* display()                          const;

#   endif

#endif

};

} // namespace Digikam
