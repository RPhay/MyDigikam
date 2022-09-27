/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-07-08
 * Description : Item delegate for import interface items - private container.
 *
 * SPDX-FileCopyrightText: 2012 by Islam Wazery <wazery at ubuntu dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_VIEW_IMPORT_DELEGATE_P_H
#define DIGIKAM_ITEM_VIEW_IMPORT_DELEGATE_P_H

#include "itemviewimportdelegate.h"

// Qt includes

#include <QCache>
#include <QFont>
#include <QPainter>
#include <QPolygon>
#include <QModelIndex>

// Local includes

#include "digikam_debug.h"
#include "thumbnailsize.h"

namespace Digikam
{

class ItemDelegateOverlay;
class ItemViewImportDelegate;

class Q_DECL_HIDDEN ItemViewImportDelegatePrivate
{
public:

    explicit ItemViewImportDelegatePrivate();
    virtual ~ItemViewImportDelegatePrivate() = default;

    void init(ItemViewImportDelegate* const _q);

    void makeStarPolygon();

    /// Resets cached rects. Remember to reimplement in subclass for added rects.
    virtual void clearRects();

public:

    int                       spacing;
    QSize                     gridSize;

    QRect                     rect;
    QRect                     ratingRect;

    QPixmap                   regPixmap;
    QPixmap                   selPixmap;
    QVector<QPixmap>          ratingPixmaps;

    QFont                     font;
    QFont                     fontReg;
    QFont                     fontCom;
    QFont                     fontXtra;

    QPolygon                  starPolygon;
    QSize                     starPolygonSize;

    ThumbnailSize             thumbSize;

    QPersistentModelIndex     editingRating;

    ItemViewImportDelegate*   q;

    QRect                     oneRowRegRect;
    QRect                     oneRowComRect;
    QRect                     oneRowXtraRect;

    /// constant values for drawing
    int                       radius;
    int                       margin;

private:

    Q_DISABLE_COPY(ItemViewImportDelegatePrivate)
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_VIEW_IMPORT_DELEGATE_P_H
