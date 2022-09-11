/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-06
 * Description : utilities for digiKam setup
 *
 * SPDX-FileCopyrightText: 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_UTILS_H
#define DIGIKAM_SETUP_UTILS_H

// Qt includes

#include <QStringList>
#include <QString>

namespace Digikam
{

QStringList cleanUserFilterString(QString filterString,
                                  const bool caseSensitive = false,
                                  const bool useSemicolon  = false);

} // namespace Digikam

#endif // DIGIKAM_SETUP_UTILS_H
