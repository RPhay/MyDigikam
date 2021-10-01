/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto stack view item
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

#include "showfotostackviewitem.h"

// Qt includes

#include <QPixmap>
#include <QLocale>
#include <QDateTime>

// Local include

#include "digikam_debug.h"
#include "showfotostackviewlist.h"
#include "itempropertiestab.h"

using namespace Digikam;

namespace ShowFoto
{

ShowfotoStackViewItem::ShowfotoStackViewItem(ShowfotoStackViewList* const parent)
    : QTreeWidgetItem(parent)
{
    setDisabled(false);
    setSelected(false);
}

ShowfotoStackViewItem::~ShowfotoStackViewItem()
{
}

void ShowfotoStackViewItem::setItemInfo(const ShowfotoItemInfo& info)
{
    m_info                 = info;
    setText(ShowfotoStackViewList::FileName, m_info.name);

    QDateTime createdDate  = m_info.ctime;
    QString str            = QLocale().toString(createdDate, QLocale::ShortFormat);
    setText(ShowfotoStackViewList::FileDate, str);

    setText(ShowfotoStackViewList::FileType, m_info.mime);

    QString localeFileSize = QLocale().toString(info.size);
    str                    = ItemPropertiesTab::humanReadableBytesCount(m_info.size);
    setText(ShowfotoStackViewList::FileSize, str);
}

ShowfotoItemInfo ShowfotoStackViewItem::itemInfo() const
{
    return m_info;
}

void ShowfotoStackViewItem::setThumbnail(const QImage& img)
{
    setIcon(0, QPixmap::fromImage(img));
}

} // namespace ShowFoto
