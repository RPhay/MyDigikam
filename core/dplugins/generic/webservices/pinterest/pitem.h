/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-05-20
 * Description : a tool to export images to Pinterest web service
 *
 * SPDX-FileCopyrightText: 2018      by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_P_ITEM_H
#define DIGIKAM_P_ITEM_H

// Qt includes

#include <QString>

namespace DigikamGenericPinterestPlugin
{

class PPhoto
{
public:

    PPhoto() = default;

    QString title;
};

class PFolder
{

public:

    PFolder() = default;

    QString title;
};

} // namespace DigikamGenericPinterestPlugin

#endif // DIGIKAM_P_ITEM_H
