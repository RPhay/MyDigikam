/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-30-08
 * Description : batch thumbnails generator
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>
#include <QImage>

// Local includes

#include "album.h"
#include "maintenancetool.h"

namespace Digikam
{

class ItemInfo;

class ThumbsGenerator : public MaintenanceTool
{
    Q_OBJECT

public:

    /**
     * Constructor using Album Id as argument. If Id = -1, whole Albums collection is processed.
     */
    explicit ThumbsGenerator(const bool rebuildAll, int albumId, ProgressItem* const parent = nullptr);

    /**
     * Constructor using AlbumList as argument. If list is empty, whole Albums collection is processed.
     */
    ThumbsGenerator(const bool rebuildAll, const AlbumList& list, ProgressItem* const parent = nullptr);
    ~ThumbsGenerator() override;

    void setUseMultiCoreCPU(bool b) override;

private:

    void init(const bool rebuildAll);

private Q_SLOTS:

    void slotStart()                override;
    void slotCancel()               override;
    void slotAdvance(const ItemInfo&, const QImage&);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
