/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Search methods.
 *
 * Copyright (C) 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2011-2013 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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
