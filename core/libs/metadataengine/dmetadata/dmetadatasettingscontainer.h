/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-20
 * Description : metadata Settings Container.
 *
 * Copyright (C) 2015      by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 * Copyright (C) 2015-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DMETADATA_SETTINGS_CONTAINER_H
#define DIGIKAM_DMETADATA_SETTINGS_CONTAINER_H

// Qt includes

#include <QFlags>
#include <QString>
#include <QMap>
#include <QDebug>

// Local includes

#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

/**
 * @brief The NamespaceEntry class provide a simple container
 *        for dmetadata namespaces variables, such
 *        as names, what types of data expects and extra
 *        xml tags
 */
class DIGIKAM_EXPORT NamespaceEntry
{

public:

    enum NsSubspace
    {
        EXIF = 0,
        IPTC = 1,
        XMP  = 2
    };

    enum TagType
    {
        TAG     = 0,
        TAGPATH = 1
    };

    enum SpecialOptions
    {
        NO_OPTS             = 0,
        COMMENT_ALTLANG     = 1,
        COMMENT_ATLLANGLIST = 2,
        COMMENT_XMP         = 3,
        COMMENT_JPEG        = 4,
        TAG_XMPBAG          = 5,
        TAG_XMPSEQ          = 6,
        TAG_ACDSEE          = 7
    };

    enum NamespaceType
    {
        TAGS       = 0,
        RATING     = 1,
        COMMENT    = 2,
     // PICKLABEL  = 3,
        COLORLABEL = 4
    };

public:

    explicit NamespaceEntry()
      : nsType          (TAGS),
        subspace        (XMP),
        isDefault       (true),
        isDisabled      (false),
        index           (-1),
        tagPaths        (TAGPATH),
        specialOpts     (NO_OPTS),
        secondNameOpts  (NO_OPTS)
    {
    }

    NamespaceEntry(const NamespaceEntry& other)
      : nsType          (other.nsType),
        subspace        (other.subspace),
        isDefault       (other.isDefault),
        isDisabled      (other.isDisabled),
        index           (other.index),
        namespaceName   (other.namespaceName),
        alternativeName (other.alternativeName),
        tagPaths        (other.tagPaths),
        separator       (other.separator),
        convertRatio    (QList<int>(other.convertRatio)),
        specialOpts     (other.specialOpts),
        secondNameOpts  (other.secondNameOpts)
    {
    }

    ~NamespaceEntry()
    {
    }

public:

    static QString DM_TAG_CONTAINER();
    static QString DM_RATING_CONTAINER();
    static QString DM_COMMENT_CONTAINER();
    static QString DM_COLORLABEL_CONTAINER();

public:

    NamespaceType  nsType;
    NsSubspace     subspace;
    bool           isDefault;
    bool           isDisabled;
    int            index;

    /**
     * Tag Options
     */
    QString        namespaceName;
    QString        alternativeName;
    TagType        tagPaths;
    QString        separator;

    /**
     * Rating Options
     */
    QList<int>     convertRatio;

    SpecialOptions specialOpts;
    SpecialOptions secondNameOpts;
};

//! qDebug() stream operator. Writes property @a inf to the debug output in a nicely formatted way.
DIGIKAM_EXPORT QDebug operator<<(QDebug dbg, const NamespaceEntry& inf);

/**
 * The class DMetadataSettingsContainer is designed to dynamically add namespaces.
 */
class DIGIKAM_EXPORT DMetadataSettingsContainer
{
public:

    explicit DMetadataSettingsContainer();
    DMetadataSettingsContainer(const DMetadataSettingsContainer& other);
    ~DMetadataSettingsContainer();

    DMetadataSettingsContainer& operator=(const DMetadataSettingsContainer& other);

public:

    void readFromConfig(KConfigGroup& group);
    void writeToConfig(KConfigGroup& group)                                                         const;

    /**
     * @brief defaultValues - default namespaces used by digiKam
     */
    void defaultValues();

    bool unifyReadWrite()                                                                           const;
    void setUnifyReadWrite(bool b);

    void addMapping(const QString& key);

    QList<NamespaceEntry>& getReadMapping(const QString& key)                                       const;

    QList<NamespaceEntry>& getWriteMapping(const QString& key)                                      const;

    QList<QString> mappingKeys()                                                                    const;

private:

    void defaultTagValues();
    void defaultRatingValues();
    void defaultCommentValues();
    void defaultColorLabelValues();
    void readOneGroup(KConfigGroup& group, const QString& name, QList<NamespaceEntry>& container);
    void writeOneGroup(KConfigGroup& group, const QString& name, QList<NamespaceEntry>& container)  const;

private:

    class Private;
    Private* d;
};

//! qDebug() stream operator. Writes property @a inf to the debug output in a nicely formatted way.
DIGIKAM_EXPORT QDebug operator<<(QDebug dbg, const DMetadataSettingsContainer& inf);

} // namespace Digikam

#endif // DIGIKAM_DMETADATA_SETTINGS_CONTAINER_H
