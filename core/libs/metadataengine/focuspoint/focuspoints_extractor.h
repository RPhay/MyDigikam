/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Extraction of focus points by exiftool data
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#ifndef DIGIKAM_FOCUSPOINTS_EXTRACTOR_H
#define DIGIKAM_FOCUSPOINTS_EXTRACTOR_H

// Qt includes

#include <QObject>
#include <QVariant>
#include <QStringList>

// Local includes

#include "digikam_export.h"
#include "focuspoint.h"

namespace Digikam
{

class DIGIKAM_EXPORT FocusPointsExtractor : public QObject
{
    Q_OBJECT

public:

    /**
     * A list used to store focus points of a image extracted from meta data
     *
     * With extract() function, an exiftool parser is used to read data from
     * metadata and lists all focus points. Each focus point is defined by their
     * relative centers coordinate and relative size. Each point has own
     * type (Inactive, Infocus, Selected, SelectedInFocus)
     */
    using ListAFPoints = QList<FocusPoint>;

public:

    explicit FocusPointsExtractor(QObject* const parent, const QString& path);
    ~FocusPointsExtractor();

public:

    ListAFPoints get_af_points();
    ListAFPoints get_af_points(FocusPoint::TypePoint type);

    bool    isAFPointsReadOnly()                                                                            const;
    QString make()                                                                                          const;
    QString model()                                                                                         const;
    QSize   originalSize()                                                                                  const;

private:

    QVariant findValue(const QString& tagName, bool isList = false)                                         const;
    QVariant findValue(const QString& tagNameRoot, const QString& key, bool isList = false)                 const;
    QVariant findValueFirstMatch(const QStringList& listTagNames, bool isList = false)                      const;
    QVariant findValueFirstMatch(const QString& tagNameRoot, const QStringList& keys, bool isList = false)  const;
    void setAFPointsReadOnly(bool readOnly)                                                                 const;
    void setOriginalSize(const QSize& size)                                                                 const;

    ListAFPoints findAFPoints()                                                                             const;

    ListAFPoints getAFPoints_canon()                                                                        const;
    ListAFPoints getAFPoints_exif()                                                                         const;
    ListAFPoints getAFPoints_nikon()                                                                        const;
    ListAFPoints getAFPoints_panasonic()                                                                    const;
    ListAFPoints getAFPoints_sony()                                                                         const;
    ListAFPoints getAFPoints_xmp()                                                                          const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FOCUSPOINTS_EXTRACTOR_H
