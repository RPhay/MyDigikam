/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-09-07
 * Description : Qt Model for ImportUI - drag and drop handling
 *
 * SPDX-FileCopyrightText: 2012      by Islam Wazery <wazery at ubuntu dot com>
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QMenu>

// Local includes

#include "abstractitemdragdrophandler.h"
#include "importimagemodel.h"
#include "album.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT ImportDragDropHandler : public AbstractItemDragDropHandler
{
    Q_OBJECT

public:

    explicit ImportDragDropHandler(ImportItemModel* const model);

    ImportItemModel* model()   const;

    bool           dropEvent(QAbstractItemView* view,
                             const QDropEvent* e,
                             const QModelIndex& droppedOn)      override;

    Qt::DropAction accepts(const QDropEvent* e,
                           const QModelIndex& dropIndex)        override;

    QStringList    mimeTypes() const                            override;
    QMimeData*     createMimeData(const QList<QModelIndex>&)    override;

private:

    enum DropAction
    {
        NoAction,
        CopyAction,
        MoveAction,
        GroupAction,
        AssignTagAction
    };

private:

    QAction*   addGroupAction(QMenu* const menu);
    QAction*   addCancelAction(QMenu* const menu);
    DropAction copyOrMove(const QDropEvent* e,
                          QWidget* const view,
                          bool allowMove = true,
                          bool askForGrouping = false);
};

} // namespace Digikam
