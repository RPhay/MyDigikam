/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-08-03
 * Description : setup Metadata tab.
 *
 * Copyright (C) 2003-2004 by Ralf Holzer <ralf at well dot com>
 * Copyright (C) 2003-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
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

#include "setupmetadata_p.h"

namespace Digikam
{

SetupMetadata::Private::Private()
  : exifAutoRotateOriginal  (false),
    exifAutoRotateShowedInfo(false),
    clearMetadataShowedInfo (false),
    fieldsGroup             (nullptr),
    readWriteGroup          (nullptr),
    rotationGroup           (nullptr),
    rotationAdvGroup        (nullptr),
    saveTagsBox             (nullptr),
    saveCommentsBox         (nullptr),
    saveRatingBox           (nullptr),
    savePickLabelBox        (nullptr),
    saveColorLabelBox       (nullptr),
    saveDateTimeBox         (nullptr),
    saveTemplateBox         (nullptr),
    saveFaceTags            (nullptr),
    savePosition            (nullptr),
    useLazySync             (nullptr),
    writeDngFilesBox        (nullptr),
    writeRawFilesBox        (nullptr),
    writeXMPSidecarBox      (nullptr),
    readXMPSidecarBox       (nullptr),
    sidecarFileNameBox      (nullptr),
    updateFileTimeStampBox  (nullptr),
    rescanImageIfModifiedBox(nullptr),
    clearMetadataIfRescanBox(nullptr),
    writingModeCombo        (nullptr),
    rotateByFlag            (nullptr),
    rotateByContents        (nullptr),
    allowRotateByMetadata   (nullptr),
    allowLossyRotate        (nullptr),
    exifRotateBox           (nullptr),
    exifSetOrientationBox   (nullptr),
    saveToBalooBox          (nullptr),
    readFromBalooBox        (nullptr),
    tab                     (nullptr),
    displaySubTab           (nullptr),
    tagsCfgPanel            (nullptr),
    advTab                  (nullptr),
    exifToolView            (nullptr),
    extensionsEdit          (nullptr)
{
}

void SetupMetadata::Private::readSettings()
{
    MetaEngineSettings* const mSettings = MetaEngineSettings::instance();

    if (!mSettings)
    {
        return;
    }

    MetaEngineSettingsContainer set = mSettings->settings();

    if (set.rotationBehavior & MetaEngineSettingsContainer::RotatingPixels)
    {
        rotateByContents->setChecked(true);
    }
    else
    {
        rotateByFlag->setChecked(true);
    }

    allowRotateByMetadata->setChecked(set.rotationBehavior & MetaEngineSettingsContainer::RotateByMetadataFlag);
    allowLossyRotate->setChecked(set.rotationBehavior & MetaEngineSettingsContainer::RotateByLossyRotation);

    exifAutoRotateOriginal       = set.exifRotate;
    exifRotateBox->setChecked(exifAutoRotateOriginal);
    exifSetOrientationBox->setChecked(set.exifSetOrientation);

    saveTagsBox->setChecked(set.saveTags);
    saveCommentsBox->setChecked(set.saveComments);
    saveRatingBox->setChecked(set.saveRating);
    savePickLabelBox->setChecked(set.savePickLabel);
    saveColorLabelBox->setChecked(set.saveColorLabel);
    saveDateTimeBox->setChecked(set.saveDateTime);
    saveTemplateBox->setChecked(set.saveTemplate);
    saveFaceTags->setChecked(set.saveFaceTags);
    savePosition->setChecked(set.savePosition);

    useLazySync->setChecked(set.useLazySync);
    writeDngFilesBox->setChecked(set.writeDngFiles);
    writeRawFilesBox->setChecked(set.writeRawFiles);
    readXMPSidecarBox->setChecked(set.useXMPSidecar4Reading);
    sidecarFileNameBox->setChecked(set.useCompatibleFileName);
    updateFileTimeStampBox->setChecked(set.updateFileTimeStamp);
    rescanImageIfModifiedBox->setChecked(set.rescanImageIfModified);
    clearMetadataIfRescanBox->setChecked(set.clearMetadataIfRescan);

    if (set.metadataWritingMode == MetaEngine::WRITE_TO_FILE_ONLY)
    {
        writeXMPSidecarBox->setChecked(false);
    }
    else
    {
        writeXMPSidecarBox->setChecked(true);
        writingModeCombo->setCurrentIndex(writingModeCombo->findData(set.metadataWritingMode));
    }

    extensionsEdit->setText(set.sidecarExtensions.join(QLatin1Char(' ')));

    exifToolView->setExifToolDirectory(set.exifToolPath);

#ifdef HAVE_KFILEMETADATA

    ApplicationSettings* const aSettings = ApplicationSettings::instance();

    if (!aSettings)
    {
        return;
    }

    saveToBalooBox->setChecked(aSettings->getSyncDigikamToBaloo());
    readFromBalooBox->setChecked(aSettings->getSyncBalooToDigikam());

#endif // HAVE_KFILEMETADATA

}

} // namespace Digikam
