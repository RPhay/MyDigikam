/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-16
 * Description : Qt Model for Albums - drag and drop handling
 *
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "albummodeldragdrophandler.h"
#include "albummodel.h"

namespace Digikam
{

class AlbumDragDropHandler : public AlbumModelDragDropHandler
{
    Q_OBJECT

public:

    explicit AlbumDragDropHandler(AlbumModel* const model);

    AlbumModel* model() const;

    bool dropEvent(QAbstractItemView* view,
                   const QDropEvent* e,
                   const QModelIndex& droppedOn)         override;

    Qt::DropAction accepts(const QDropEvent* e,
                           const QModelIndex& dropIndex) override;

    QStringList mimeTypes() const override;
    QMimeData* createMimeData(const QList<Album*>&)      override;
};

} // namespace Digikam
