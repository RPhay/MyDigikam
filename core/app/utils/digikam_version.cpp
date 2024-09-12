/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-09-09
 * Description : digiKam release ID header.
 *
 * SPDX-FileCopyrightText: 2004-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "digikam_version.h"

// Qt includes

#include <QDate>
#include <QTime>
#include <QLocale>
#include <QSysInfo>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_gitversion.h"
#include "digikam_builddate.h"
#include "digikam_globals.h"
#include "daboutdata.h"

namespace Digikam
{

int digiKamMakeIntegerVersion(int major, int minor, int patch)
{
    return (((major) << 16) | ((minor) << 8) | (patch));
}

const QString digiKamVersion()
{
    return QLatin1String(digikam_version);
}

const QDateTime digiKamBuildDate()
{
    QDateTime dt = QDateTime::fromString(QLatin1String(BUILD_DATE), QLatin1String("yyyyMMddTHHmmss"));

    return asDateTimeUTC(dt);
}

const QString additionalInformation()
{
    QString gitVer       = QLatin1String(GITVERSION);
    QString gitBra       = QLatin1String(GITBRANCH);
    QString ret          = i18n("IRC: %1 - #digikam\n"
                                "Feedback: %2\n"
                                "Documentation: %3\n\n"
                                "Build date: %4 (target: %5)",
                                QString::fromLatin1("<a href='https://libera.chat/'>irc.libera.chat</a>"),
                                QString::fromLatin1("<a href='https://mail.kde.org/mailman/listinfo/digikam-users'>digikam-user@kde.org</a>"),
                                QString::fromLatin1("<a href='https://docs.digikam.org/en/index.html'>docs.digikam.org</a>"),
                                QLocale().toString(digiKamBuildDate(), QLocale::ShortFormat),
                                QLatin1String(digikam_build_type));

    if (
        !gitVer.isEmpty()                           &&
        !gitBra.isEmpty()                           &&
        !gitVer.startsWith(QLatin1String("unknow")) &&
        !gitVer.startsWith(QLatin1String("export")) &&
        !gitBra.startsWith(QLatin1String("unknow"))
       )
    {
        ret = i18n(
                   "IRC: %1 - #digikam\n"
                   "Feedback: %2\n"
                   "Documentation: %3\n\n"
                   "Build date: %4 (target: %5)\n"
                   "Build architecture: %6\n"
                   "Revision: %7\n"
                   "Branch: %8",
                   QString::fromLatin1("<a href='https://libera.chat/'>irc.libera.chat</a>"),
                   QString::fromLatin1("<a href='https://mail.kde.org/mailman/listinfo/digikam-users'>digikam-user@kde.org</a>"),
                   QString::fromLatin1("<a href='https://docs.digikam.org/en/index.html'>docs.digikam.org</a>"),
                   QLocale().toString(digiKamBuildDate(), QLocale::ShortFormat),
                   QLatin1String(digikam_build_type),
                   QSysInfo::buildCpuArchitecture(),
                   QString::fromLatin1("<a href='https://invent.kde.org/graphics/digikam/commit/%1'>%2</a>").arg(gitVer).arg(gitVer),
                   gitBra
                  );
    }

    return ret;
}

} // namespace Digikam
