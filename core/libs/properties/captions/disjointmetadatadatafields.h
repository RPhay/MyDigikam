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

#ifndef DIGIKAM_DISJOINT_METADATA_DATA_FIELDS_H
#define DIGIKAM_DISJOINT_METADATA_DATA_FIELDS_H

#include "disjointmetadata.h"

// Qt includes

#include <QMap>
#include <QList>
#include <QDateTime>
#include <QStringList>

// Local includes

#include "captionvalues.h"
#include "template.h"

namespace Digikam
{

/**
 * This class was split from DisjointMetadata::Private to allow to use the automatic C++ copy constructor
 * (DisjointMetadata::Private contains a QMutex and is thus non-copyable)
 */
class DisjointMetadataDataFields
{
public:

    DisjointMetadataDataFields()
        : dateTimeChanged   (false),
          titlesChanged     (false),
          commentsChanged   (false),
          pickLabelChanged  (false),
          colorLabelChanged (false),
          ratingChanged     (false),
          templateChanged   (false),
          tagsChanged       (false),
          withoutTags       (false),
          pickLabel         (-1),
          highestPickLabel  (-1),
          colorLabel        (-1),
          highestColorLabel (-1),
          rating            (-1),
          highestRating     (-1),
          count             (0),
          dateTimeStatus    (DisjointMetadata::MetadataInvalid),
          titlesStatus      (DisjointMetadata::MetadataInvalid),
          commentsStatus    (DisjointMetadata::MetadataInvalid),
          pickLabelStatus   (DisjointMetadata::MetadataInvalid),
          colorLabelStatus  (DisjointMetadata::MetadataInvalid),
          ratingStatus      (DisjointMetadata::MetadataInvalid),
          templateStatus    (DisjointMetadata::MetadataInvalid),
          invalid           (false)
    {
    }

    bool                                dateTimeChanged;
    bool                                titlesChanged;
    bool                                commentsChanged;
    bool                                pickLabelChanged;
    bool                                colorLabelChanged;
    bool                                ratingChanged;
    bool                                templateChanged;
    bool                                tagsChanged;
    bool                                withoutTags;

    int                                 pickLabel;
    int                                 highestPickLabel;
    int                                 colorLabel;
    int                                 highestColorLabel;
    int                                 rating;
    int                                 highestRating;
    int                                 count;

    QDateTime                           dateTime;
    QDateTime                           lastDateTime;

    CaptionsMap                         titles;
    CaptionsMap                         comments;

    Template                            metadataTemplate;

    QMap<int, DisjointMetadata::Status> tags;

    QStringList                         tagList;

    DisjointMetadata::Status            dateTimeStatus;
    DisjointMetadata::Status            titlesStatus;
    DisjointMetadata::Status            commentsStatus;
    DisjointMetadata::Status            pickLabelStatus;
    DisjointMetadata::Status            colorLabelStatus;
    DisjointMetadata::Status            ratingStatus;
    DisjointMetadata::Status            templateStatus;

    QList<int>                          tagIds;
    bool                                invalid;
};

} // namespace Digikam

#endif // DIGIKAM_DISJOINT_METADATA_DATA_FIELDS_H
