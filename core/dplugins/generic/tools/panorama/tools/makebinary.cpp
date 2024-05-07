/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect make binary program
 *
 * SPDX-FileCopyrightText: 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "makebinary.h"

namespace DigikamGenericPanoramaPlugin
{

MakeBinary::MakeBinary(QObject* const)
    : DBinaryIface(QLatin1String("make"),
                   QLatin1String("3.80"),
                   QLatin1String("GNU Make "),
                   0,
                   QLatin1String("GNU"),

#ifdef Q_OS_WIN

                   QLatin1String("http://gnuwin32.sourceforge.net/packages/make.htm"),          // krazy:exclude=insecurenet

#else

                   QLatin1String("https://www.gnu.org/software/make/"),

#endif

                   QLatin1String("Panorama"),
                   QStringList(QLatin1String("-v"))
                  )
{
    setup();
}

} // namespace DigikamGenericPanoramaPlugin

#include "moc_makebinary.cpp"
