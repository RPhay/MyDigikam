/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
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

#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QLocale>
#include <QDateTime>
#include <QMimeDatabase>
#include <QFileInfo>

// KDE includes

#include <klocalizedstring.h>

// Local include

#include "digikam_debug.h"
#include "showfotostackviewlist.h"
#include "itempropertiestab.h"
#include "drawdecoder.h"

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

void ShowfotoStackViewItem::setInfo(const ShowfotoItemInfo& info)
{
    m_info                 = info;
    setText(ShowfotoStackViewList::FileName, m_info.name);

    QDateTime dt           = (m_info.ctime.isValid() ? m_info.ctime : m_info.dtime);
    QString str            = QLocale().toString(dt, QLocale::ShortFormat);
    setText(ShowfotoStackViewList::FileDate, str);
    setData(ShowfotoStackViewList::FileDate, Qt::UserRole, dt);

    QFileInfo fileInfo(m_info.name);
    QString rawFilesExt    = DRawDecoder::rawFiles();
    QString ext            = fileInfo.suffix().toUpper();

    if (!ext.isEmpty() && rawFilesExt.toUpper().contains(ext))
    {
        setText(ShowfotoStackViewList::FileType, i18nc("@info: item properties", "RAW Image"));
    }
    else
    {
        setText(ShowfotoStackViewList::FileType, QMimeDatabase().mimeTypeForFile(fileInfo).comment());
    }

    QString localeFileSize = QLocale().toString(info.size);
    str                    = ItemPropertiesTab::humanReadableBytesCount(m_info.size);
    setText(ShowfotoStackViewList::FileSize, str);
    setData(ShowfotoStackViewList::FileSize, Qt::UserRole, info.size);
}

ShowfotoItemInfo ShowfotoStackViewItem::info() const
{
    return m_info;
}

void ShowfotoStackViewItem::setThumbnail(const QPixmap& thumb)
{
    QPixmap pix = thumb.scaled(treeWidget()->iconSize(), Qt::KeepAspectRatio,
                                                         Qt::FastTransformation);

    QPixmap icon(treeWidget()->iconSize());
    icon.fill(Qt::transparent);
    QPainter p(&icon);
    p.drawPixmap((icon.width()  - pix.width() ) / 2,
                 (icon.height() - pix.height()) / 2,
                 pix);

    setIcon(0, icon);
}

bool ShowfotoStackViewItem::operator<(const QTreeWidgetItem& other) const
{
    bool result = false;
    int column  = treeWidget()->sortColumn();

    switch (column)
    {
        case ShowfotoStackViewList::FileSize:
        {
            result = (data(ShowfotoStackViewList::FileSize, Qt::UserRole).toInt() < other.data(ShowfotoStackViewList::FileSize, Qt::UserRole).toInt());
            break;
        }

        case ShowfotoStackViewList::FileType:
        {
            result = (text(ShowfotoStackViewList::FileType) < other.text(ShowfotoStackViewList::FileType));
            break;
        }

        case ShowfotoStackViewList::FileDate:
        {
            result = (data(ShowfotoStackViewList::FileDate, Qt::UserRole).toDateTime() < other.data(ShowfotoStackViewList::FileDate, Qt::UserRole).toDateTime());
            break;
        }

        default:    // ShowfotoStackViewList::FileName
        {
            result = (text(ShowfotoStackViewList::FileName) < other.text(ShowfotoStackViewList::FileName));
            break;
        }
    }

    return result;
}

} // namespace ShowFoto
