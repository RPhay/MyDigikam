/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Search methods.
 *
 * SPDX-FileCopyrightText: 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2002-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2011-2013 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemiconview_p.h"
#include "progressmanager.h"

namespace Digikam
{

void ItemIconView::slotNewKeywordSearch()
{
    slotLeftSideBarActivate(d->searchSideBar);
    d->searchSideBar->newKeywordSearch();
}

void ItemIconView::slotNewAdvancedSearch()
{
    slotLeftSideBarActivate(d->searchSideBar);
    d->searchSideBar->newAdvancedSearch();
}

void ItemIconView::slotNewDuplicatesSearch(const QList<PAlbum*>& albums)
{
    slotLeftSideBarActivate(d->fuzzySearchSideBar);
    d->fuzzySearchSideBar->newDuplicatesSearch(albums);
}

void ItemIconView::slotNewDuplicatesSearch(const QList<TAlbum*>& albums)
{
    slotLeftSideBarActivate(d->fuzzySearchSideBar);
    d->fuzzySearchSideBar->newDuplicatesSearch(albums);
}


void ItemIconView::slotImageFindSimilar()
{
    const ItemInfo current = currentInfo();

    if (!current.isNull())
    {
        d->fuzzySearchSideBar->newSimilarSearch(current);
        slotLeftSideBarActivate(d->fuzzySearchSideBar);
    }
}

void ItemIconView::slotImageScanForFaces()
{

    if (ProgressManager::instance()->findItembyId(i18n("FacesDetector")))
    {
        QString message = i18n("A face recognition task is already running. "
                               "Only one task can be running at a time. "
                               "Please wait until it is finished.");
        d->errorWidget->setMessageType(DNotificationWidget::Information);
        d->errorWidget->setText(message);
        d->errorWidget->animatedShowTemporized(5000);

        return;
    }

    FaceScanSettings settings;

    // TODO Faces engine : set K-nearest config

    settings.accuracy               = ApplicationSettings::instance()->getFaceDetectionAccuracy();
    settings.useYoloV3              = ApplicationSettings::instance()->getFaceDetectionYoloV3();
    settings.task                   = FaceScanSettings::Detect;
    settings.alreadyScannedHandling = FaceScanSettings::Rescan;

    // Remove possible duplicate ItemInfos.

    Q_FOREACH (const ItemInfo& info, selectedInfoList(ApplicationSettings::Tools))
    {
        if (!settings.infos.contains(info))
        {
            settings.infos << info;
        }
    }

    FacesDetector* const tool = new FacesDetector(settings);

    connect(tool, SIGNAL(signalComplete()),
            this, SLOT(slotRefreshImagePreview()));

    tool->start();
}

void ItemIconView::slotImageRecognizeFaces()
{
    if (ProgressManager::instance()->findItembyId(i18n("FacesDetector")))
    {
        QString message = i18n("A face recognition task is already running. "
                               "Only one task can be running at a time. "
                               "Please wait until it is finished.");
        d->errorWidget->setMessageType(DNotificationWidget::Information);
        d->errorWidget->setText(message);
        d->errorWidget->animatedShowTemporized(5000);

        return;
    }

    FaceScanSettings settings;

    // TODO Faces engine : set K-nearest config

    settings.accuracy  = ApplicationSettings::instance()->getFaceDetectionAccuracy();
    settings.useYoloV3 = ApplicationSettings::instance()->getFaceDetectionYoloV3();
    settings.task      = FaceScanSettings::RecognizeMarkedFaces;

    // Remove possible duplicate ItemInfos.

    Q_FOREACH (const ItemInfo& info, selectedInfoList(ApplicationSettings::Tools))
    {
        if (!settings.infos.contains(info))
        {
            settings.infos << info;
        }
    }

    FacesDetector* const tool = new FacesDetector(settings);

    connect(tool, SIGNAL(signalComplete()),
            this, SLOT(slotRefreshImagePreview()));

    tool->start();
}

void ItemIconView::slotImageRemoveAllFaces()
{
    const ItemInfoList& selected = selectedInfoList(ApplicationSettings::Metadata);
    MetadataRemover* const tool  = new MetadataRemover(selected, MetadataRemover::Faces);
    tool->start();
}

} // namespace Digikam
