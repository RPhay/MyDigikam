/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-05-16
 * Description : Autodetects pano_modify binary program and version
 *
 * SPDX-FileCopyrightText: 2013-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "panomodifybinary.h"

namespace DigikamGenericPanoramaPlugin
{

PanoModifyBinary::PanoModifyBinary(QObject* const)
    : DBinaryIface(QLatin1String("pano_modify"),
                   QLatin1String("2012.0"),
                   QLatin1String("pano_modify version "),
                   1,
                   QLatin1String("Hugin"),
                   QLatin1String("http://hugin.sourceforge.net/download/"),     // krazy:exclude=insecurenet
                   QLatin1String("Panorama"),
                   QStringList(QLatin1String("-h"))
                  )
{
    setup();
}

PanoModifyBinary::~PanoModifyBinary()
{
}

} // namespace DigikamGenericPanoramaPlugin
