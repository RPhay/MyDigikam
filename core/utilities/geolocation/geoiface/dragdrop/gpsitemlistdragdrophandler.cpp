/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-03-22
 * Description : Drag and drop handler for the item list
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2010      by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "gpsitemlistdragdrophandler.h"

namespace Digikam
{

ItemListDragDropHandler::ItemListDragDropHandler(QObject* const parent)
    : QObject(parent)
{
}

ItemListDragDropHandler::~ItemListDragDropHandler()
{
}

// ------------------------------------------------------------------------------------------------

GPSItemListDragDropHandler::GPSItemListDragDropHandler(QObject* const parent)
    : ItemListDragDropHandler(parent)
{
}

GPSItemListDragDropHandler::~GPSItemListDragDropHandler()
{
}

QMimeData* GPSItemListDragDropHandler::createMimeData(const QList<QPersistentModelIndex>& modelIndices)
{
    MapDragData* const mimeData = new MapDragData();
    mimeData->draggedIndices    = modelIndices;

    return mimeData;
}

} // namespace Digikam
