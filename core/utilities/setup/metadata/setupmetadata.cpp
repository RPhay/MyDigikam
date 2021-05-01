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

    d->readSettings();

    connect(d->exifRotateBox, SIGNAL(toggled(bool)),
            this, SLOT(slotExifAutoRotateToggled(bool)));

    connect(d->clearMetadataIfRescanBox, SIGNAL(toggled(bool)),
            this, SLOT(slotClearMetadataToggled(bool)));

    connect(d->writeRawFilesBox, SIGNAL(toggled(bool)),
            this, SLOT(slotWriteRawFilesToggled(bool)));
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

void SetupMetadata::slotWriteRawFilesToggled(bool b)
{
    // Show info if write metadata to raw files was switched on

    if (b)
    {
        QApplication::beep();

        QPointer<QMessageBox> msgBox1 = new QMessageBox(QMessageBox::Warning,
                 qApp->applicationName(),
                 i18nc("@info",
                       "Do you really want to enable metadata writing to RAW files? "
                       "digiKam delegates this task to the Exiv2 library.\n"
                       "With different RAW formats, problems are known which can "
                       "lead to the destruction of RAW files.\n"
                       "If you decide to do so, make a backup of your RAW files. "
                       "We strongly recommend not to enable this option."),
                 QMessageBox::Yes | QMessageBox::No, this);

        msgBox1->button(QMessageBox::Yes)->setText(i18nc("@action", "Yes I understand"));
        msgBox1->setDefaultButton(QMessageBox::No);

        int result1 = msgBox1->exec();
        delete msgBox1;

        if (result1 == QMessageBox::Yes)
        {
            QPointer<QMessageBox> msgBox2 = new QMessageBox(QMessageBox::Warning,
                     qApp->applicationName(),
                     i18nc("@info", "You would rather disable writing metadata to RAW files?"),
                     QMessageBox::Yes | QMessageBox::No, this);

            int result2 = msgBox2->exec();
            delete msgBox2;

            if (result2 == QMessageBox::No)
            {
                return;
            }
        }

        d->writeRawFilesBox->setChecked(false);
    }
}

} // namespace Digikam
