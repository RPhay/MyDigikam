/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-05-20
 * Description : a tool to export images to Onedrive web service
 *
 * SPDX-FileCopyrightText: 2018 by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_OD_ITEM_H
#define DIGIKAM_OD_ITEM_H

// Qt includes

#include <QString>

namespace DigikamGenericOneDrivePlugin
{

class ODPhoto
{
public:

    ODPhoto() = default;

    QString title;
};

class ODFolder
{
public:

    ODFolder() = default;

    QString title;
};

} // namespace DigikamGenericOneDrivePlugin

#endif // DIGIKAM_OD_ITEM_H
