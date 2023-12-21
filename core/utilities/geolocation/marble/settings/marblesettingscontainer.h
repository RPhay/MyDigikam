/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings Container.
 *
 * SPDX-FileCopyrightText: 2022-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MARBLE_SETTINGS_CONTAINER_H
#define DIGIKAM_MARBLE_SETTINGS_CONTAINER_H

// Qt includes

#include <QFlags>
#include <QDebug>
#include <QStringList>

// Local includes

#include "digikam_export.h"
#include "metaengine.h"

class KConfigGroup;

namespace Digikam
{

/**
 * The class MarbleSettingsContainer encapsulates all metadata related settings.
 * NOTE: this allows supply changed arguments to MetadataHub without changing the global settings.
 */
class DIGIKAM_EXPORT MarbleSettingsContainer
{
public:

    /**
     * Describes the allowed and desired operation when rotating a picture.
     * The modes are in escalating order and describe if an operation is allowed.
     * What is actually done will be governed by what is possible:
     * 1) RAW files cannot by rotated by content, setting the metadata may be problematic
     * 2) Read-Only files cannot edited, neither content nor metadata
     * 3) Writable files will have lossy compression
     * 4) Only JPEG and PGF offer lossless rotation
     * Using a contents-based rotation always implies resetting the flag.
     */
    enum RotationBehaviorFlag
    {
        NoRotation               = 0,
        RotateByInternalFlag     = 1 << 0,
        RotateByMetadataFlag     = 1 << 1,
        RotateByLosslessRotation = 1 << 2,
        RotateByLossyRotation    = 1 << 3,

        RotatingFlags            = RotateByInternalFlag     | RotateByMetadataFlag,
        RotatingPixels           = RotateByLosslessRotation | RotateByLossyRotation
    };
    Q_DECLARE_FLAGS(RotationBehaviorFlags, RotationBehaviorFlag)

    enum AlbumDateSource
    {
        NewestItemDate = 0,
        OldestItemDate,
        AverageDate,
        FolderDate,
        IgnoreDate
    };

public:

    explicit MarbleSettingsContainer();
    ~MarbleSettingsContainer();

public:

    void readFromConfig(KConfigGroup& group);
    void writeToConfig(KConfigGroup& group)  const;

    QStringList defaultExifToolSearchPaths() const;

public:

    bool                            exifRotate;
    bool                            exifSetOrientation;

    bool                            saveComments;
    bool                            saveDateTime;
    bool                            savePickLabel;
    bool                            saveColorLabel;
    bool                            saveRating;

    bool                            saveTemplate;
    bool                            saveTags;
    bool                            saveFaceTags;
    bool                            savePosition;

    bool                            writeWithExifTool;
    bool                            writeRawFiles;
    bool                            writeDngFiles;
    bool                            updateFileTimeStamp;
    bool                            rescanImageIfModified;
    bool                            useXMPSidecar4Reading;
    bool                            useCompatibleFileName;
    bool                            useLazySync;
    bool                            useFastScan;

    MetaEngine::MetadataWritingMode metadataWritingMode;

    RotationBehaviorFlags           rotationBehavior;

    AlbumDateSource                 albumDateFrom;

    QStringList                     sidecarExtensions;

    QString                         exifToolPath;
};

//! qDebug() stream operator. Writes property @a inf to the debug output in a nicely formatted way.
DIGIKAM_EXPORT QDebug operator<<(QDebug dbg, const MarbleSettingsContainer& inf);

} // namespace Digikam

Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::MarbleSettingsContainer::RotationBehaviorFlags)

#endif // DIGIKAM_MARBLE_SETTINGS_CONTAINER_H
