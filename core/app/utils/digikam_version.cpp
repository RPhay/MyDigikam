/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-09-09
 * Description : digiKam release ID header.
 *
 * Copyright (C) 2004-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "digikam_version.h"

// KDE includes.

#include <klocalizedstring.h>

// Local includes.

#include "gitversion.h"
#include "daboutdata.h"

#if defined(Q_CC_GNU)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdate-time"
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdate-time"
#endif

namespace Digikam
{

const int digiKamMakeIntergerVersion(int major, int minor, int patch)
{
    return (((major) << 16) | ((minor) << 8) | (patch));
}

const QString digiKamVersion()
{
    return QLatin1String(digikam_version);
}

const QString additionalInformation()
{
    QString gitVer       = QLatin1String(GITVERSION);
    QString ret          = i18n("IRC: irc.freenode.net - #digikam\n"
                                "Feedback: digikam-user@kde.org\n\n"
                                "Build date: %1 (target: %2)",
                                QLatin1String(__DATE__),
                                QLatin1String(digikam_build_type));

    if (!gitVer.isEmpty() && !gitVer.startsWith(QLatin1String("unknow")) && !gitVer.startsWith(QLatin1String("export")))
    {
        ret = i18n("IRC: irc.freenode.net - #digikam\n"
                   "Feedback: digikam-user@kde.org\n\n"
                   "Build date: %1 (target: %2)\n"
                   "Rev.: %3",
                   QLatin1String(__DATE__),
                   QLatin1String(digikam_build_type),
                   QString::fromLatin1("<a href='http://commits.kde.org/digikam/%1'>%2</a>").arg(gitVer).arg(gitVer));
    }

    return ret;
}

}  // namespace Digikam

// Restore warnings

#if defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif
