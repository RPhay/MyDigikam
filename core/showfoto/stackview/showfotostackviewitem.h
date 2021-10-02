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

#ifndef SHOWFOTO_STACK_VIEW_ITEM_H
#define SHOWFOTO_STACK_VIEW_ITEM_H

// Qt includes

#include <QString>
#include <QPixmap>
#include <QTreeWidgetItem>

// Local includes

#include "showfotoiteminfo.h"

namespace ShowFoto
{

class ShowfotoStackViewList;

class ShowfotoStackViewItem : public QTreeWidgetItem
{

public:

    explicit ShowfotoStackViewItem(ShowfotoStackViewList* const parent);
    ~ShowfotoStackViewItem()               override;

    void setInfo(const ShowfotoItemInfo&);
    ShowfotoItemInfo info()      const;

    void setThumbnail(const QPixmap&);

private:

    ShowfotoItemInfo m_info;

    Q_DISABLE_COPY(ShowfotoStackViewItem)
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_ITEM_H
