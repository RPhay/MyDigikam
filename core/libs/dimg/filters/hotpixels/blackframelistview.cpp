/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-05
 * Description : a ListView to display black frames
 *
 * Copyright (C) 2005-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#include "blackframelistview.h"

// Qt includes

#include <QList>
#include <QPointer>
#include <QPainter>
#include <QPixmap>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dmetadata.h"
#include "blackframelistviewitem.h"

namespace Digikam
{

BlackFrameListView::BlackFrameListView(QWidget* const parent)
    : QTreeWidget(parent)
{
    setColumnCount(3);
    setRootIsDecorated(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAllColumnsShowFocus(true);
    setIconSize(QSize(BlackFrameListViewItem::THUMB_WIDTH, BlackFrameListViewItem::THUMB_WIDTH));

    QStringList labels;
    labels.append(i18n("Preview"));
    labels.append(i18n("Size"));
    labels.append(i18nc("This is a column which will contain the amount of HotPixels "
                        "found in the black frame file", "Hot Pixels"));
    setHeaderLabels(labels);

    connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSelectionChanged()));
}

BlackFrameListView::~BlackFrameListView()
{
}

bool BlackFrameListView::contains(const QUrl& url)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        BlackFrameListViewItem* const item = dynamic_cast<BlackFrameListViewItem*>(*it);

        if (item && (item->frameUrl() == url))
        {
            return true;
        }

        ++it;
    }

    return false;
}

void BlackFrameListView::slotSelectionChanged()
{
    BlackFrameListViewItem* const item = dynamic_cast<BlackFrameListViewItem*>(currentItem());

    if (item)
    {
        item->emitHotPixelsParsed();
    }
}

void BlackFrameListView::slotHotPixelsParsed(const QList<HotPixelProps>& hotPixels, const QUrl& blackFrameUrl)
{
    emit signalBlackFrameSelected(hotPixels, blackFrameUrl);
}

} // namespace Digikam
