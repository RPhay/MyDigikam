/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-24
 * Description : album parser progress indicator
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#include "albumparser.h"

// Qt includes

#include <QTimer>
#include <QApplication>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "album.h"
#include "albummanager.h"
#include "iteminfoalbumsjob.h"
#include "applicationsettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN AlbumParser::Private
{
public:

    explicit Private()
      : cancel(false),
        album(nullptr)
    {
    }

    bool         cancel;
    ItemInfoList infoList;
    Album*       album;
};

AlbumParser::AlbumParser(const ItemInfoList& infoList)
    : ProgressItem(nullptr, QLatin1String("AlbumParser"), QString(), QString(), true, true),
      d(new Private)
{
    d->infoList = infoList;

    ProgressManager::addProgressItem(this);
}

AlbumParser::AlbumParser(Album* const album)
    : ProgressItem(nullptr, QLatin1String("AlbumParser"), QString(), QString(), true, true),
      d(new Private)
{
    d->album = album;

    ProgressManager::addProgressItem(this);
}

AlbumParser::~AlbumParser()
{
    delete d;
}

void AlbumParser::run()
{
    QTimer::singleShot(500, this, SLOT(slotRun()));
}

void AlbumParser::slotRun()
{
    connect(this, SIGNAL(progressItemCanceled(ProgressItem*)),
            this, SLOT(slotCancel()));

    setLabel(i18n("Scanning albums"));
    setThumbnail(QIcon::fromTheme(QLatin1String("digikam")));

    if (d->album)
    {
        AlbumList albumList;
        albumList.append(d->album);
        AlbumIterator it(d->album);

        while (it.current())
        {
            albumList.append(*it);
            ++it;
        }

        ItemInfoAlbumsJob* const job = new ItemInfoAlbumsJob;

        connect(job, SIGNAL(signalCompleted(ItemInfoList)),
                this, SLOT(slotParseItemInfoList(ItemInfoList)));

        job->allItemsFromAlbums(albumList);
    }
    else
    {
        slotParseItemInfoList(d->infoList);
    }
}

void AlbumParser::slotParseItemInfoList(const ItemInfoList& list)
{
    setTotalItems(list.count());

    int i = 0;

    QList<QUrl> imageList;

    for (ItemInfoList::const_iterator it = list.constBegin() ;
         !d->cancel && (it != list.constEnd()) ;
         ++it)
    {
        ItemInfo info = *it;
        imageList.append(info.fileUrl());

        advance(i++);
        qApp->processEvents();
    }

    if (!d->cancel)
    {
        emit signalComplete(imageList);
    }

    setComplete();
}

void AlbumParser::slotCancel()
{
    d->cancel = true;
}

} // namespace Digikam
