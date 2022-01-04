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

// Qt include

#include <QString>
#include <QList>
#include <QVariant>
#include <QDateTime>
#include <QStringList>
#include <QDataStream>
#include <QRect>
#include <QSize>
#include <QPoint>

// MacOS header

#import <Foundation/Foundation.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_export.h"

/**
 * Conversion helper method taken from qtbase/src/corelib/io/qsettings.cpp
 */
static QStringList splitArgs(const QString& s, int idx)
{
    int l = s.length();
    Q_ASSERT(l > 0);
    Q_ASSERT(s.at(idx) == QLatin1Char('('));
    Q_ASSERT(s.at(l - 1) == QLatin1Char(')'));

    QStringList result;
    QString item;

    for (++idx; idx < l; ++idx) {
        QChar c = s.at(idx);
        if (c == QLatin1Char(')')) {
            Q_ASSERT(idx == l - 1);
            result.append(item);
        } else if (c == QLatin1Char(' ')) {
            result.append(item);
            item.clear();
        } else {
            item.append(c);
        }
    }

    return result;
}

/**
 * Conversion helper method taken from qtbase/src/corelib/io/qsettings.cpp
 */
static QVariant stringToVariant(const QString& s)
{
    if (s.startsWith(QLatin1Char('@'))) {
        if (s.endsWith(QLatin1Char(')'))) {
            if (s.startsWith(QLatin1String("@ByteArray("))) {
                return QVariant(QStringView{s}.mid(11, s.size() - 12).toLatin1());
            } else if (s.startsWith(QLatin1String("@String("))) {
                return QVariant(QStringView{s}.mid(8, s.size() - 9).toString());
            } else if (s.startsWith(QLatin1String("@Variant("))
                       || s.startsWith(QLatin1String("@DateTime("))) {
                QDataStream::Version version;
                int offset;
                if (s.at(1) == QLatin1Char('D')) {
                    version = QDataStream::Qt_5_6;
                    offset = 10;
                } else {
                    version = QDataStream::Qt_4_0;
                    offset = 9;
                }
                QByteArray a = QStringView{s}.mid(offset).toLatin1();
                QDataStream stream(&a, QIODevice::ReadOnly);
                stream.setVersion(version);
                QVariant result;
                stream >> result;
                return result;
            } else if (s.startsWith(QLatin1String("@Rect("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 4)
                    return QVariant(QRect(args[0].toInt(), args[1].toInt(), args[2].toInt(), args[3].toInt()));
            } else if (s.startsWith(QLatin1String("@Size("))) {
                QStringList args = splitArgs(s, 5);
                if (args.size() == 2)
                    return QVariant(QSize(args[0].toInt(), args[1].toInt()));
            } else if (s.startsWith(QLatin1String("@Point("))) {
                QStringList args = splitArgs(s, 6);
                if (args.size() == 2)
                    return QVariant(QPoint(args[0].toInt(), args[1].toInt()));
            } else if (s == QLatin1String("@Invalid()")) {
                return QVariant();
            }

        }
        if (s.startsWith(QLatin1String("@@")))
            return QVariant(s.mid(1));
    }

    return QVariant(s);
}

/**
 * Conversion helper method taken from qtbase/src/corelib/io/qsettings_mac.cpp
 */
static QVariant qtValue(CFPropertyListRef cfvalue)
{
    if (!cfvalue)
        return QVariant();

    CFTypeID typeId = CFGetTypeID(cfvalue);

    /*
        Sorted grossly from most to least frequent type.
    */
    if (typeId == CFStringGetTypeID()) {
        return stringToVariant(QString::fromCFString(static_cast<CFStringRef>(cfvalue)));
    } else if (typeId == CFNumberGetTypeID()) {
        CFNumberRef cfnumber = static_cast<CFNumberRef>(cfvalue);
        if (CFNumberIsFloatType(cfnumber)) {
            double d;
            CFNumberGetValue(cfnumber, kCFNumberDoubleType, &d);
            return d;
        } else {
            int i;
            qint64 ll;

            if (CFNumberGetType(cfnumber) == kCFNumberIntType) {
                CFNumberGetValue(cfnumber, kCFNumberIntType, &i);
                return i;
            }
            CFNumberGetValue(cfnumber, kCFNumberLongLongType, &ll);
            return ll;
        }
    } else if (typeId == CFArrayGetTypeID()) {
        CFArrayRef cfarray = static_cast<CFArrayRef>(cfvalue);
        QList<QVariant> list;
        CFIndex size = CFArrayGetCount(cfarray);
        bool metNonString = false;
        for (CFIndex i = 0; i < size; ++i) {
            QVariant value = qtValue(CFArrayGetValueAtIndex(cfarray, i));
            if (value.type() != QVariant::String)
                metNonString = true;
            list << value;
        }
        if (metNonString)
            return list;
        else
            return QVariant(list).toStringList();
    } else if (typeId == CFBooleanGetTypeID()) {
        return (bool)CFBooleanGetValue(static_cast<CFBooleanRef>(cfvalue));
    } else if (typeId == CFDataGetTypeID()) {
        QByteArray byteArray = QByteArray::fromRawCFData(static_cast<CFDataRef>(cfvalue));

        // Fast-path for QByteArray, so that we don't have to go
        // though the expensive and lossy conversion via UTF-8.
        if (!byteArray.startsWith('@')) {
            byteArray.detach();
            return byteArray;
        }

        const QString str = QString::fromUtf8(byteArray.constData(), byteArray.size());
        QVariant variant = stringToVariant(str);
        if (variant == QVariant(str)) {
            // We did not find an encoded variant in the string,
            // so return the raw byte array instead.
            byteArray.detach();
            return byteArray;
        }

        return variant;
    } else if (typeId == CFDictionaryGetTypeID()) {
        CFDictionaryRef cfdict = static_cast<CFDictionaryRef>(cfvalue);
        CFTypeID arrayTypeId = CFArrayGetTypeID();
        int size = (int)CFDictionaryGetCount(cfdict);
        QVarLengthArray<CFPropertyListRef> keys(size);
        QVarLengthArray<CFPropertyListRef> values(size);
        CFDictionaryGetKeysAndValues(cfdict, keys.data(), values.data());

        QVariantMap map;
        for (int i = 0; i < size; ++i) {
            QString key = QString::fromCFString(static_cast<CFStringRef>(keys[i]));

            if (CFGetTypeID(values[i]) == arrayTypeId) {
                CFArrayRef cfarray = static_cast<CFArrayRef>(values[i]);
                CFIndex arraySize = CFArrayGetCount(cfarray);
                QVariantList list;
                list.reserve(arraySize);
                for (CFIndex j = 0; j < arraySize; ++j)
                    list.append(qtValue(CFArrayGetValueAtIndex(cfarray, j)));
                map.insert(key, list);
            } else {
                map.insert(key, qtValue(values[i]));
            }
        }
        return map;
    } else if (typeId == CFDateGetTypeID()) {
        return QDateTime::fromCFDate(static_cast<CFDateRef>(cfvalue));
    }
    return QVariant();
}


// ---------------------------------------------------------------------------------------

/**
 * Given a filename extension "extension", here's how to find all of the
 * applications known to the MacOS who can open files of that type.
 * Return a list of suitable bundle properties.
 */
DIGIKAM_GUI_EXPORT QList<QVariantList> MacApplicationForFileExtension(const QString& suffix)
{
    QList<QVariantList> appIDs;

    if (suffix.isEmpty())
    {
        return appIDs;
    }

    // Make a UTI from a filename extension.

    CFArrayRef  bundleIDs             = nullptr;
    CFStringRef extensionRef          = suffix.toCFString();
    CFStringRef uniformTypeIdentifier = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension, extensionRef, nullptr);

    if (!UTTypeConformsTo(uniformTypeIdentifier, kUTTypeBundle))
    {
        return appIDs;
    }

    // Get a list of all of the application bundle IDs that know how to handle this.

    bundleIDs = LSCopyAllRoleHandlersForContentType(uniformTypeIdentifier, kLSRolesViewer | kLSRolesEditor);

    if (bundleIDs != nullptr)
    {
        // Find all the available applications with this bundle ID.
        // We can also get the display name and version if necessary.

        const CFIndex count = CFArrayGetCount(bundleIDs);

        for (CFIndex i = 0 ; i < count ; ++i)
        {
            CFStringRef val            = (CFStringRef)(CFArrayGetValueAtIndex(bundleIDs, i));
            CFArrayRef appsForBundleID = LSCopyApplicationURLsForBundleIdentifier(val, nullptr);

            if (appsForBundleID != nullptr)
            {
                // TODO: call CFURLResourceIsReachable() on each item before adding
                // it to the big array of qualified applications.

                QVariantList propers;
                CFIndex size = CFArrayGetCount(appsForBundleID);

                for (CFIndex j = 0 ; j < size ; ++j)
                {
                    QVariant value = qtValue(CFArrayGetValueAtIndex(appsForBundleID, j));
                    propers << value;
                }

                appIDs << propers;
                CFRelease(appsForBundleID);
            }
        }
    }

    // "applications" now has an array of ALL of the possible applications
    // when finished. Given a UI to choose one, you can then call
    // LSOpenFromURLSpec() to open your file with a specific application.

    // TODO: host applications properties to a class with Qt containers and return it.
    // Look how it's done in QtCore implementation:
    // qtbase/src/plugins/platforms/cocoa/qcocoanativeinterface.mm : QCocoaNativeInterface::defaultBackgroundPixmapForQWizard()
    // qtbase/src/corelib/io/qfilesystemengine_unix.cpp            : isPackage()
    // qtbase/src/corelib/global/qlibraryinfo.cpp                  : getRelocatablePrefix()

    // Release the resources.

    CFRelease(uniformTypeIdentifier);
    CFRelease(bundleIDs);

    qCDebug(DIGIKAM_GENERAL_LOG) << "Bundle properties for" << suffix;
    qCDebug(DIGIKAM_GENERAL_LOG) << appIDs;

    return appIDs;
}
