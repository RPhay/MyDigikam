/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-08-17
 * Description : Helper class for Image Description Editor Tab
 *
 * Copyright (C) 2015 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
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

#ifndef DIGIKAM_DISJOINT_METADATA_P_H
#define DIGIKAM_DISJOINT_METADATA_P_H

#include "disjointmetadata.h"

// Qt includes

#include <QMutexLocker>
#include <QtAlgorithms>

// Local includes

#include "templatemanager.h"
#include "tagscache.h"
#include "coredbaccess.h"
#include "itemcomments.h"
#include "iteminfo.h"
#include "coredbwatch.h"
#include "metadatahub.h"

namespace Digikam
{

class Q_DECL_HIDDEN DisjointMetadata::Private : public DisjointMetadataDataFields
{
public:

    explicit Private();

    // use the automatic copy constructor

    explicit Private(const DisjointMetadataDataFields& other);
    explicit Private(const Private& other);

public:

    void makeConnections(DisjointMetadata* const q);

    template <class T>
    void loadSingleValue(const T& data,
                         T& storage,
                         DisjointMetadataDataFields::Status& status)
    {
        switch (status)
        {
            case DisjointMetadataDataFields::MetadataInvalid:
            {
                storage = data;
                status  = DisjointMetadataDataFields::MetadataAvailable;
                break;
            }

            case DisjointMetadataDataFields::MetadataAvailable:
            {
                // we have two values. If they are equal, status is unchanged

                if (data == storage)
                {
                    break;
                }

                // they are not equal. We need to enter the disjoint state.

                status = DisjointMetadataDataFields::MetadataDisjoint;
                break;
            }

            case DisjointMetadataDataFields::MetadataDisjoint:
            {
                break;
            }
        }
    };

public:

    QMutex mutex;
};

} // namespace Digikam

#endif // DIGIKAM_DISJOINT_METADATA_P_H
