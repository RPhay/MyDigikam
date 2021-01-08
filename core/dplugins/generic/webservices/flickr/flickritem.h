/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-07
 * Description : a tool to export images to Flickr web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
