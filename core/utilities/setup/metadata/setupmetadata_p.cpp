/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-08-03
 * Description : setup Metadata tab.
 *
 * SPDX-FileCopyrightText: 2003-2004 by Ralf Holzer <ralf at well dot com>
 * SPDX-FileCopyrightText: 2003-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupmetadata_p.h"

namespace Digikam
{

SetupMetadata::Private::Private()
  : exifAutoRotateOriginal   (false),
    exifAutoRotateShowedInfo (false),
    sidecarFileNameShowedInfo(false),
    fieldsGroup              (nullptr),
    readWriteGroup           (nullptr),
    rotationGroup            (nullptr),
    rotationAdvGroup         (nullptr),
    saveTagsBox              (nullptr),
    saveCommentsBox          (nullptr),
    saveRatingBox            (nullptr),
    savePickLabelBox         (nullptr),
    saveColorLabelBox        (nullptr),
    saveDateTimeBox          (nullptr),
    saveTemplateBox          (nullptr),
    saveFaceTags             (nullptr),
    savePosition             (nullptr),
    useLazySync              (nullptr),
    writeWithExifToolBox     (nullptr),
    writeDngFilesBox         (nullptr),
    writeRawFilesBox         (nullptr),
    writeWithExifToolLabel   (nullptr),
    writeXMPSidecarBox       (nullptr),
    readXMPSidecarBox        (nullptr),
    sidecarFileNameBox       (nullptr),
    updateFileTimeStampBox   (nullptr),
    rescanImageIfModifiedBox (nullptr),
    writingModeCombo         (nullptr),
    rotateByFlag             (nullptr),
    rotateByContents         (nullptr),
    allowRotateByMetadata    (nullptr),
    allowLossyRotate         (nullptr),
    exifRotateBox            (nullptr),
    exifSetOrientationBox    (nullptr),
    saveToBalooBox           (nullptr),
    readFromBalooBox         (nullptr),
    tab                      (nullptr),
    displaySubTab            (nullptr),
    tagsCfgPanel             (nullptr),
    advTab                   (nullptr),
    exifToolView             (nullptr),
    extensionsEdit           (nullptr)
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

    writeWithExifToolBox->setChecked(set.writeWithExifTool);
    writeDngFilesBox->setChecked(set.writeDngFiles);
    writeRawFilesBox->setChecked(set.writeRawFiles);

    useLazySync->setChecked(set.useLazySync);
    readXMPSidecarBox->setChecked(set.useXMPSidecar4Reading);
    sidecarFileNameBox->setChecked(set.useCompatibleFileName);
    updateFileTimeStampBox->setChecked(set.updateFileTimeStamp);
    rescanImageIfModifiedBox->setChecked(set.rescanImageIfModified);

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
