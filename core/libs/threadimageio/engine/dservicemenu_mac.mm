/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-28
 * Description : Objective-C wrapper for open-with operations under MacOS
 *
 * Copyright (C) 2021      by Robert Lindsay <robert dot lindsay at gmail dot com>
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dservicemenu.h"

// Qt include

#include <QString>
#include <QList>
#include <QUrl>
#include <QFileInfo>

// MacOS header

#import <Foundation/Foundation.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_export.h"

namespace Digikam
{

QList<QUrl> DServiceMenu::MacApplicationForFileExtension(const QString& suffix)
{
    // Code inspired from:
    // qtbase/src/plugins/platforms/cocoa/qcocoanativeinterface.mm : QCocoaNativeInterface::defaultBackgroundPixmapForQWizard()
    // qtbase/src/corelib/io/qfilesystemengine_unix.cpp            : isPackage()
    // qtbase/src/corelib/global/qlibraryinfo.cpp                  : getRelocatablePrefix()
    // qtbase/src/corelib/plugin/qlibrary_unix.cpp                 : load_sys()

    QList<QUrl> appUrls;

    if (suffix.isEmpty())
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Suffix is empty";
        return appUrls;
    }

    // Make a Uniform Type Identifier from a filename extension.

    CFArrayRef  bundleIDs             = nullptr;
    CFStringRef extensionRef          = suffix.toCFString();
    CFStringRef uniformTypeIdentifier = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, extensionRef, nullptr);

    if (!uniformTypeIdentifier)
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot get the Uniform Type Identifier for" << suffix;
        return appUrls;
    }

    // Get a list of all of the application bundle IDs that know how to handle this.

    bundleIDs = LSCopyAllRoleHandlersForContentType(uniformTypeIdentifier, kLSRolesViewer | kLSRolesEditor);

    if (bundleIDs)
    {
        // Find all the available applications with this bundle ID.
        // We can also get the display name and version if necessary.

        const CFIndex count = CFArrayGetCount(bundleIDs);

        for (CFIndex i = 0 ; i < count ; ++i)
        {
            CFStringRef val            = (CFStringRef)(CFArrayGetValueAtIndex(bundleIDs, i));
            CFArrayRef appsForBundleID = LSCopyApplicationURLsForBundleIdentifier(val, nullptr);

            if (appsForBundleID)
            {
                // TODO: call CFURLResourceIsReachable() on each item before to use it.

                QList<QUrl> urls;
                CFIndex size = CFArrayGetCount(appsForBundleID);

                for (CFIndex j = 0 ; j < size ; ++j)
                {
                    CFPropertyListRef prop = CFArrayGetValueAtIndex(appsForBundleID, j);

                    if (prop)
                    {
                        CFTypeID typeId = CFGetTypeID(prop);

                        if (typeId == CFURLGetTypeID())
                        {
                            urls << QUrl::fromCFURL(static_cast<CFURLRef>(prop));
                        }
                        else
                        {
                            qCWarning(DIGIKAM_GENERAL_LOG) << "Application Bundle Property type is not CFURL:" << typeId << "(" << QString::fromCFString(CFCopyTypeIDDescription(typeId)) << ")";
                        }
                    }
                    else
                    {
                        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot get url" << j << "for application" << i;
                    }
                }

                appUrls << urls;
                CFRelease(appsForBundleID);
            }
        }
    }
    else
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot get the Application urls list for" << suffix;
    }

    // appUrls is a list of ALL possible applications suitable for suffix.
    // Given a UI to choose one, you can then call

    // Release the resources.

    CFRelease(uniformTypeIdentifier);
    CFRelease(bundleIDs);

    return appUrls;
}

bool DServiceMenu::MacOpenFilesWithApplication(const QList<QUrl>& fileUrls, const QUrl& appUrl)
{
    // Inspired from https://github.com/eep/fugu/blob/master/NSWorkspace(LaunchServices).m

    bool success               = true;
    LSLaunchURLSpec lspec      = { nullptr, nullptr, nullptr, 0, nullptr };
    CFMutableArrayRef arrayref = CFArrayCreateMutable(nullptr, 0, nullptr);

    foreach (const QUrl& fileUrl, fileUrls)
    {
        CFURLRef furl = fileUrl.toCFURL();
        CFArrayAppendValue(arrayref, furl);
    }

    lspec.appURL          = appUrl.toCFURL();
    lspec.itemURLs        = arrayref;
//    lspec.passThruParams  = params;
//    lspec.launchFlags     = flags;
    lspec.asyncRefCon     = nullptr;

    OSStatus status       = LSOpenFromURLSpec(&lspec, nullptr);

    if (status != noErr)
    {
        success = false;
        qCWarning(DIGIKAM_GENERAL_LOG) << "Cannot start Application Bundle url" << appUrl << "for files" << fileUrls;
    }

    if (arrayref)
    {
        CFRelease(arrayref);
    }

    return success;
}

QList<QUrl> DServiceMenu::MacApplicationsForFiles(const QList<QUrl>& files)
{
    if (files.isEmpty())
    {
        return QList<QUrl>();
    }

    QString suffix            = QFileInfo(files.first().toLocalFile()).suffix();
    QList<QUrl> commonAppUrls = DServiceMenu::MacApplicationForFileExtension(suffix);

    foreach (const QUrl& file, files)
    {
        suffix            = QFileInfo(file.toLocalFile()).suffix();
        QList<QUrl> aurls = DServiceMenu::MacApplicationForFileExtension(suffix);

        foreach (const QUrl& url, aurls)
        {
            if (!commonAppUrls.contains(url))
            {
                commonAppUrls.removeAll(url);
            }
        }
    }

    return commonAppUrls;
}

QString DServiceMenu::MacApplicationBundleName(const QUrl& appUrl)
{
    return (appUrl.toLocalFile().section(QLatin1Char('/'), -2, -2));
}

} // namespace Digikam

