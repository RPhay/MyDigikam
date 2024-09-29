/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-20
 * Description : Duplicates items finder.
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

    Private() = default;

    int                          minSimilarity              = 90;
    int                          maxSimilarity              = 100;
    int                          albumTagRelation           = 0;
    int                          searchResultRestriction    = 0;
    HaarIface::RefImageSelMethod refSelMethod               = HaarIface::RefImageSelMethod::OlderOrLarger;
    bool                         isAlbumUpdate              = false;
    QList<int>                   albumsIdList;
    QList<int>                   tagsIdList;
    QList<int>                   referenceAlbumsList;
    SearchesDBJobsThread*        job                        = nullptr;
};

DuplicatesFinder::DuplicatesFinder(const AlbumList& albums,
                                   const AlbumList& tags,
                                   int albumTagRelation,
                                   int minSimilarity,
                                   int maxSimilarity,
                                   int searchResultRestriction,
                                   HaarIface::RefImageSelMethod method,
                                   const AlbumList& referenceImageAlbum,
                                   ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("DuplicatesFinder"), parent),
      d                            (new Private)
{
    d->minSimilarity           = minSimilarity;
    d->maxSimilarity           = maxSimilarity;
    d->albumTagRelation        = albumTagRelation;
    d->searchResultRestriction = searchResultRestriction;
    d->refSelMethod            = method;

    for (Album* const a : std::as_const(albums))
    {
        d->albumsIdList << a->id();
    }

    for (Album* const a : std::as_const(tags))
    {
        d->tagsIdList << a->id();
    }

    for (Album* const a : std::as_const(referenceImageAlbum))
    {
        d->referenceAlbumsList << a->id();
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
    QSet<qlonglong> imageIds                   = HaarIface::imagesFromAlbumsAndTags(d->albumsIdList, d->tagsIdList, relation);
    QSet<qlonglong> referenceImageIds          = HaarIface::imagesFromAlbumsAndTags(d->referenceAlbumsList, {}, HaarIface::AlbumExclusive);

    switch(d->refSelMethod)
    {
        case HaarIface::RefImageSelMethod::ExcludeFolder:
        case HaarIface::RefImageSelMethod::PreferFolder:
        {
            imageIds.unite(referenceImageIds); // All reference images must be also in the search path, otherwise no duplicates are found
            break;
        }

        case HaarIface::RefImageSelMethod::NewerCreationDate:
        case HaarIface::RefImageSelMethod::NewerModificationDate:
        case HaarIface::RefImageSelMethod::OlderOrLarger:
        {
            break;
        }
    }

    // Finding the duplicates

    SearchesDBJobInfo jobInfo(
                              std::move(imageIds),
                              d->isAlbumUpdate,
                              d->refSelMethod,
                              std::move(referenceImageIds)
                             );

    jobInfo.setMinThreshold(minThresh);
    jobInfo.setMaxThreshold(maxThresh);
    jobInfo.setSearchResultRestriction(d->searchResultRestriction);

    d->job = DBJobsManager::instance()->startSearchesJobThread(jobInfo);

    connect(d->job, SIGNAL(finished()),
            this, SLOT(slotDone()));

    connect(d->job, SIGNAL(signalProgress(int,ItemInfo,QImage)),
            this, SLOT(slotDuplicatesProgress(int,ItemInfo,QImage)));

    connect(this, SIGNAL(progressItemCanceled(ProgressItem*)),
            this, SIGNAL(signalComplete()));
}

void DuplicatesFinder::slotDuplicatesProgress(int percentage, const ItemInfo& inf, const QImage& img)
{
    setThumbnail(QIcon(QPixmap::fromImage(img)));

    QString lbl = i18n("Duplicates for: %1\n", inf.name());
    lbl.append(i18n("Path: %1", inf.relativePath()));
    setLabel(lbl);

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
        disconnect(d->job, nullptr, this, nullptr);

        d->job->cancel();
        d->job = nullptr;
    }

    MaintenanceTool::slotCancel();
}

} // namespace Digikam

#include "moc_duplicatesfinder.cpp"
