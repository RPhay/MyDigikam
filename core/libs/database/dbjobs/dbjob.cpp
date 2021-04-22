/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-01
 * Description : DB Jobs for listing and scanning
 *
 * Copyright (C) 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
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

#include "dbjob.h"

// Local includes

#include "coredbaccess.h"
#include "dbengineparameters.h"
#include "coredb.h"
#include "facetags.h"
#include "itemlister.h"
#include "digikam_export.h"
#include "digikam_debug.h"
#include "dbjobsthread.h"

namespace Digikam
{

DBJob::DBJob()
    : ActionJob()
{
}

DBJob::~DBJob()
{
}

// ----------------------------------------------

AlbumsJob::AlbumsJob(const AlbumsDBJobInfo& jobInfo)
    : DBJob    (),
      m_jobInfo(jobInfo)
{
}

AlbumsJob::~AlbumsJob()
{
}

void AlbumsJob::run()
{
    if (m_jobInfo.isFoldersJob())
    {
        QMap<int, int> albumNumberMap = CoreDbAccess().db()->getNumberOfImagesInAlbums();

        emit foldersData(albumNumberMap);
    }
    else
    {
        ItemLister lister;
        lister.setRecursive(m_jobInfo.isRecursive());
        lister.setListOnlyAvailable(m_jobInfo.isListAvailableImagesOnly());

        // Send data every 200 images to be more responsive

        ItemListerJobGrowingPartsSendingReceiver receiver(this, 200, 2000, 100);
        lister.listPAlbum(&receiver, m_jobInfo.albumRootId(), m_jobInfo.album());
        receiver.sendData();
    }

    emit signalDone();
}

// ----------------------------------------------

DatesJob::DatesJob(const DatesDBJobInfo& jobInfo)
    : DBJob    (),
      m_jobInfo(jobInfo)
{
}

DatesJob::~DatesJob()
{
}

void DatesJob::run()
{
    if (m_jobInfo.isFoldersJob())
    {
        const QVariantList& values = CoreDbAccess().db()->getAllCreationDates();

        QHash<QDateTime, int> dateNumberHash;
        QHash<QDateTime, int>::iterator it;

        foreach (const QVariant& value, values)
        {
            if (!value.isNull())
            {
                QDateTime dateTime = value.toDateTime();

                if (!dateTime.isValid())
                {
                    continue;
                }

                it = dateNumberHash.find(dateTime);

                if (it == dateNumberHash.end())
                {
                    dateNumberHash.insert(dateTime, 1);
                }
                else
                {
                    it.value()++;
                }
            }
        }

        emit foldersData(dateNumberHash);
    }
    else
    {
        ItemLister lister;
        lister.setListOnlyAvailable(m_jobInfo.isListAvailableImagesOnly());

        // Send data every 200 images to be more responsive

        ItemListerJobPartsSendingReceiver receiver(this, 200);
        lister.listDateRange(&receiver, m_jobInfo.startDate(), m_jobInfo.endDate());

        // Send rest

        receiver.sendData();
    }

    emit signalDone();
}

// ----------------------------------------------

GPSJob::GPSJob(const GPSDBJobInfo& jobInfo)
    : DBJob    (),
      m_jobInfo(jobInfo)
{
}

GPSJob::~GPSJob()
{
}

void GPSJob::run()
{
    if (m_jobInfo.isDirectQuery())
    {
        QList<QVariant> imagesInfoFromArea =
                CoreDbAccess().db()->getImageIdsFromArea(m_jobInfo.lat1(),
                                                         m_jobInfo.lat2(),
                                                         m_jobInfo.lng1(),
                                                         m_jobInfo.lng2(),
                                                         0,
                                                         QLatin1String("rating"));

        emit directQueryData(imagesInfoFromArea);
    }
    else
    {
        ItemLister lister;
        lister.setListOnlyAvailable(m_jobInfo.isListAvailableImagesOnly());

        // Send data every 200 images to be more responsive

        ItemListerJobPartsSendingReceiver receiver(this, 200);
        lister.listAreaRange(&receiver,
                             m_jobInfo.lat1(),
                             m_jobInfo.lat2(),
                             m_jobInfo.lng1(),
                             m_jobInfo.lng2());
        // send rest

        receiver.sendData();
    }

    emit signalDone();
}

// ----------------------------------------------

TagsJob::TagsJob(const TagsDBJobInfo& jobInfo)
    : DBJob    (),
      m_jobInfo(jobInfo)
{
}

TagsJob::~TagsJob()
{
}

void TagsJob::run()
{
    if      (m_jobInfo.isFoldersJob())
    {
        QMap<int, int> tagNumberMap = CoreDbAccess().db()->getNumberOfImagesInTags();

        //qCDebug(DIGIKAM_DBJOB_LOG) << tagNumberMap;

        emit foldersData(tagNumberMap);
    }
    else if (m_jobInfo.isFaceFoldersJob())
    {
        QString property;
        QMap<int, int> counts;
        QMap<QString, QMap<int, int> > facesNumberMap;

        property = ImageTagPropertyName::autodetectedFace();
        counts   = CoreDbAccess().db()->getNumberOfImagesInTagProperties(property);

        if (!counts.contains(FaceTags::unknownPersonTagId()))
        {
            counts[FaceTags::unknownPersonTagId()] = 0;
        }

        facesNumberMap.insert(property, counts);

        property = ImageTagPropertyName::autodetectedPerson();
        counts   = CoreDbAccess().db()->getNumberOfImagesInTagProperties(property);

        facesNumberMap.insert(property, counts);

        property = ImageTagPropertyName::ignoredFace();
        counts   = CoreDbAccess().db()->getNumberOfImagesInTagProperties(property);

        facesNumberMap.insert(property, counts);

        property = ImageTagPropertyName::tagRegion();
        counts   = CoreDbAccess().db()->getNumberOfImagesInTagProperties(property);

        foreach (int tagId, FaceTags::allPersonTags())
        {
            if (!counts.contains(tagId))
            {
                counts[tagId] = 0;
            }
        }

        facesNumberMap.insert(property, counts);

        emit faceFoldersData(facesNumberMap);
    }
    else
    {
        ItemLister lister;
        lister.setRecursive(m_jobInfo.isRecursive());
        lister.setListOnlyAvailable(m_jobInfo.isListAvailableImagesOnly());

        // Send data every 200 images to be more responsive

        ItemListerJobPartsSendingReceiver receiver(this, 200);

        if (!m_jobInfo.specialTag().isNull())
        {
            QString searchXml = lister.tagSearchXml(m_jobInfo.tagsIds().constFirst(),
                                                    m_jobInfo.specialTag(),
                                                    m_jobInfo.isRecursive());
            lister.listImageTagPropertySearch(&receiver, searchXml);
        }
        else
        {
            lister.listTag(&receiver, m_jobInfo.tagsIds());
        }

        // Finish sending

        receiver.sendData();
    }

    emit signalDone();
}

// ----------------------------------------------

SearchesJob::SearchesJob(const SearchesDBJobInfo& jobInfo)
    : DBJob    (),
      m_jobInfo(jobInfo),
      m_iface  (nullptr)
{
}

SearchesJob::SearchesJob(const SearchesDBJobInfo& jobInfo,
                         const QSet<qlonglong>::const_iterator& begin,
                         const QSet<qlonglong>::const_iterator& end,
                         HaarIface* iface)
    : DBJob    (),
      m_jobInfo(jobInfo),
      m_begin  (begin),
      m_end    (end),
      m_iface  (iface)
{
}

SearchesJob::~SearchesJob()
{
}

void SearchesJob::run()
{
    m_jobInfo.isDuplicatesJob() ? runFindDuplicates() : runSearches();
}

void SearchesJob::runSearches()
{
    QList<SearchInfo> infos;

    foreach (int id, m_jobInfo.searchIds())
    {
        infos << CoreDbAccess().db()->getSearchInfo(id);
    }

    ItemLister lister;
    lister.setListOnlyAvailable(m_jobInfo.isListAvailableImagesOnly());

    // Send data every 200 images to be more responsive

    ItemListerJobPartsSendingReceiver receiver(this, 200);

    foreach (const SearchInfo& info, infos)
    {
        if (info.type == DatabaseSearch::HaarSearch)
        {
            lister.listHaarSearch(&receiver, info.query);
        }
        else
        {
            bool ok;
            qlonglong referenceImageId = info.name.toLongLong(&ok);

            if (ok)
            {
                lister.listSearch(&receiver, info.query, 0, referenceImageId);
            }
            else
            {
                lister.listSearch(&receiver, info.query, 0, -1);
            }
        }

        if (!receiver.hasError)
        {
            receiver.sendData();
        }
    }

    emit signalDone();
}

void SearchesJob::runFindDuplicates()
{
    if (m_jobInfo.imageIds().isEmpty())
    {
        qCDebug(DIGIKAM_DBJOB_LOG) << "No image ids passed for duplicates search";
        return;
    }

    DuplicatesProgressObserver observer(this);

    if (!m_iface)
    {
        qCDebug(DIGIKAM_DBJOB_LOG) << "Invalid HaarIface pointer";
        return;
    }

    auto restriction = static_cast<HaarIface::DuplicatesSearchRestrictions>(m_jobInfo.searchResultRestriction());
    auto results     = m_iface->findDuplicates(m_jobInfo.imageIds(),
                                               m_begin,
                                               m_end,
                                               m_jobInfo.minThreshold(),
                                               m_jobInfo.maxThreshold(),
                                               restriction,
                                               &observer);

    emit signalDuplicatesResults(results);
}

bool SearchesJob::isCanceled() const
{
    return m_cancel;
}

} // namespace Digikam
