/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : central place for ICC settings - Private header.
 *
 * SPDX-FileCopyrightText: 2005-2006 by F.J. Cruz <fj dot cruz at supercable dot es>
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ICC_SETTINGS_P_H
#define DIGIKAM_ICC_SETTINGS_P_H

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

    explicit Private();

    QList<IccProfile>    scanDirectories(const QStringList& dirs);
    void                 scanDirectory(const QString& path, const QStringList& filter, QList<IccProfile>* const profiles);

    IccProfile           profileFromWindowSystem(QWidget* const widget);

    ICCSettingsContainer readFromConfig()               const;
    void                 writeToConfig()                const;
    void                 writeManagedViewToConfig()     const;
    void                 writeManagedPreviewsToConfig() const;

public:

    ICCSettingsContainer   settings;
    QMutex                 mutex;

    QList<IccProfile>      profiles;

    QHash<int, IccProfile> screenProfiles;

    const QString          configGroup;

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))

#   ifdef HAVE_X11

private:

    // X11 helper functions for Qt6 support.
    // Imported from https://code.qt.io/cgit/qt/qtx11extras.git/tree/src/x11extras/qx11info_x11.cpp?h=5.15.2#n102

    bool     isX11();
    quint32  getAppRootWindow(int);
    QScreen* findScreenForVirtualDesktop(int);
    int      appScreen();
    Display* display();

#   endif

#endif

};

} // namespace Digikam

#endif // DIGIKAM_ICC_SETTINGS_P_H
