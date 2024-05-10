/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-05-20
 * Description : a tool to export images to Box web service
 *
 * SPDX-FileCopyrightText: 2018      by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BOX_ITEM_H
#define DIGIKAM_BOX_ITEM_H

// Qt includes

#include <QString>

namespace DigikamGenericBoxPlugin
{

class BOXPhoto
{
public:

    BOXPhoto() = default;

    QString title;
};

class BOXFolder
{
public:

    BOXFolder() = default;

    QString title;
};

} // namespace DigikamGenericBoxPlugin

#endif // DIGIKAM_BOX_ITEM_H
