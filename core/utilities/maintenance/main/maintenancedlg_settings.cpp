/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-30
 * Description : maintenance dialog
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "maintenancedlg_p.h"

namespace Digikam
{

MaintenanceSettings MaintenanceDlg::settings() const
{
    MaintenanceSettings prm;
    prm.wholeAlbums                         = d->albumSelectors->wholeAlbumsChecked();
    prm.wholeTags                           = d->albumSelectors->wholeTagsChecked();
    prm.albums                              = d->albumSelectors->selectedAlbums();
    prm.tags                                = d->albumSelectors->selectedTags();
    prm.useMutiCoreCPU                      = d->useMutiCoreCPU->isChecked();
    prm.newItems                            = d->expanderBox->isChecked(Private::NewItems);
    prm.databaseCleanup                     = d->expanderBox->isChecked(Private::DbCleanup);
    prm.cleanThumbDb                        = d->cleanThumbsDb->isChecked();
    prm.cleanFacesDb                        = d->cleanFacesDb->isChecked();
    prm.cleanSimilarityDb                   = d->cleanSimilarityDb->isChecked();
    prm.shrinkDatabases                     = d->shrinkDatabases->isChecked();
    prm.thumbnails                          = d->expanderBox->isChecked(Private::Thumbnails);
    prm.scanThumbs                          = d->scanThumbs->isChecked();
    prm.fingerPrints                        = d->expanderBox->isChecked(Private::FingerPrints);
    prm.scanFingerPrints                    = d->scanFingerPrints->isChecked();
    prm.duplicates                          = d->expanderBox->isChecked(Private::Duplicates);
    prm.minSimilarity                       = d->similarityRange->minValue();
    prm.maxSimilarity                       = d->similarityRange->maxValue();
    prm.duplicatesRestriction               = (HaarIface::DuplicatesSearchRestrictions)
                                                  d->searchResultRestriction->itemData(d->searchResultRestriction->currentIndex()).toInt();
    prm.faceManagement                      = d->expanderBox->isChecked(Private::FaceManagement);
    prm.faceSettings.alreadyScannedHandling = (FaceScanSettings::AlreadyScannedHandling)
                                                  d->faceScannedHandling->itemData(d->faceScannedHandling->currentIndex()).toInt();
    prm.faceSettings.task                   = d->retrainAllFaces->isChecked() ? FaceScanSettings::RetrainAll
                                                                              : FaceScanSettings::DetectAndRecognize;
    prm.faceSettings.albums                 = d->albumSelectors->selectedAlbums();

    prm.autotagsAssignment                  = d->expanderBox->isChecked(Private::AutotagsAssignment);
    prm.autotaggingScanMode                 = d->autotaggingScanMode->itemData(d->autotaggingScanMode->currentIndex()).toInt();
    prm.modelSelectionMode                  = d->modelSelectionMode->itemData(d->modelSelectionMode->currentIndex()).toInt();
    prm.autotagsLanguages                   = d->trSelectorList->languagesList();

    prm.qualitySort                         = d->expanderBox->isChecked(Private::ImageQualitySorter);
    prm.qualityScanMode                     = d->qualityScanMode->itemData(d->qualityScanMode->currentIndex()).toInt();
    prm.qualitySettingsSelected             = (int)d->qualitySelector->settingsSelected();

    if (prm.qualitySettingsSelected == ImageQualityConfSelector::GlobalSettings)
    {
        ImageQualityContainer imgq;
        imgq.readFromConfig();
        prm.quality                         = imgq;
    }
    else
    {
        prm.quality                         = d->qualitySelector->customSettings();
    }

    prm.metadataSync                        = d->expanderBox->isChecked(Private::MetadataSync);
    prm.syncDirection                       = d->syncDirection->itemData(d->syncDirection->currentIndex()).toInt();

    return prm;
}

void MaintenanceDlg::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->expanderBox->readSettings(group);
    d->albumSelectors->loadState();

    d->useLastSettings->blockSignals(true);
    d->useLastSettings->setChecked(group.readEntry(d->configUseLastSettings, false));
    d->useLastSettings->blockSignals(false);

    if (d->useLastSettings->isChecked())
    {
        MaintenanceSettings prm;

        d->useMutiCoreCPU->setChecked(group.readEntry(d->configUseMutiCoreCPU,                                  prm.useMutiCoreCPU));
        d->expanderBox->setChecked(Private::NewItems,           group.readEntry(d->configNewItems,              prm.newItems));

        d->expanderBox->setChecked(Private::DbCleanup,          group.readEntry(d->configCleanupDatabase,       prm.databaseCleanup));
        d->cleanThumbsDb->setChecked(group.readEntry(d->configCleanupThumbDatabase,                             prm.cleanThumbDb));
        d->cleanFacesDb->setChecked(group.readEntry(d->configCleanupFacesDatabase,                              prm.cleanFacesDb));
        d->cleanSimilarityDb->setChecked(group.readEntry(d->configCleanupSimilarityDatabase,                    prm.cleanSimilarityDb));
        d->shrinkDatabases->setChecked(group.readEntry(d->configShrinkDatabases,                                prm.shrinkDatabases));

        d->expanderBox->setChecked(Private::Thumbnails,         group.readEntry(d->configThumbnails,            prm.thumbnails));
        d->scanThumbs->setChecked(group.readEntry(d->configScanThumbs,                                          prm.scanThumbs));

        d->expanderBox->setChecked(Private::FingerPrints,       group.readEntry(d->configFingerPrints,          prm.fingerPrints));
        d->scanFingerPrints->setChecked(group.readEntry(d->configScanFingerPrints,                              prm.scanFingerPrints));

        d->expanderBox->setChecked(Private::Duplicates,         group.readEntry(d->configDuplicates,            prm.duplicates));
        d->similarityRange->setInterval(group.readEntry(d->configMinSimilarity,                                 prm.minSimilarity),
                                        group.readEntry(d->configMaxSimilarity,                                 prm.maxSimilarity));
        int restrictions = d->searchResultRestriction->findData(group.readEntry(d->configDuplicatesRestriction, (int)prm.duplicatesRestriction));
        d->searchResultRestriction->setCurrentIndex(restrictions);

        d->expanderBox->setChecked(Private::FaceManagement,     group.readEntry(d->configFaceManagement,        prm.faceManagement));
        int faceHandling = d->faceScannedHandling->findData(group.readEntry(d->configFaceScannedHandling,       (int)prm.faceSettings.alreadyScannedHandling));
        d->faceScannedHandling->setCurrentIndex(faceHandling);

        d->expanderBox->setChecked(Private::AutotagsAssignment, group.readEntry(d->configAutotagsAssignment,    prm.autotagsAssignment));
        int tagScanMode  = d->autotaggingScanMode->findData(group.readEntry(d->configAutotaggingScanMode,       prm.autotaggingScanMode));
        d->autotaggingScanMode->setCurrentIndex(tagScanMode);
        int tagSelection = d->modelSelectionMode->findData(group.readEntry(d->configModelSelectionMode,         prm.modelSelectionMode));
        d->modelSelectionMode->setCurrentIndex(tagSelection);
        d->trSelectorList->clearLanguages();

        Q_FOREACH (const QString& lg, group.readEntry(d->configAutotagsLanguages,                               prm.autotagsLanguages))
        {
            d->trSelectorList->addLanguage(lg);
        }

        d->expanderBox->setChecked(Private::ImageQualitySorter, group.readEntry(d->configImageQualitySorter,    prm.qualitySort));
        int qualityMod   = d->qualityScanMode->findData(group.readEntry(d->configQualityScanMode,               prm.qualityScanMode));
        d->qualityScanMode->setCurrentIndex(qualityMod);
        d->qualitySelector->setSettingsSelected(
            (ImageQualityConfSelector::SettingsType)group.readEntry(d->configQualitySettingsSelected,           prm.qualitySettingsSelected));

        ImageQualityContainer imq;
        imq.readFromConfig(group);
        d->qualitySelector->setCustomSettings(imq);

        d->expanderBox->setChecked(Private::MetadataSync,       group.readEntry(d->configMetadataSync,          prm.metadataSync));
        int direction    = d->syncDirection->findData(group.readEntry(d->configSyncDirection,                   prm.syncDirection));
        d->syncDirection->setCurrentIndex(direction);
    }

    for (int i = Private::NewItems ; i < Private::Stretch ; ++i)
    {
        slotItemToggled(i, d->expanderBox->isChecked(i));
    }

    winId();
    DXmlGuiWindow::setGoodDefaultWindowSize(windowHandle());
    DXmlGuiWindow::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size());
}

void MaintenanceDlg::writeSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->expanderBox->writeSettings(group);
    d->albumSelectors->saveState();

    group.writeEntry(d->configUseLastSettings, d->useLastSettings->isChecked());

    if (d->useLastSettings->isChecked())
    {
        MaintenanceSettings prm   = settings();

        group.writeEntry(d->configUseMutiCoreCPU,             prm.useMutiCoreCPU);
        group.writeEntry(d->configNewItems,                   prm.newItems);
        group.writeEntry(d->configCleanupDatabase,            prm.databaseCleanup);
        group.writeEntry(d->configCleanupThumbDatabase,       prm.cleanThumbDb);
        group.writeEntry(d->configCleanupFacesDatabase,       prm.cleanFacesDb);
        group.writeEntry(d->configCleanupSimilarityDatabase,  prm.cleanSimilarityDb);
        group.writeEntry(d->configShrinkDatabases,            prm.shrinkDatabases);
        group.writeEntry(d->configThumbnails,                 prm.thumbnails);
        group.writeEntry(d->configScanThumbs,                 prm.scanThumbs);
        group.writeEntry(d->configFingerPrints,               prm.fingerPrints);
        group.writeEntry(d->configScanFingerPrints,           prm.scanFingerPrints);
        group.writeEntry(d->configDuplicates,                 prm.duplicates);
        group.writeEntry(d->configMinSimilarity,              prm.minSimilarity);
        group.writeEntry(d->configMaxSimilarity,              prm.maxSimilarity);
        group.writeEntry(d->configDuplicatesRestriction,      (int)prm.duplicatesRestriction);
        group.writeEntry(d->configFaceManagement,             prm.faceManagement);
        group.writeEntry(d->configFaceScannedHandling,        (int)prm.faceSettings.alreadyScannedHandling);
        group.writeEntry(d->configAutotagsAssignment,         prm.autotagsAssignment);
        group.writeEntry(d->configAutotaggingScanMode,        prm.autotaggingScanMode);
        group.writeEntry(d->configModelSelectionMode,         prm.modelSelectionMode);
        group.writeEntry(d->configAutotagsLanguages,          prm.autotagsLanguages);
        group.writeEntry(d->configImageQualitySorter,         prm.qualitySort);
        group.writeEntry(d->configQualityScanMode,            prm.qualityScanMode);
        group.writeEntry(d->configQualitySettingsSelected,    prm.qualitySettingsSelected);

        ImageQualityContainer imq = d->qualitySelector->customSettings();
        imq.writeToConfig(group);

        group.writeEntry(d->configMetadataSync,               prm.metadataSync);
        group.writeEntry(d->configSyncDirection,              prm.syncDirection);
    }

    DXmlGuiWindow::saveWindowSize(windowHandle(), group);
}

void MaintenanceDlg::slotUseLastSettings(bool checked)
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    group.writeEntry(d->configUseLastSettings, d->useLastSettings->isChecked());

    if (checked)
    {
        readSettings();
    }
    else
    {
        MaintenanceSettings prm;

        d->useMutiCoreCPU->setChecked(prm.useMutiCoreCPU);
        d->expanderBox->setChecked(Private::NewItems,           prm.newItems);

        d->expanderBox->setChecked(Private::DbCleanup,          prm.databaseCleanup);
        d->cleanThumbsDb->setChecked(prm.cleanThumbDb);
        d->cleanFacesDb->setChecked(prm.cleanFacesDb);
        d->cleanSimilarityDb->setChecked(prm.cleanSimilarityDb);
        d->shrinkDatabases->setChecked(prm.shrinkDatabases);

        d->expanderBox->setChecked(Private::Thumbnails,         prm.thumbnails);
        d->scanThumbs->setChecked(prm.scanThumbs);

        d->expanderBox->setChecked(Private::FingerPrints,       prm.fingerPrints);
        d->scanFingerPrints->setChecked(prm.scanFingerPrints);

        d->expanderBox->setChecked(Private::Duplicates,         prm.duplicates);
        d->similarityRange->setInterval(prm.minSimilarity,      prm.maxSimilarity);
        int restrictions = d->searchResultRestriction->findData(prm.duplicatesRestriction);
        d->searchResultRestriction->setCurrentIndex(restrictions);

        d->expanderBox->setChecked(Private::FaceManagement,     prm.faceManagement);
        d->faceScannedHandling->setCurrentIndex(prm.faceSettings.alreadyScannedHandling);

        d->expanderBox->setChecked(Private::AutotagsAssignment, prm.autotagsAssignment);
        d->autotaggingScanMode->setCurrentIndex(prm.autotaggingScanMode);

        d->expanderBox->setChecked(Private::ImageQualitySorter, prm.qualitySort);
        d->qualityScanMode->setCurrentIndex(prm.qualityScanMode);

        d->expanderBox->setChecked(Private::MetadataSync,       prm.metadataSync);
        d->syncDirection->setCurrentIndex(prm.syncDirection);
    }
}

} // namespace Digikam
