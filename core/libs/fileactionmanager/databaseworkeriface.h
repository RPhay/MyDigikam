/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-18
 * Description : database worker interface
 *
 * Copyright (C) 2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_DATABASE_WORKER_IFACE_H
#define DIGIKAM_DATABASE_WORKER_IFACE_H

// Qt includes

#include <QList>

// Local includes

#include "fileactionmngr.h"
#include "fileactionimageinfolist.h"
#include "iteminfo.h"
#include "workerobject.h"

namespace Digikam
{

class MetadataHub;

class DatabaseWorkerInterface : public WorkerObject      // clazy:exclude=ctor-missing-parent-argument
{
    Q_OBJECT

public Q_SLOTS:

    virtual void assignTags(const FileActionItemInfoList&, const QList<int>&)      {};
    virtual void removeTags(const FileActionItemInfoList&, const QList<int>&)      {};
    virtual void assignPickLabel(const FileActionItemInfoList&, int)               {};
    virtual void assignColorLabel(const FileActionItemInfoList&, int)              {};
    virtual void assignRating(const FileActionItemInfoList&, int)                  {};
    virtual void editGroup(int, const ItemInfo&, const FileActionItemInfoList&)    {};
    virtual void setExifOrientation(const FileActionItemInfoList&, int)            {};
    virtual void applyMetadata(const FileActionItemInfoList&, DisjointMetadata*)   {};
    virtual void copyAttributes(const FileActionItemInfoList&, const QStringList&) {};

Q_SIGNALS:

    void writeMetadataToFiles(FileActionItemInfoList infos);
    void writeOrientationToFiles(FileActionItemInfoList infos, int orientation);
    void writeMetadata(FileActionItemInfoList infos, int flag);
};

// ------------------------------------------------------------------------------------

class FileActionMngrDatabaseWorker : public DatabaseWorkerInterface
{
    Q_OBJECT

public:

    explicit FileActionMngrDatabaseWorker(FileActionMngr::Private* const dd)
        : d(dd)
    {
    }

public:

    void assignTags(const FileActionItemInfoList& infos, const QList<int>& tagIDs)             override;
    void removeTags(const FileActionItemInfoList& infos, const QList<int>& tagIDs)             override;
    void assignPickLabel(const FileActionItemInfoList& infos, int pickId)                      override;
    void assignColorLabel(const FileActionItemInfoList& infos, int colorId)                    override;
    void assignRating(const FileActionItemInfoList& infos, int rating)                         override;
    void editGroup(int groupAction, const ItemInfo& pick, const FileActionItemInfoList& infos) override;
    void setExifOrientation(const FileActionItemInfoList& infos, int orientation)              override;
    void applyMetadata(const FileActionItemInfoList& infos, DisjointMetadata* hub)             override;
    void copyAttributes(const FileActionItemInfoList& infos, const QStringList& derivedPaths)  override;

private:

    void changeTags(const FileActionItemInfoList& infos, const QList<int>& tagIDs, bool addOrRemove);

private:

    FileActionMngr::Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DATABASE_WORKER_IFACE_H
