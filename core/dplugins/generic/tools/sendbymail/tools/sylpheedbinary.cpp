/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : Autodetect sylpheed binary program
 *
 * Copyright (C) 2017-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "sylpheedbinary.h"

// KDE includes

#include <klocalizedstring.h>

namespace DigikamGenericSendByMailPlugin
{

SylpheedBinary::SylpheedBinary(QObject* const)
    : DBinaryIface(
                   QLatin1String("sylpheed"),
                   QLatin1String("Sylpheed"),
                   QLatin1String("https://sylpheed.sraoss.jp/en/"),
                   QLatin1String("SendByMail"),
                   QStringList(QLatin1String("--version")),
                   i18n("GTK based Mail Client.")
                  )
{
    setup();
}

SylpheedBinary::~SylpheedBinary()
{
}

} // namespace DigikamGenericSendByMailPlugin
