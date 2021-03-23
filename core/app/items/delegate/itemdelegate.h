/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-19
 * Description : Qt model-view for items - the delegate
 *
 * Copyright (C) 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ITEM_DELEGATE_H
#define DIGIKAM_ITEM_DELEGATE_H

// Local includes

#include "itemviewdelegate.h"
#include "thumbnailsize.h"

namespace Digikam
{

class ItemCategoryDrawer;
class ItemCategorizedView;
class ItemDelegateOverlay;
class ItemFilterModel;
class ItemModel;
class ItemThumbnailModel;

class ItemDelegate : public ItemViewDelegate
{
    Q_OBJECT

public:

    explicit ItemDelegate(QObject* const parent = nullptr);
    ~ItemDelegate()                                                                                    override;

    void setView(ItemCategorizedView* view);

    ItemCategoryDrawer* categoryDrawer()                                                         const;

    QRect commentsRect()                                                                         const;
    QRect tagsRect()                                                                             const;
    QRect actualPixmapRect(const QModelIndex& index)                                             const;
    QRect groupIndicatorRect()                                                                   const;
    QRect coordinatesIndicatorRect()                                                             const;

    int calculatethumbSizeToFit(int ws);

    void setSpacing(int spacing) override;
    void setDefaultViewOptions(const QStyleOptionViewItem& option)                                     override;
    bool acceptsToolTip(const QPoint& pos, const QRect& visualRect,
                                const QModelIndex& index, QRect* tooltipRect = nullptr)          const override;
    bool acceptsActivation(const QPoint& pos, const QRect& visualRect,
                                   const QModelIndex& index, QRect* activationRect = nullptr)    const override;

    QRect pixmapRect()                                                                           const override;
    QRect imageInformationRect()                                                                 const override;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)  const override;
    QPixmap pixmapForDrag(const QStyleOptionViewItem& option, const QList<QModelIndex>& indexes) const override;

    /**
     * Retrieve the thumbnail pixmap in given size for the ItemModel::ThumbnailRole for
     * the given index from the given index, which must adhere to ItemThumbnailModel semantics.
     */
    static QPixmap retrieveThumbnailPixmap(const QModelIndex& index, int thumbnailSize);

public:

    // Declared as public because of use in DigikamItemDelegate class.
    class ItemDelegatePrivate;

protected:

    /** Reimplement this to set contentWidth. This is the maximum width of all
     *  content rectangles, typically excluding margins on both sides.
     */
    virtual void updateContentWidth();

    /** In a subclass, you need to implement this method to set up the rects
     *  for drawing. The paint() method operates depending on these rects.
     */
    virtual void updateRects() = 0;

    void clearCaches() override;

    /** Reimplement to clear caches based on model indexes (hash on row number etc.)
     *  Change signals are listened to this is called whenever such properties become invalid.
     */
    virtual void clearModelDataCaches();

    virtual QPixmap thumbnailPixmap(const QModelIndex& index)               const;

    bool onActualPixmapRect(const QPoint& pos, const QRect& visualRect,
                            const QModelIndex& index, QRect* actualRect)    const;
    void updateActualPixmapRect(const QModelIndex& index, const QRect& rect);

    void invalidatePaintingCache() override;
    void updateSizeRectsAndPixmaps() override;

    void setModel(QAbstractItemModel* model);

    ItemDelegate(ItemDelegate::ItemDelegatePrivate& dd, QObject* const parent);

protected Q_SLOTS:

    void modelChanged();
    void modelContentsChanged();

private:

    Q_DECLARE_PRIVATE(ItemDelegate)
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_DELEGATE_H
