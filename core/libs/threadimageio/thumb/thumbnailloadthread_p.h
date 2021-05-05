/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-06-05
 * Description : Thumbnail loading - private containers
 *
 * Copyright (C) 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_THUMB_NAIL_LOAD_THREAD_P_H
#define DIGIKAM_THUMB_NAIL_LOAD_THREAD_P_H

#include "thumbnailloadthread.h"

// Qt includes

#include <QApplication>
#include <QMessageBox>
#include <QEventLoop>
#include <QPainter>
#include <QHash>
#include <QIcon>
#include <QMimeType>
#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "iccmanager.h"
#include "iccprofile.h"
#include "loadingcache.h"
#include "thumbsdbaccess.h"
#include "thumbnailsize.h"
#include "thumbnailcreator.h"

namespace Digikam
{

class Q_DECL_HIDDEN ThumbnailResult
{

public:

    explicit ThumbnailResult(const LoadingDescription& description, const QImage& image)
        : loadingDescription(description),
          image             (image)
    {
    }

    LoadingDescription loadingDescription;
    QImage             image;
};

// -------------------------------------------------------------------

class Q_DECL_HIDDEN ThumbnailLoadThreadStaticPriv
{
public:

    explicit ThumbnailLoadThreadStaticPriv()
      : firstThreadCreated  (false),
        storageMethod       (ThumbnailCreator::FreeDesktopStandard),
        provider            (nullptr),
        profile             (IccProfile::sRGB())
    {
    }

    ~ThumbnailLoadThreadStaticPriv()
    {
        delete provider;
    }

public:

    bool                            firstThreadCreated;

    ThumbnailCreator::StorageMethod storageMethod;
    ThumbnailInfoProvider*          provider;
    IccProfile                      profile;
};

// -------------------------------------------------------------------

class Q_DECL_HIDDEN ThumbnailLoadThread::Private
{

public:

    explicit Private()
      : wantPixmap          (true),
        highlight           (true),
        sendSurrogate       (true),
        notifiedForResults  (false),
        size                (ThumbnailSize::maxThumbsSize()),
        creator             (nullptr)
    {
    }

    bool                               wantPixmap;
    bool                               highlight;
    bool                               sendSurrogate;
    bool                               notifiedForResults;

    int                                size;

    ThumbnailCreator*                  creator;

    QHash<QString, ThumbnailResult>    collectedResults;
    QMutex                             resultsMutex;

    QList<LoadingDescription>          lastDescriptions;

public:

    LoadingDescription        createLoadingDescription(const ThumbnailIdentifier& identifier, int size, bool setLastDescription = true);
    LoadingDescription        createLoadingDescription(const ThumbnailIdentifier& identifier, int size,
                                                       const QRect& detailRect, bool setLastDescription = true);
    bool                      checkDescription(const LoadingDescription& description);
    QList<LoadingDescription> makeDescriptions(const QList<ThumbnailIdentifier>& identifiers, int size);
    QList<LoadingDescription> makeDescriptions(const QList<QPair<ThumbnailIdentifier, QRect> >& idsAndRects, int size);
    bool                      hasHighlightingBorder()                       const;
    int                       pixmapSizeForThumbnailSize(int thumbnailSize) const;
    int                       thumbnailSizeForPixmapSize(int pixmapSize)    const;
};

// --- ThumbnailImageCatcher ---------------------------------------------------------

class Q_DECL_HIDDEN ThumbnailImageCatcher::Private
{

public:

    enum CatcherState
    {
        Inactive,
        Accepting,
        Waiting,
        Quitting
    };

public:

    class Q_DECL_HIDDEN CatcherResult
    {
    public:

        explicit CatcherResult(const LoadingDescription& d)
            : description(d),
              received   (false)
        {
        }

        CatcherResult(const LoadingDescription& d, const QImage& image)
            : image      (image),
              description(d),
              received   (true)
        {
        }

    public:

        QImage             image;
        LoadingDescription description;
        bool               received;
    };

public:

    explicit Private()
      : state (Inactive),
        active(true),
        thread(nullptr)
    {
    }

    void reset();
    void harvest(const LoadingDescription& description, const QImage& image);

public:

    CatcherState                  state;

    bool                          active;
    ThumbnailLoadThread*          thread;
    QList<Private::CatcherResult> tasks;
    QList<Private::CatcherResult> intermediate;

    QMutex                        mutex;
    QWaitCondition                condVar;
};

} // namespace Digikam

#endif // DIGIKAM_THUMB_NAIL_LOAD_THREAD_P_H
