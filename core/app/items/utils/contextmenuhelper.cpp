/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-15
 * Description : contextmenu helper class
 *
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "contextmenuhelper_p.h"

namespace Digikam
{

ContextMenuHelper::ContextMenuHelper(QMenu* const parent)
    : QObject(parent),
      d      (new Private(this))
{
    d->parent              = parent;
    d->stdActionCollection = DigikamApp::instance()->actionCollection();
}

ContextMenuHelper::~ContextMenuHelper()
{
    delete d;
}

void ContextMenuHelper::setItemFilterModel(ItemFilterModel* model)
{
    d->imageFilterModel = model;
}

void ContextMenuHelper::setSelectedIds(const imageIds& ids)
{
    if (d->selectedIds.isEmpty())
    {
        d->selectedIds = ids;
    }
}

void ContextMenuHelper::setSelectedItems(const QList<QUrl>& urls)
{
    if (d->selectedItems.isEmpty())
    {
        d->selectedItems = urls;
    }
}

bool ContextMenuHelper::imageIdsHaveSameCategory(const imageIds& ids, DatabaseItem::Category category)
{
    bool sameCategory = true;
    QVariantList varList;

    foreach (const qlonglong& id, ids)
    {
        varList = CoreDbAccess().db()->getImagesFields(id, DatabaseFields::Category);

        if (varList.isEmpty() ||
            ((DatabaseItem::Category)varList.first().toInt() != category))
        {
            sameCategory = false;
            break;
        }
    }

    return sameCategory;
}

} // namespace Digikam
