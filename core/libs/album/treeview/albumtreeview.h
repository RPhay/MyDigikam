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
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ALBUM_TREE_VIEW_H
#define DIGIKAM_ALBUM_TREE_VIEW_H

// Local includes

#include "abstractcheckablealbumtreeview.h"

namespace Digikam
{

class AlbumTreeView : public AbstractCheckableAlbumTreeView
{
    Q_OBJECT

public:

    explicit AlbumTreeView(QWidget* const parent = nullptr, Flags flags = DefaultFlags);
    ~AlbumTreeView() override;

    AlbumModel* albumModel()                        const;
    PAlbum* currentAlbum()                          const;
    PAlbum* albumForIndex(const QModelIndex& index) const;

    void setAlbumFilterModel(CheckableAlbumFilterModel* const filterModel);
    void setAlbumModel(AlbumModel* const model);

public Q_SLOTS:

    void setCurrentAlbums(const QList<Album*>& albums, bool selectInAlbumManager = true);
    void setCurrentAlbum(int albumId, bool selectInAlbumManager = true);
};

} // namespace Digikam

#endif // DIGIKAM_ALBUM_TREE_VIEW_H
