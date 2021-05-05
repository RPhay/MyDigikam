/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-12-26
 * Description : images versions QTreeView
 *
 * Copyright (C) 2010 by Martin Klapetek <martin dot klapetek at gmail dot com>
 * Copyright (C) 2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef DIGIKAM_VERSIONS_TREEVIEW_H
#define DIGIKAM_VERSIONS_TREEVIEW_H

// Qt includes

#include <QTreeView>

// Local includes

#include "digikam_export.h"
#include "dragdropimplementations.h"

namespace Digikam
{

class VersionsDelegate;
class ItemDelegateOverlay;

class VersionsTreeView : public QTreeView,
                         public DragDropViewImplementation
{
    Q_OBJECT

public:

    explicit VersionsTreeView(QWidget* const parent = nullptr);
    ~VersionsTreeView()                                                override;

    void setToolTipEnabled(bool on);

    void addOverlay(ItemDelegateOverlay* overlay);
    void removeOverlay(ItemDelegateOverlay* overlay);

    VersionsDelegate* delegate()                                 const;

protected:

    void paintEvent(QPaintEvent* e)                                    override;
    void mouseMoveEvent(QMouseEvent* event)                            override;
    bool viewportEvent(QEvent* event)                                  override;
    QModelIndex moveCursor(CursorAction cursorAction,
                           Qt::KeyboardModifiers modifiers)            override;

    DECLARE_VIEW_DRAG_DROP_METHODS(QTreeView)
    QModelIndex mapIndexForDragDrop(const QModelIndex& index)    const override;
    QPixmap     pixmapForDrag(const QList<QModelIndex>& indexes) const override;
    AbstractItemDragDropHandler* dragDropHandler()               const override;
    virtual void setDragDropHandler(AbstractItemDragDropHandler* handler);

protected:

    class ToolTip;

    VersionsDelegate*            m_delegate;
    AbstractItemDragDropHandler* m_dragDropHandler;
    bool                         m_showToolTip;
    ToolTip*                     m_toolTip;

private:

    // Disable
    VersionsTreeView(const VersionsTreeView&)            = delete;
    VersionsTreeView& operator=(const VersionsTreeView&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_VERSIONS_TREEVIEW_H
