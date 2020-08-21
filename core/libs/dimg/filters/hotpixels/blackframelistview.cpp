/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-05
 * Description : a list view to display black frames
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
    : QTreeWidget(parent),
      m_toolTipItem(nullptr)
{
    setColumnCount(3);
    setRootIsDecorated(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAllColumnsShowFocus(true);
    setIconSize(QSize(BlackFrameListViewItem::THUMB_WIDTH, BlackFrameListViewItem::THUMB_WIDTH));
    viewport()->setMouseTracking(true);

    QStringList labels;
    labels.append(i18n("Preview"));
    labels.append(i18n("Size"));
    labels.append(i18nc("This is a column which will contain the amount of HotPixels "
                        "found in the black frame file", "Hot Pixels"));
    setHeaderLabels(labels);

    m_toolTip      = new BlackFrameToolTip(this);
    m_toolTipTimer = new QTimer(this);

    connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSelectionChanged()));

    connect(m_toolTipTimer, SIGNAL(timeout()),
            this, SLOT(slotToolTip()));
}

BlackFrameListView::~BlackFrameListView()
{
    delete m_toolTip;
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

QList<QUrl> BlackFrameListView::itemUrls()
{
    QList<QUrl> urls;
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        BlackFrameListViewItem* const item = dynamic_cast<BlackFrameListViewItem*>(*it);

        if (item)
        {
            urls << item->frameUrl();
        }

        ++it;
    }

    return urls;
}

void BlackFrameListView::slotSelectionChanged()
{
    BlackFrameListViewItem* const item = dynamic_cast<BlackFrameListViewItem*>(currentItem());

    if (item)
    {
        item->emitHotPixelsParsed();
    }
}

void BlackFrameListView::slotHotPixelsParsed(const QList<HotPixelProps>& hotPixels,
                                             const QUrl& blackFrameUrl)
{
    emit signalBlackFrameSelected(hotPixels, blackFrameUrl);
}

void BlackFrameListView::hideToolTip()
{
    m_toolTipItem = nullptr;
    m_toolTipTimer->stop();
    slotToolTip();
}

bool BlackFrameListView::acceptToolTip(const QPoint& pos) const
{
    if (columnAt(pos.x()) == 0)
    {
        return true;
    }

    return false;
}

void BlackFrameListView::slotToolTip()
{
    m_toolTip->setItem(m_toolTipItem);
}

void BlackFrameListView::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() == Qt::NoButton)
    {
        BlackFrameListViewItem* const item = dynamic_cast<BlackFrameListViewItem*>(itemAt(e->pos()));

        if (!isActiveWindow() || !item)
        {
            hideToolTip();
            return;
        }

        if (item != m_toolTipItem)
        {
            hideToolTip();

            if (acceptToolTip(e->pos()))
            {
                m_toolTipItem = item;
                m_toolTip->setToolTipString(item->toolTipString());
                m_toolTipTimer->setSingleShot(true);
                m_toolTipTimer->start(500);
            }
        }

        if ((item == m_toolTipItem) && !acceptToolTip(e->pos()))
        {
            hideToolTip();
        }

        return;
    }

    hideToolTip();
    QTreeWidget::mouseMoveEvent(e);
}

void BlackFrameListView::wheelEvent(QWheelEvent* e)
{
    hideToolTip();
    QTreeWidget::wheelEvent(e);
}

void BlackFrameListView::keyPressEvent(QKeyEvent* e)
{
    hideToolTip();
    QTreeWidget::keyPressEvent(e);
}

void BlackFrameListView::focusOutEvent(QFocusEvent* e)
{
    hideToolTip();
    QTreeWidget::focusOutEvent(e);
}

void BlackFrameListView::leaveEvent(QEvent* e)
{
    hideToolTip();
    QTreeWidget::leaveEvent(e);
}

} // namespace Digikam
