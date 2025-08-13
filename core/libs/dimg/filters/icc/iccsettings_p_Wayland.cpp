/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : wrapper to capture the monitor profile under Linux/Wayland.
 *
 * SPDX-FileCopyrightText: 2005-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "iccsettings_p.h"

namespace Digikam
{

bool IccSettings::Private::profileFromWayland(QScreen* const screen,
                                              int screenNumber,
                                              IccProfile& profile)
{
/*
    // Connecting to D-Bus system.

    QDBusConnection bus = QDBusConnection::systemBus();

    if (!bus.isConnected())
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Cannot open a connexion to D-Bus system.";

        return QByteArray();
    }

    // D-Bus colord interface.

    QDBusInterface colordInterface
    (
        "org.freedesktop.ColorManager",
        "/org/freedesktop/ColorManager",
        "org.freedesktop.ColorManager",
        bus
    );

    if (!colordInterface.isValid())
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Cannot dial with the colord service:" << bus.lastError().message();

        return QByteArray();
    }

    // Get the screens list

    QDBusReply<QList<QDBusObjectPath>> devicesReply = colordInterface.call("GetDevices");

    if (!devicesReply.isValid())
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Error while to get the screens list:" << devicesReply.error().message();

        return QByteArray();
    }

    // Check if the screen number is valid.

    if ((screenNumber < 0) || (screenNumber >= devicesReply.value().size()))
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Invalid screen number:" << screenNumber;

        return QByteArray();
    }

    // Get the device corresponding to the screen number.

    QDBusObjectPath devicePath = devicesReply.value().at(screenNumber);

    QDBusInterface deviceInterface
    (
        "org.freedesktop.ColorManager",
        devicePath.path(),
        "org.freedesktop.ColorManager.Device",
        bus
    );

    if (!deviceInterface.isValid())
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Device interface invalid:" << devicePath.path();
        return QByteArray();
    }

    // Check if the device is a monitor.

    QDBusReply<QString> kindReply = deviceInterface.property("Kind");

    if (!kindReply.isValid() || kindReply.value() != "display")
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "The device is not a screen:" << devicePath.path();
        return QByteArray();
    }

    // Get the default profile.

    QDBusReply<QDBusObjectPath> profilePathReply = deviceInterface.call("GetProfile");

    if (!profilePathReply.isValid())
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Cannot get the profil data for" << devicePath.path();

        return QByteArray();
    }

    QDBusInterface profileInterface
    (
        "org.freedesktop.ColorManager",
        profilePathReply.value().path(),
        "org.freedesktop.ColorManager.Profile",
        bus
    );

    if (!profileInterface.isValid())
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Invalid interface for the profil:" << profilePathReply.value().path();

        return QByteArray();
    }

    // Get the profile data.

    QDBusReply<QByteArray> iccDataReply = profileInterface.call("GetData");

    if (!iccDataReply.isValid())
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Cannot get the profile" << profilePathReply.value().path();

        return QByteArray();
    }

    return iccDataReply.value();
*/
}

} // namespace Digikam
