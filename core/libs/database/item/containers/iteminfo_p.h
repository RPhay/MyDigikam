/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-21
 * Description : Handling access to one item and associated data - private
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_INFO_P_H
#define DIGIKAM_ITEM_INFO_P_H

#include "iteminfo.h"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QHash>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "coredb.h"
#include "coredbaccess.h"
#include "coredbinfocontainers.h"
#include "coredboperationgroup.h"
#include "dimagehistory.h"
#include "collectionmanager.h"
#include "collectionlocation.h"
#include "iteminfodata.h"
#include "iteminfocache.h"
#include "itemlister.h"
#include "itemlisterrecord.h"
#include "iteminfolist.h"
#include "itemcomments.h"
#include "itemcopyright.h"
#include "itemextendedproperties.h"
#include "itemposition.h"
#include "itemscanner.h"
#include "itemtagpair.h"
#include "facetagseditor.h"
#include "tagscache.h"
#include "template.h"
#include "thumbnailinfo.h"
#include "photoinfocontainer.h"
#include "videoinfocontainer.h"

namespace Digikam
{

MetadataInfo::Field DatabaseVideoMetadataFieldsToMetadataInfoField(const DatabaseFields::VideoMetadata videoMetadataField);
MetadataInfo::Field DatabaseImageMetadataFieldsToMetadataInfoField(const DatabaseFields::ImageMetadata imageMetadataField);

#define RETURN_IF_CACHED(x)                            \
                                                       \
    {                                                  \
        ItemInfoReadLocker lock;                       \
                                                       \
        if (m_data->x##Cached)                         \
        {                                              \
            return m_data->x;                          \
        }                                              \
    }

#define RETURN_ASPECTRATIO_IF_IMAGESIZE_CACHED()       \
                                                       \
    {                                                  \
        ItemInfoReadLocker lock;                       \
                                                       \
        if (m_data->imageSizeCached)                   \
        {                                              \
            return (double)m_data->imageSize.width() / \
                           m_data->imageSize.height(); \
        }                                              \
    }

#define STORE_IN_CACHE_AND_RETURN(x, retrieveMethod)   \
                                                       \
    {                                                  \
        ItemInfoWriteLocker lock;                      \
                                                       \
        if (!values.isEmpty())                         \
        {                                              \
            m_data.data()->x##Cached = true;           \
            m_data.data()->x         = retrieveMethod; \
        }                                              \
                                                       \
        return m_data->x;                              \
    }

} // namespace Digikam

#endif // DIGIKAM_ITEM_INFO_P_H
