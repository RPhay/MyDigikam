/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-21
 * Description : Collection scanning to database - private containers.
 *
 * SPDX-FileCopyrightText: 2005-2006 by Tom Albers <tomalbers at kde dot nl>
 * SPDX-FileCopyrightText: 2007-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "collectionscanner.h"

// C++ includes

#include <sys/types.h>
#include <sys/stat.h>

#ifndef Q_CC_MSVC
#   include <unistd.h>
#endif

// Qt includes

#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QWriteLocker>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QStringList>
#include <QSet>
#include <QElapsedTimer>
#include <QScopedPointer>

// Local includes

#include "drawfiles.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "coredb.h"
#include "collectionmanager.h"
#include "collectionlocation.h"
#include "collectionscannerobserver.h"
#include "coredbaccess.h"
#include "coredbbackend.h"
#include "coredbtransaction.h"
#include "coredboperationgroup.h"
#include "itemcomments.h"
#include "itemcopyright.h"
#include "iteminfo.h"
#include "itemscanner.h"
#include "metaenginesettings.h"
#include "tagscache.h"
#include "thumbsdbaccess.h"
#include "thumbsdb.h"

namespace Digikam
{

bool s_modificationDateEquals(const QDateTime& a, const QDateTime& b);

// --------------------------------------------------------------------

class Q_DECL_HIDDEN NewlyAppearedFile
{

public:

    NewlyAppearedFile() = default;
    NewlyAppearedFile(int albumId, const QString& fileName);

    bool operator==(const NewlyAppearedFile& other) const;

public:

    int     albumId = 0;
    QString fileName;
};

// --------------------------------------------------------------------

inline uint qHash(const NewlyAppearedFile& file)
{
    return (::qHash(file.albumId) ^ ::qHash(file.fileName));
}

// --------------------------------------------------------------------

class Q_DECL_HIDDEN CollectionScannerHintContainerImplementation : public CollectionScannerHintContainer
{
public:

    void recordHints(const QList<AlbumCopyMoveHint>& hints) override;
    void recordHints(const QList<ItemCopyMoveHint>& hints)  override;
    void recordHints(const QList<ItemChangeHint>& hints)    override;
    void recordHint(const ItemMetadataAdjustmentHint& hint) override;

    void clear() override;

    bool hasAnyNormalHint(qlonglong id);
    bool hasAlbumHints();
    bool hasModificationHint(qlonglong id);
    bool hasRescanHint(qlonglong id);
    bool hasMetadataAboutToAdjustHint(qlonglong id);
    bool hasMetadataAdjustedHint(qlonglong id);

public:

    QReadWriteLock                                                        lock;

    QHash<CollectionScannerHints::DstPath, CollectionScannerHints::Album> albumHints;
    QHash<NewlyAppearedFile, qlonglong>                                   itemHints;
    QSet<qlonglong>                                                       modifiedItemHints;
    QSet<qlonglong>                                                       rescanItemHints;
    QHash<qlonglong, QDateTime>                                           metadataAboutToAdjustHints;
    QHash<qlonglong, QDateTime>                                           metadataAdjustedHints;
};

// --------------------------------------------------------------------

class Q_DECL_HIDDEN CollectionScanner::Private
{

public:

    Private() = default;

public:

    void resetRemovedItemsTime();
    void removedItems();

    bool checkObserver();
    bool checkDeferred(const QFileInfo& info);

    void finishScanner(ItemScanner& scanner);

public:

    QSet<QString>                                 nameFilters;
    QSet<QString>                                 imageFilterSet;
    QSet<QString>                                 videoFilterSet;
    QSet<QString>                                 audioFilterSet;
    QSet<QString>                                 ignoreDirectory;

    QList<int>                                    scannedAlbums;
    bool                                          wantSignals               = false;
    bool                                          needTotalFiles            = false;
    bool                                          performFastScan           = true;

    QDateTime                                     removedItemsTime;

    CollectionScannerHintContainerImplementation* hints                     = nullptr;
    QHash<int, int>                               establishedSourceAlbums;
    bool                                          updatingHashHint          = false;

    bool                                          recordHistoryIds          = false;
    QSet<qlonglong>                               needResolveHistorySet;
    QSet<qlonglong>                               needTaggingHistorySet;

    bool                                          deferredFileScanning      = false;
    QSet<QString>                                 deferredAlbumPaths;

    QHash<QString, QDateTime>                     albumDateCache;
    QList<qlonglong>                              newIdsList;

    CollectionScannerObserver*                    observer                  = nullptr;
};

} // namespace Digikam
