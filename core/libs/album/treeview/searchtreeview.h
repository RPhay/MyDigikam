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

#pragma once

// Local includes

#include "abstractcheckablealbumtreeview.h"

namespace Digikam
{

class SearchTreeView : public AbstractCheckableAlbumTreeView
{
    Q_OBJECT

public:

    explicit SearchTreeView(QWidget* const parent = nullptr, Flags flags = DefaultFlags);
    ~SearchTreeView() override = default;

    /// Note: not filtered by search type
    SearchModel* albumModel()          const;

    /// Contains only the searches with appropriate type - prefer to albumModel()
    SearchFilterModel* filteredModel() const;
    SAlbum* currentAlbum()             const;

    void setAlbumModel(SearchModel* const model);
    void setAlbumFilterModel(SearchFilterModel* const filteredModel, CheckableAlbumFilterModel* const model);

public Q_SLOTS:

    void setCurrentAlbums(const QList<Album*>& albums, bool selectInAlbumManager = true);
    void setCurrentAlbum(int searchId, bool selectInAlbumManager = true);

protected:

    SearchFilterModel* m_filteredModel = nullptr;
};

} // namespace Digikam
