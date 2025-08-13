/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : Objective-C wrapper to capture the monitor profile under MacOS.
 *
 * SPDX-FileCopyrightText: 2005-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "iccsettings_p.h"

// MacOS headers

#import <CoreGraphics/CoreGraphics.h>
#import <ColorSync/ColorSync.h>

namespace Digikam
{

bool IccSettings::Private::profileFromMacos(QScreen* const screen,
                                            int screenNumber,
                                            IccProfile& profile)
{
    Q_UNUSED(screen);

    // Get the number of active screens.

    uint32_t displayCount = 0;
    CGError error         = CGGetActiveDisplayList(0, nullptr, &displayCount);

    if (error != CGDisplayNoErr)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get the number of screens";

        return false;
    }

    if ((screenNumber < 0) || (screenNumber >= displayCount))
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Screen number is out of range";

        return false;
    }

    // Allocate an array to store the screen IDs

    std::vector<CGDirectDisplayID> displayIDs(displayCount);
    error = CGGetActiveDisplayList(displayCount, displayIDs.data(), &displayCount);

    if (error != CGDisplayNoErr)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get the list of screens";

        return false;
    }

    // Screens profile extraction.

    CGDirectDisplayID displayID = displayIDs[screenNumber];
    ColorSyncProfileRef profile = CGDisplayCopyColorSyncProfile(displayID);

    if (!profile)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get the monitor color profile handle" << screenNumber;

        continue;
    }

    // Get the monitor color profile binary data.

    CFDataRef profileData = ColorSyncProfileGetData(profile);

    if (!profileData)
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "Cannot get the monitor color profile binary data" << screenNumber;
        CFRelease(profile);

        return false;
    }

    // Convert CFData to QByteArray

    QByteArray profileBytes
    (
        reinterpret_cast<const char*>(CFDataGetBytePtr(profileData)),
        static_cast<int>(CFDataGetLength(profileData))
    );

    // Populate the profile from the byte-array

    if (!profileBytes.isEmpty())
    {
        profile = IccProfile(profileBytes);
    }

    // Free memory

    CFRelease(profileData);
    CFRelease(profile);

    return true;
}

} // namespace Digikam
