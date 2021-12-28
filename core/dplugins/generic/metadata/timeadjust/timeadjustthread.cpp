/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : time adjust thread.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2018-2021 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "timeadjustthread.h"

// Qt includes

#include <QFileInfo>

// Local includes

#include "dmetadata.h"
#include "dinfointerface.h"
#include "timeadjusttask.h"

namespace DigikamGenericTimeAdjustPlugin
{

class Q_DECL_HIDDEN TimeAdjustThread::Private
{

public:

    explicit Private()
      : iface(nullptr)
    {
    }

    // Settings from GUI.

    TimeAdjustContainer settings;

    DInfoInterface*     iface;
};


TimeAdjustThread::TimeAdjustThread(QObject* const parent, DInfoInterface* const iface)
    : ActionThreadBase(parent),
      d(new Private)
{
    d->iface = iface;
}

TimeAdjustThread::~TimeAdjustThread()
{
    // cancel the thread

    cancel();

    // wait for the thread to finish

    wait();

    delete d;
}

void TimeAdjustThread::setUpdatedDates(const QMap<QUrl, int>& itemsMap)
{
    ActionJobCollection collection;

    foreach (const QUrl& url, itemsMap.keys())
    {
        TimeAdjustTask* const t = new TimeAdjustTask(url, this);
        t->setSettings(d->settings);
        t->setItemsMap(itemsMap);

        connect(t, SIGNAL(signalProcessStarted(QUrl)),
                this, SIGNAL(signalProcessStarted(QUrl)));

        connect(t, SIGNAL(signalProcessEnded(QUrl,QDateTime,QDateTime,int)),
                this, SIGNAL(signalProcessEnded(QUrl,QDateTime,QDateTime,int)));

        connect(t, SIGNAL(signalDateTimeForUrl(QUrl,QDateTime,bool)),
                this, SIGNAL(signalDateTimeForUrl(QUrl,QDateTime,bool)));

        collection.insert(t, 0);
     }

    appendJobs(collection);
}

void TimeAdjustThread::setPreviewDates(const QMap<QUrl, int>& itemsMap)
{
    ActionJobCollection collection;

    foreach (const QUrl& url, itemsMap.keys())
    {
        TimePreviewTask* const t = new TimePreviewTask(url, this);
        t->setSettings(d->settings);
        t->setItemsList(itemsMap);

        connect(t, SIGNAL(signalPreviewReady(QUrl,QDateTime,QDateTime)),
                this, SIGNAL(signalPreviewReady(QUrl,QDateTime,QDateTime)));

        collection.insert(t, 0);
     }

    appendJobs(collection);
}

void TimeAdjustThread::setSettings(const TimeAdjustContainer& settings)
{
    d->settings = settings;
}

QDateTime TimeAdjustThread::readTimestamp(const QUrl& url) const
{
    QDateTime dateTime;

    switch (d->settings.dateSource)
    {
        case TimeAdjustContainer::APPDATE:
        {
            dateTime = readApplicationTimestamp(url);
            break;
        }

        case TimeAdjustContainer::FILENAME:
        {
            dateTime = readFileNameTimestamp(url);
            break;
        }

        case TimeAdjustContainer::FILEDATE:
        {
            dateTime = readFileTimestamp(url);
            break;
        }

        case TimeAdjustContainer::METADATADATE:
        {
            dateTime = readMetadataTimestamp(url);
            break;
        }

        default:  // CUSTOMDATE
        {
            dateTime = QDateTime(d->settings.customDate.date(),
                                 d->settings.customTime.time());

            break;
        }
    }

    return dateTime;
}

QDateTime TimeAdjustThread::readApplicationTimestamp(const QUrl& url) const
{
    DItemInfo info(d->iface->itemInfo(url));

    if (info.dateTime().isValid())
    {
        return info.dateTime();
    }

    return QDateTime();
}

QDateTime TimeAdjustThread::readFileNameTimestamp(const QUrl& url) const
{
    return d->settings.getDateTimeFromUrl(url);
}

QDateTime TimeAdjustThread::readFileTimestamp(const QUrl& url) const
{
    QFileInfo fileInfo(url.toLocalFile());

    return fileInfo.lastModified();
}

QDateTime TimeAdjustThread::readMetadataTimestamp(const QUrl& url) const
{
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (!meta->load(url.toLocalFile()))
    {
        return QDateTime();
    }

    QDateTime dateTime;
    QString exifDateTimeFormat = QLatin1String("yyyy:MM:dd hh:mm:ss");
    QString xmpDateTimeFormat  = QLatin1String("yyyy-MM-ddThh:mm:ss");

    switch (d->settings.metadataSource)
    {
        case TimeAdjustContainer::EXIFIPTCXMP:
        {
            dateTime = meta->getItemDateTime();
            break;
        }

        case TimeAdjustContainer::EXIFCREATED:
        {
            dateTime = QDateTime::fromString(meta->getExifTagString("Exif.Image.DateTime"),
                                             exifDateTimeFormat);
            break;
        }

        case TimeAdjustContainer::EXIFORIGINAL:
        {
            dateTime = QDateTime::fromString(meta->getExifTagString("Exif.Photo.DateTimeOriginal"),
                                             exifDateTimeFormat);
            break;
        }

        case TimeAdjustContainer::EXIFDIGITIZED:
        {
            dateTime = QDateTime::fromString(meta->getExifTagString("Exif.Photo.DateTimeDigitized"),
                                             exifDateTimeFormat);
            break;
        }

        case TimeAdjustContainer::IPTCCREATED:
        {
            // we have to truncate the timezone from the time, otherwise it cannot be converted to a QTime

            dateTime = QDateTime(QDate::fromString(meta->getIptcTagString("Iptc.Application2.DateCreated"),
                                                   Qt::ISODate),
                                 QTime::fromString(meta->getIptcTagString("Iptc.Application2.TimeCreated").left(8),
                                                   Qt::ISODate));
            break;
        }

        case TimeAdjustContainer::XMPCREATED:
        {
            dateTime = QDateTime::fromString(meta->getXmpTagString("Xmp.xmp.CreateDate"),
                                             xmpDateTimeFormat);
            break;
        }

        default:
        {
            // dateTime stays invalid

            break;
        }
    };

    return dateTime;
}

} // namespace DigikamGenericTimeAdjustPlugin
