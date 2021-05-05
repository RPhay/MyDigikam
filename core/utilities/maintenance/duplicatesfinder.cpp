/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-20
 * Description : Duplicates items finder.
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "duplicatesfinder.h"

// Qt includes

#include <QTimer>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "itemlister.h"
#include "dnotificationwrapper.h"
#include "digikamapp.h"
#include "dbjobsthread.h"
#include "dbjobsmanager.h"
#include "applicationsettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN DuplicatesFinder::Private
{
public:

    explicit Private()
      : minSimilarity(90),
        maxSimilarity(100),
        albumTagRelation(0),
        searchResultRestriction(0),
        isAlbumUpdate(false),
        job(nullptr)
    {
    }

    int                   minSimilarity;
    int                   maxSimilarity;
    int                   albumTagRelation;
    int                   searchResultRestriction;
    bool                  isAlbumUpdate;
    QList<int>            albumsIdList;
    QList<int>            tagsIdList;
    SearchesDBJobsThread* job;
};

DuplicatesFinder::DuplicatesFinder(const AlbumList& albums, const AlbumList& tags, int albumTagRelation,int minSimilarity,
                                   int maxSimilarity, int searchResultRestriction, ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("DuplicatesFinder"), parent),
      d(new Private)
{
    d->minSimilarity            = minSimilarity;
    d->maxSimilarity            = maxSimilarity;
    d->albumTagRelation         = albumTagRelation;
    d->searchResultRestriction  = searchResultRestriction;

    foreach (Album* const a, albums)
    {
        d->albumsIdList << a->id();
    }

    foreach (Album* const a, tags)
    {
        d->tagsIdList << a->id();
    }
}

DuplicatesFinder::~DuplicatesFinder()
{
    delete d;
}

void DuplicatesFinder::slotStart()
{
    MaintenanceTool::slotStart();
    setLabel(i18n("Find duplicates items"));
    setThumbnail(QIcon::fromTheme(QLatin1String("tools-wizard")).pixmap(22));
    ProgressManager::addProgressItem(this);

    double minThresh = d->minSimilarity / 100.0;
    double maxThresh = d->maxSimilarity / 100.0;

    const HaarIface::AlbumTagRelation relation = static_cast<HaarIface::AlbumTagRelation>(d->albumTagRelation);
    QSet<qlonglong> imageIds = HaarIface::imagesFromAlbumsAndTags(d->albumsIdList, d->tagsIdList, relation);
    SearchesDBJobInfo jobInfo(std::move(imageIds), d->isAlbumUpdate);

    jobInfo.setMinThreshold(minThresh);
    jobInfo.setMaxThreshold(maxThresh);
    jobInfo.setSearchResultRestriction(d->searchResultRestriction);

    d->job = DBJobsManager::instance()->startSearchesJobThread(jobInfo);

    connect(d->job, SIGNAL(finished()),
            this, SLOT(slotDone()));

    connect(d->job, SIGNAL(signalProgress(int)),
            this, SLOT(slotDuplicatesProgress(int)));

    connect(this, SIGNAL(progressItemCanceled(ProgressItem*)),
            this, SIGNAL(signalComplete()));
}

void DuplicatesFinder::slotDuplicatesProgress(int percentage)
{
    setProgress(percentage);
}

void DuplicatesFinder::slotDone()
{
    if (d->job && d->job->hasErrors())
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Failed to list url: " << d->job->errorsList().first();

        // Pop-up a message about the error.
        DNotificationWrapper(QString(), d->job->errorsList().first(),
                             DigikamApp::instance(), DigikamApp::instance()->windowTitle());
    }

    // save the min and max similarity in the configuration.
    ApplicationSettings::instance()->setDuplicatesSearchLastMinSimilarity(d->minSimilarity);
    ApplicationSettings::instance()->setDuplicatesSearchLastMaxSimilarity(d->maxSimilarity);
    ApplicationSettings::instance()->setDuplicatesAlbumTagRelation(d->albumTagRelation);
    ApplicationSettings::instance()->setDuplicatesSearchRestrictions(d->searchResultRestriction);

    d->job = nullptr;
    MaintenanceTool::slotDone();
}

void DuplicatesFinder::slotCancel()
{
    if (d->job)
    {
        d->job->cancel();
        d->job = nullptr;
    }

    MaintenanceTool::slotCancel();
}

} // namespace Digikam
