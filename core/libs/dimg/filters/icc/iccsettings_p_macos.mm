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

namespace Digikam
{

bool IccSettings::Private::profileFromMacos(QScreen* const screen,
                                            int screenNumber,
                                            IccProfile& profile)
{
    Q_UNUSED(screen);
    Q_UNUSED(screenNumber);
    Q_UNUSED(profile);

    // TODO

    return false;
}

} // namespace Digikam
