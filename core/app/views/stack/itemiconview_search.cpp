/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Search methods.
 *
 * SPDX-FileCopyrightText: 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2002-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
    FaceScanSettings settings;

    // TODO Faces engine : set K-nearest config

    settings.accuracy               = ApplicationSettings::instance()->getFaceDetectionAccuracy();
    settings.useYoloV3              = ApplicationSettings::instance()->getFaceDetectionYoloV3();
    settings.task                   = FaceScanSettings::DetectAndRecognize;
    settings.alreadyScannedHandling = FaceScanSettings::Rescan;
    settings.infos                  = selectedInfoList(ApplicationSettings::Tools);

    FacesDetector* const tool = new FacesDetector(settings);

    connect(tool, SIGNAL(signalComplete()),
            this, SLOT(slotRefreshImagePreview()));

    tool->start();
}

} // namespace Digikam
