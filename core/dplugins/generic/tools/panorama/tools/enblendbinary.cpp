/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect enblend binary program and version
 *
 * SPDX-FileCopyrightText: 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "enblendbinary.h"

namespace DigikamGenericPanoramaPlugin
{

EnblendBinary::EnblendBinary(QObject* const)
    : DBinaryIface(QLatin1String("enblend"),
                   QLatin1String("4.0"),
                   QLatin1String("enblend "),
                   0,
                   QLatin1String("Hugin"),
                   QLatin1String("https://hugin.sourceforge.net/download/"),
                   QLatin1String("Panorama"),
                   QStringList(QLatin1String("-V")))
{
    setup();
}

} // namespace DigikamGenericPanoramaPlugin

#include "moc_enblendbinary.cpp"
