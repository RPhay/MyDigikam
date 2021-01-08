/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-01
 * Description : A widget to search for places.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Michael G. Hansen <mike at mghansen dot de>
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

#ifndef DIGIKAM_SEARCH_RESULT_MODEL_H
#define DIGIKAM_SEARCH_RESULT_MODEL_H

// Qt includes

#include <QAbstractItemModel>
#include <QItemSelectionModel>

// local includes

#include "searchbackend.h"

class QItemSelection;

namespace DigikamGenericGeolocationEditPlugin
{

class SearchResultItem;

class SearchResultModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    class SearchResultItem
    {
    public:

        SearchBackend::SearchResult result;
    };

public:

    explicit SearchResultModel(QObject* const parent = nullptr);
    ~SearchResultModel() override;

    void addResults(const SearchBackend::SearchResult::List& results);
    SearchResultItem resultItem(const QModelIndex& index) const;
    bool getMarkerIcon(const QModelIndex& index, QPoint* const offset, QSize* const size, QPixmap* const pixmap, QUrl* const url) const;
    void setSelectionModel(QItemSelectionModel* const selectionModel);
    void clearResults();
    void removeRowsByIndexes(const QModelIndexList& rowsList);
    void removeRowsBySelection(const QItemSelection& selection);

    // QAbstractItemModel:
    int columnCount(const QModelIndex& parent = QModelIndex() ) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex() ) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericGeolocationEditPlugin

#endif // DIGIKAM_SEARCH_RESULT_MODEL_H
