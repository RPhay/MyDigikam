/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 20013-08-22
 * Description : List View Model with support for mime data and drag-n-drop
 *
 * Copyright (C) 2013 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
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

#ifndef DIGIKAM_TAG_MNGR_LIST_MODEL_H
#define DIGIKAM_TAG_MNGR_LIST_MODEL_H

// Qt includes

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

namespace Digikam
{

class ListItem;

class TagMngrListModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    explicit TagMngrListModel(QObject* const parent = nullptr);
    ~TagMngrListModel() override;

    /**
     * @brief addItem   - add new item to list
     * @param values    - A list of data for item: Name as QString, QBrush as
     *                    background and qlonglong as id
     * @return          - pointer to newly created listitem
     */
    ListItem* addItem(QList<QVariant> values);

    /**
     * @brief allItems  - return all items from List, usually to be saved
     *                    in KConfig
     */
    QList<ListItem*> allItems()                                                                 const;

    void deleteItem(ListItem* const item);

    /**
     * Standard methods to be implemented when subcassing QAbstractItemModel
     */
    QVariant data(const QModelIndex& index, int role)                                           const override;

    Qt::ItemFlags flags(const QModelIndex& index)                                               const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)   const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex())           const override;

    QModelIndex parent(const QModelIndex& index)                                                const override;

    int rowCount(const QModelIndex& parent = QModelIndex())                                     const override;

    int columnCount(const QModelIndex& parent = QModelIndex())                                  const override;

    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    /**
     * Reimplemented methods for handling drag-n-drop, encoding and decoding
     * mime types
     */
    Qt::DropActions supportedDropActions()                                                      const override;
    QStringList mimeTypes()                                                                     const override;
    QMimeData* mimeData(const QModelIndexList& indexes)                                         const override;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row,
                      int column, const QModelIndex& parent) override;

    QList<int> getDragNewSelection()                                                            const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_TAG_MNGR_LIST_MODEL_H
