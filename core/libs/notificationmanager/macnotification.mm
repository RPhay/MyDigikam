/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-07-03
 * Description : Objective-C wrapper to post events on MacOS notifier
 *
 * SPDX-FileCopyrightText: 2014-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Qt include

#include <QString>

// MacOS header

#import <Foundation/Foundation.h>

void MacSendNotificationCenterMessage(NSString* const summary, NSString* const message)
{
    Class notifierClass     = NSClassFromString(@"NSUserNotificationCenter");
    id notificationCenter   = [notifierClass defaultUserNotificationCenter];
    Class notificationClass = NSClassFromString(@"NSUserNotification");
    id notification         = [[notificationClass alloc] init];

    [notification setTitle: summary];
    [notification setSubtitle: message];

    [notificationCenter deliverNotification: notification];
}

bool MacNotificationCenterSupported()
{
    return NSClassFromString(@"NSUserNotificationCenter");
}

bool MacNativeDispatchNotify(const QString& summary, const QString& message)
{
    if (!MacNotificationCenterSupported())
    {
        return false;
    }

    NSString* const sum = [[NSString alloc] initWithUTF8String:summary.toUtf8().constData()];
    NSString* const mes = [[NSString alloc] initWithUTF8String:message.toUtf8().constData()];
    MacSendNotificationCenterMessage(sum, mes);

    return true;
}

