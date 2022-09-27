/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-02-06
 * Description : Thumbnail bar for items
 *
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_THUMBNAIL_BAR_H
#define DIGIKAM_ITEM_THUMBNAIL_BAR_H

// Local includes

#include "itemcategorizedview.h"

namespace Digikam
{

class ItemThumbnailBar : public ItemCategorizedView
{
    Q_OBJECT

public:

    explicit ItemThumbnailBar(QWidget* const parent = nullptr);
    ~ItemThumbnailBar() override;

    /// Sets the policy always for the one scroll bar which is relevant, depending on orientation
    void setScrollBarPolicy(Qt::ScrollBarPolicy policy);
    void setFlow(QListView::Flow newFlow);

    void installOverlays();

    /**
     * This installs a duplicate filter model, if the ItemModel may contain duplicates.
     * Otherwise, just use setModels().
     */
    void setModelsFiltered(ItemModel* model, ImageSortFilterModel* filterModel);

    QModelIndex nextIndex(const QModelIndex& index)     const;
    QModelIndex previousIndex(const QModelIndex& index) const;
    QModelIndex firstIndex()                            const;
    QModelIndex lastIndex()                             const;

public Q_SLOTS:

    void assignRating(const QList<QModelIndex>& index, int rating);
    void slotDockLocationChanged(Qt::DockWidgetArea area);

protected:

    void slotSetupChanged() override;
    bool event(QEvent*)     override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_THUMBNAIL_BAR_H
