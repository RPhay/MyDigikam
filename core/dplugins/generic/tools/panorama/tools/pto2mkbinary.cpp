/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect pto2mk binary program and version
 *
 * SPDX-FileCopyrightText: 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "pto2mkbinary.h"

namespace DigikamGenericPanoramaPlugin
{

Pto2MkBinary::Pto2MkBinary(QObject* const)
    : DBinaryIface(QLatin1String("pto2mk"),
                   QLatin1String("2010.4"),
                   QLatin1String("pto2mk version "),
                   2,
                   QLatin1String("Hugin"),
                   QLatin1String("http://hugin.sourceforge.net/download/"),         // krazy:exclude=insecurenet
                   QLatin1String("Panorama"),
                   QStringList(QLatin1String("-h"))
                  )
{
    setup();
}

Pto2MkBinary::~Pto2MkBinary()
{
}

} // namespace DigikamGenericPanoramaPlugin
