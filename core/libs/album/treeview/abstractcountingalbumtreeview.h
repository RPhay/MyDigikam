/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-03-25
 * Description : Tree View for album models
 *
 * SPDX-FileCopyrightText: 2009-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ABSTRACT_COUNTING_ALBUM_TREE_VIEW_H
#define DIGIKAM_ABSTRACT_COUNTING_ALBUM_TREE_VIEW_H

// Local includes

#include "abstractalbumtreeview.h"

namespace Digikam
{

class AbstractCountingAlbumTreeView : public AbstractAlbumTreeView
{
    Q_OBJECT

public:

    explicit AbstractCountingAlbumTreeView(QWidget* const parent, Flags flags);

protected:

    void setAlbumModel(AbstractCountingAlbumModel* const model);
    void setAlbumFilterModel(AlbumFilterModel* const filterModel);

    void rowsInserted(const QModelIndex& parent, int start, int end) override;

private Q_SLOTS:

    void slotCollapsed(const QModelIndex& index);
    void slotExpanded(const QModelIndex& index);
    void setShowCountFromSettings();
    void updateShowCountState(const QModelIndex& index, bool recurse);

private:

    void init();
};

} // namespace Digikam

#endif // DIGIKAM_ABSTRACT_COUNTING_ALBUM_TREE_VIEW_H
