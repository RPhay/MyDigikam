/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-08-03
 * Description : setup Metadata tab.
 *
 * Copyright (C) 2003-2004 by Ralf Holzer <ralf at well dot com>
 * Copyright (C) 2003-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

SetupMetadata::SetupMetadata(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->tab                          = new QTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);

    // --------------------------------------------------------

    appendBehaviorTab();

    // --------------------------------------------------------

    appendSidecarsTab();

    // --------------------------------------------------------

    appendRotationTab();

    // --------------------------------------------------------

    appendDisplayTab();

    // --------------------------------------------------------

    d->exifToolView = new ExifToolConfPanel(d->tab);
    d->tab->insertTab(ExifTool, d->exifToolView, i18nc("@title:tab", "ExifTool"));

    // --------------------------------------------------------

    appendBalooTab();

    //--------------Advanced Metadata Configuration --------------

    d->advTab = new AdvancedMetadataTab(this);
    d->tab->insertTab(AdvancedConfig, d->advTab, i18nc("@title:tab advanced metadata config view", "Advanced"));

    // --------------------------------------------------------

    connect(d->sidecarFileNameBox, SIGNAL(toggled(bool)),
            this, SLOT(slotSidecarFileNameToggled(bool)));

    connect(d->exifRotateBox, SIGNAL(toggled(bool)),
            this, SLOT(slotExifAutoRotateToggled(bool)));

    connect(d->clearMetadataIfRescanBox, SIGNAL(toggled(bool)),
            this, SLOT(slotClearMetadataToggled(bool)));

    connect(d->writeWithExifToolBox, SIGNAL(toggled(bool)),
            this, SLOT(slotWriteWithExifToolToggled(bool)));

    connect(d->exifToolView, SIGNAL(signalExifToolSettingsChanged(bool)),
            this, SLOT(slotExifToolSettingsChanged(bool)));

    // --------------------------------------------------------

    d->readSettings();
    slotWriteWithExifToolToggled(d->writeWithExifToolBox->isChecked());

    QTimer::singleShot(0, d->exifToolView, SLOT(slotStartFoundExifTool()));
}

SetupMetadata::~SetupMetadata()
{
    delete d;
}

void SetupMetadata::setActiveMainTab(MetadataTab tab)
{
    d->tab->setCurrentIndex(tab);
}

void SetupMetadata::setActiveSubTab(int tab)
{
    d->displaySubTab->setCurrentIndex(tab);
}

void SetupMetadata::applySettings()
{
    MetaEngineSettings* const mSettings = MetaEngineSettings::instance();

    if (!mSettings)
    {
        return;
    }

    MetaEngineSettingsContainer set;

    set.rotationBehavior = MetaEngineSettingsContainer::RotateByInternalFlag;

    if (d->allowRotateByMetadata->isChecked())
    {
        set.rotationBehavior |= MetaEngineSettingsContainer::RotateByMetadataFlag;
    }

    if (d->rotateByContents->isChecked())
    {
        set.rotationBehavior |= MetaEngineSettingsContainer::RotateByLosslessRotation;

        if (d->allowLossyRotate->isChecked())
        {
            set.rotationBehavior |= MetaEngineSettingsContainer::RotateByLossyRotation;
        }
    }

    set.exifRotate            = d->exifRotateBox->isChecked();
    set.exifSetOrientation    = d->exifSetOrientationBox->isChecked();

    set.saveComments          = d->saveCommentsBox->isChecked();
    set.saveDateTime          = d->saveDateTimeBox->isChecked();
    set.savePickLabel         = d->savePickLabelBox->isChecked();
    set.saveColorLabel        = d->saveColorLabelBox->isChecked();
    set.saveRating            = d->saveRatingBox->isChecked();
    set.saveTags              = d->saveTagsBox->isChecked();
    set.saveTemplate          = d->saveTemplateBox->isChecked();
    set.saveFaceTags          = d->saveFaceTags->isChecked();
    set.savePosition          = d->savePosition->isChecked();

    set.useLazySync           = d->useLazySync->isChecked();
    set.writeWithExifTool     = d->writeWithExifToolBox->isChecked();
    set.writeDngFiles         = d->writeDngFilesBox->isChecked();
    set.writeRawFiles         = d->writeRawFilesBox->isChecked();
    set.useXMPSidecar4Reading = d->readXMPSidecarBox->isChecked();
    set.useCompatibleFileName = d->sidecarFileNameBox->isChecked();

    if (d->writeXMPSidecarBox->isChecked())
    {
        set.metadataWritingMode = (MetaEngine::MetadataWritingMode)
                                  d->writingModeCombo->itemData(d->writingModeCombo->currentIndex()).toInt();
    }
    else
    {
        set.metadataWritingMode = MetaEngine::WRITE_TO_FILE_ONLY;
    }

    set.updateFileTimeStamp   = d->updateFileTimeStampBox->isChecked();
    set.rescanImageIfModified = d->rescanImageIfModifiedBox->isChecked();
    set.clearMetadataIfRescan = d->clearMetadataIfRescanBox->isChecked();

    set.sidecarExtensions     = cleanUserFilterString(d->extensionsEdit->text(), true);
    set.sidecarExtensions.removeAll(QLatin1String("xmp"));
    set.sidecarExtensions.removeDuplicates();

    set.exifToolPath          = d->exifToolView->exifToolDirectory();

    mSettings->setSettings(set);

#ifdef HAVE_KFILEMETADATA

    ApplicationSettings* const aSettings = ApplicationSettings::instance();

    if (!aSettings)
    {
        return;
    }

    aSettings->setSyncDigikamToBaloo(d->saveToBalooBox->isChecked());
    aSettings->setSyncBalooToDigikam(d->readFromBalooBox->isChecked());

    aSettings->saveSettings();

#endif // HAVE_KFILEMETADATA

    d->tagsCfgPanel->applySettings();

    d->advTab->applySettings();
}

bool SetupMetadata::exifAutoRotateHasChanged() const
{
    return (d->exifAutoRotateOriginal != d->exifRotateBox->isChecked());
}

void SetupMetadata::slotSidecarFileNameToggled(bool b)
{
    // Show info if sidcar file name for commercial programs was switched on, and only once.

    if (b && !d->sidecarFileNameShowedInfo)
    {
        d->sidecarFileNameShowedInfo = true;
        QMessageBox::information(this, qApp->applicationName(),
                                 i18nc("@info",
                                       "You should only activate this option if you are exchanging "
                                       "image metadata with programs that use the sidecar format "
                                       "BASENAME.xmp. If you have images with the same base name "
                                       "(e.g. Image.CR2 and Image.JPG) then the sidecar can no "
                                       "longer be uniquely assigned and will be used for both "
                                       "images, which can lead to metadata problems."));
    }
}

void SetupMetadata::slotExifAutoRotateToggled(bool b)
{
    // Show info if rotation was switched off, and only once.

    if (!b && !d->exifAutoRotateShowedInfo && exifAutoRotateHasChanged())
    {
        d->exifAutoRotateShowedInfo = true;
        QMessageBox::information(this, qApp->applicationName(),
                                 i18nc("@info",
                                       "Switching off exif auto rotation will most probably show "
                                       "your images in a wrong orientation, so only change this "
                                       "option if you explicitly require this."));
    }
}

void SetupMetadata::slotClearMetadataToggled(bool b)
{
    // Show info if delete metadata from the database was switched on, and only once.

    if (b && !d->clearMetadataShowedInfo)
    {
        d->clearMetadataShowedInfo = true;
        QMessageBox::information(this, qApp->applicationName(),
                                 i18nc("@info",
                                       "Switching on this option and your metadata has been written to the "
                                       "database only and not to the file or sidecar, you will be able to "
                                       "lose inserted metadata such as tags, keywords, or geographic "
                                       "coordinates."));
    }
}

void SetupMetadata::slotWriteWithExifToolToggled(bool b)
{
    d->writeDngFilesBox->setEnabled(b);
    d->writeRawFilesBox->setEnabled(b);
}

} // namespace Digikam
