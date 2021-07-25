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

#include "disjointmetadata_p.h"

namespace Digikam
{

DisjointMetadata::Private::Private()
{
}

DisjointMetadata::Private::Private(const DisjointMetadataDataFields& other)
    : DisjointMetadataDataFields(other)
{
}

DisjointMetadata::Private::Private(const Private& other)
    : DisjointMetadataDataFields(other)
{
}

void DisjointMetadata::Private::makeConnections(DisjointMetadata* const q)
{
    QObject::connect(TagsCache::instance(), SIGNAL(tagDeleted(int)),
                     q, SLOT(slotTagDeleted(int)),
                     Qt::DirectConnection);

    QObject::connect(CoreDbAccess::databaseWatch(), SIGNAL(databaseChanged()),
                     q, SLOT(slotInvalidate()));
}

} // namespace Digikam
