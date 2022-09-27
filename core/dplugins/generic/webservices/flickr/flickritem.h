/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a tool to export images to Flickr web service
 *
 * SPDX-FileCopyrightText: 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FLICKR_ITEM_H
#define DIGIKAM_FLICKR_ITEM_H

// Qt includes

#include <QStringList>
#include <QString>

// Local includes

#include "flickrlist.h"

namespace DigikamGenericFlickrPlugin
{

class FPhotoInfo
{

public:

    FPhotoInfo()
      : is_public   (false),
        is_friend   (false),
        is_family   (false),
        size        (0),
        safety_level(FlickrList::SAFE),
        content_type(FlickrList::PHOTO)
    {
    }

    bool                    is_public;
    bool                    is_friend;
    bool                    is_family;

    QString                 title;
    QString                 description;
    qlonglong               size;
    QStringList             tags;

    FlickrList::SafetyLevel safety_level;
    FlickrList::ContentType content_type;
};

// -------------------------------------------------------------

class FPhotoSet
{

public:

    FPhotoSet()
      : id(QLatin1String("-1"))
    {
    }

    QString id;
    QString primary;    ///< "2483"
    QString secret;     ///< "abcdef"
    QString server;
    QString photos;
    QString title;
    QString description;
};

} // namespace DigikamGenericFlickrPlugin

#endif // DIGIKAM_FLICKR_ITEM_H
