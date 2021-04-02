/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-01
 * Description : DB Jobs thread for listing and scanning
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

#include "dbjobsthread.h"

// Local includes

#include "coredbaccess.h"
#include "dbjobinfo.h"
#include "dbjob.h"
#include "duplicatesprogressobserver.h"
#include "digikam_debug.h"

namespace Digikam
{

DBJobsThread::DBJobsThread(QObject* const parent)
    : ActionThreadBase(parent)
{
    setObjectName(QLatin1String("DBJobsThread"));
}

DBJobsThread::~DBJobsThread()
{
}

bool DBJobsThread::hasErrors()
{
    return !m_errorsList.isEmpty();
}

QList<QString>& DBJobsThread::errorsList()
{
    return m_errorsList;
}

void DBJobsThread::connectFinishAndErrorSignals(DBJob* const j)
{
    connect(j, SIGNAL(signalDone()),
            this, SIGNAL(finished()));

    connect(j, SIGNAL(error(QString)),
            this, SLOT(error(QString)));
}

void DBJobsThread::error(const QString& errString)
{
    m_errorsList.append(errString);
}

// -------------------------------------------------

AlbumsDBJobsThread::AlbumsDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

AlbumsDBJobsThread::~AlbumsDBJobsThread()
{
}

void AlbumsDBJobsThread::albumsListing(const AlbumsDBJobInfo& info)
{
    AlbumsJob* const j = new AlbumsJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isFoldersJob())
    {
        connect(j, SIGNAL(foldersData(QMap<int,int>)),
                this, SIGNAL(foldersData(QMap<int,int>)));
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

// -------------------------------------------------

TagsDBJobsThread::TagsDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

TagsDBJobsThread::~TagsDBJobsThread()
{
}

void TagsDBJobsThread::tagsListing(const TagsDBJobInfo& info)
{
    TagsJob* const j = new TagsJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isFoldersJob())
    {
        connect(j, SIGNAL(foldersData(QMap<int,int>)),
                this, SIGNAL(foldersData(QMap<int,int>)));
    }
    else if (info.isFaceFoldersJob())
    {
        connect(j, SIGNAL(faceFoldersData(QMap<QString,QMap<int,int> >)),       // krazy:exclude=normalize
                this, SIGNAL(faceFoldersData(QMap<QString,QMap<int,int> >)));   // krazy:exclude=normalize
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

// -------------------------------------------------

DatesDBJobsThread::DatesDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

DatesDBJobsThread::~DatesDBJobsThread()
{
}

void DatesDBJobsThread::datesListing(const DatesDBJobInfo& info)
{
    DatesJob* const j = new DatesJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isFoldersJob())
    {
        connect(j, SIGNAL(foldersData(QHash<QDateTime,int>)),
                this, SIGNAL(foldersData(QHash<QDateTime,int>)));
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

// -------------------------------------------------

GPSDBJobsThread::GPSDBJobsThread(QObject* const parent)
    : DBJobsThread(parent)
{
}

GPSDBJobsThread::~GPSDBJobsThread()
{
}

void GPSDBJobsThread::GPSListing(const GPSDBJobInfo& info)
{
    GPSJob* const j = new GPSJob(info);

    connectFinishAndErrorSignals(j);

    if (info.isDirectQuery())
    {
        connect(j, SIGNAL(directQueryData(QList<QVariant>)),
                this, SIGNAL(directQueryData(QList<QVariant>)));
    }
    else
    {
        connect(j, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));
    }

    ActionJobCollection collection;
    collection.insert(j, 0);

    appendJobs(collection);
}

// -------------------------------------------------

SearchesDBJobsThread::SearchesDBJobsThread(QObject* const parent)
    : DBJobsThread(parent),
      m_isAlbumUpdate(false),
      m_processedImages(0),
      m_totalImages2Scan(0)
{
}

SearchesDBJobsThread::~SearchesDBJobsThread()
{
}

void SearchesDBJobsThread::searchesListing(const SearchesDBJobInfo& info)
{
    ActionJobCollection collection;

    if (info.isDuplicatesJob())
    {
        m_results.clear();
        m_haarIface.reset(new HaarIface(info.imageIds()));
        m_isAlbumUpdate    = info.isAlbumUpdate();
        m_processedImages  = 0;
        m_totalImages2Scan = info.imageIds().count();

        const int threadsCount         = (m_totalImages2Scan < 200) ? 1 : qMax(1, maximumNumberOfThreads());
        const int images2ScanPerThread = m_totalImages2Scan / threadsCount;

        QSet<qlonglong>::const_iterator begin = info.imageIds().cbegin();
        QSet<qlonglong>::const_iterator end;

        for (int i = 0; i < threadsCount; ++i)
        {
            // The last thread should read until the end of the list.

            end = (i == threadsCount - 1) ? info.imageIds().cend() : begin + images2ScanPerThread;

            SearchesJob* const job = new SearchesJob(info, begin, end, m_haarIface.get());

            begin = end;

            connect(job, &SearchesJob::signalDuplicatesResults,
                    this, &SearchesDBJobsThread::slotDuplicatesResults);

            connect(job, &SearchesJob::signalImageProcessed,
                    this, &SearchesDBJobsThread::slotImageProcessed);

            collection.insert(job, 0);
        }
    }
    else
    {
        SearchesJob* const job = new SearchesJob(info);
        connectFinishAndErrorSignals(job);

        connect(job, SIGNAL(data(QList<ItemListerRecord>)),
                this, SIGNAL(data(QList<ItemListerRecord>)));

        collection.insert(job, 0);
    }

    appendJobs(collection);
}

void SearchesDBJobsThread::slotImageProcessed()
{
    emit signalProgress((++m_processedImages * 100) / m_totalImages2Scan);
}

void SearchesDBJobsThread::slotDuplicatesResults(const HaarIface::DuplicatesResultsMap& incoming)
{
    auto searchResults = [&](qlonglong imageId) -> HaarIface::DuplicatesResultsMap::iterator
    {
        for (auto it = m_results.begin(); it != m_results.end(); ++it)
        {
            if ((imageId == it.key()) || (it->second.contains(imageId)))
            {
                return it;
            }
        }
        return m_results.end();
    };

    for (const auto referenceImage : incoming.keys())
    {
        if (searchResults(referenceImage) == m_results.end())
        {
            m_results.insert(referenceImage, incoming.value(referenceImage));
        }
    }

    if (m_processedImages != m_totalImages2Scan)
    {
        return;
    }

    HaarIface::rebuildDuplicatesAlbums(m_results, m_isAlbumUpdate);

    emit finished();
}

} // namespace Digikam
