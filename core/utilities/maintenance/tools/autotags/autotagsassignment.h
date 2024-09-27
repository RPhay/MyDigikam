/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Autotags Assignment maintenance tool
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>

// Local includes

#include "album.h"
#include "maintenancetool.h"
#include "imagequalitycontainer.h"

class QImage;

namespace Digikam
{

class ItemInfo;

class AutotagsAssignment : public MaintenanceTool
{
    Q_OBJECT

public:

    enum AutotagsAssignmentScanMode
    {
        AllItems = 0,        ///< Clean all tags already assigned and re-scan all items.
        NonAssignedItems     ///< Scan only items with no tags assigned.
    };

public:

    /**
     * Constructor using AlbumList as argument. If list is empty, whole Albums collection is processed.
     */
    explicit AutotagsAssignment(AutotagsAssignmentScanMode mode,
                                const AlbumList& list,
                                int modelType,
                                const QStringList& langs,
                                ProgressItem* const parent = nullptr);
    ~AutotagsAssignment()           override;

    void setUseMultiCoreCPU(bool b) override;

private:

    void processOne();

private Q_SLOTS:

    void slotStart()                override;
    void slotCancel()               override;
    void slotAdvance(const ItemInfo&, const QImage&);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
