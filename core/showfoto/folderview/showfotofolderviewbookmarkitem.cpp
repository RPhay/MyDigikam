/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view bookmarks list
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotofolderviewbookmarkitem.h"

// Local include

#include "digikam_debug.h"

namespace ShowFoto
{

ShowfotoFolderViewBookmarkItem::ShowfotoFolderViewBookmarkItem(QTreeWidgetItem* const parent)
    : QTreeWidgetItem(parent)
{
    setDisabled(false);
    setSelected(false);
}

ShowfotoFolderViewBookmarkItem::~ShowfotoFolderViewBookmarkItem()
{
}

void ShowfotoFolderViewBookmarkItem::setPath(const QString& path)
{
    m_path = path;
}

QString ShowfotoFolderViewBookmarkItem::path() const
{
    return m_path;
}

} // namespace ShowFoto
