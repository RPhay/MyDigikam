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

#define THUMB_WIDTH 150

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

namespace Digikam
{

BlackFrameListViewItem::BlackFrameListViewItem(QTreeWidget* const parent, const QUrl& url)
    : QObject(parent),
      QTreeWidgetItem(parent),
      m_blackFrameURL(url)
{
    m_parser = new BlackFrameParser(this);
    m_parser->parseBlackFrame(url);

    connect(m_parser, SIGNAL(signalParsed(QList<HotPixelProps>)),
            this, SLOT(slotParsed(QList<HotPixelProps>)));

    connect(this, SIGNAL(signalParsed(QList<HotPixelProps>,QUrl)),
            parent, SLOT(slotParsed(QList<HotPixelProps>,QUrl)));

    connect(m_parser, SIGNAL(signalLoadingProgress(float)),
            this, SIGNAL(signalLoadingProgress(float)));

    connect(m_parser, SIGNAL(signalLoadingComplete()),
            this, SIGNAL(signalLoadingComplete()));
}

BlackFrameListViewItem::~BlackFrameListViewItem()
{
}

void BlackFrameListViewItem::slotParsed(const QList<HotPixelProps>& hotPixels)
{
    m_hotPixels  = hotPixels;

    // First scale it down to the size

    QSize size   = QSize(THUMB_WIDTH, THUMB_WIDTH/3*2);
    QImage thumb = m_parser->image().smoothScale(size, Qt::KeepAspectRatio).copyQImage();

    // And draw the hot pixel positions on the thumb

    QPainter p(&thumb);

    // Take scaling into account

    float xRatio, yRatio;
    float hpThumbX, hpThumbY;
    QRect hpRect;

    xRatio = (float)size.width()  / (float)m_parser->image().width();
    yRatio = (float)size.height() / (float)m_parser->image().height();

    // Draw hot pixels one by one

    QList<HotPixelProps>::const_iterator it;

    for (it = m_hotPixels.constBegin() ; it != m_hotPixels.constEnd() ; ++it)
    {
        hpRect   = (*it).rect;
        hpThumbX = (hpRect.x() + hpRect.width()  / 2) * xRatio;
        hpThumbY = (hpRect.y() + hpRect.height() / 2) * yRatio;

        p.setPen(QPen(Qt::black));
        p.drawLine((int) hpThumbX,      (int) hpThumbY - 1, (int) hpThumbX, (int) hpThumbY + 1);
        p.drawLine((int) hpThumbX  - 1, (int) hpThumbY, (int) hpThumbX + 1, (int) hpThumbY);
        p.setPen(QPen(Qt::white));
        p.drawPoint((int) hpThumbX - 1, (int) hpThumbY - 1);
        p.drawPoint((int) hpThumbX + 1, (int) hpThumbY + 1);
        p.drawPoint((int) hpThumbX - 1, (int) hpThumbY + 1);
        p.drawPoint((int) hpThumbX + 1, (int) hpThumbY - 1);
    }

    // Preview

    setIcon(0, QPixmap::fromImage(thumb));

    // Image size

    if (!m_parser->image().size().isEmpty())
    {
        setText(1, QString::fromUtf8("%1x%2").arg(m_parser->image().width()).arg(m_parser->image().height()));
    }

    // Amount of hot pixels

    setText(2, QString::number(m_hotPixels.count()));

    // Descriptions as tooltip (file name, camera model, and hot pixels list)

    DMetadata meta(m_blackFrameURL.toLocalFile());
    PhotoInfoContainer info = meta.getPhotographInformation();

    QString blackFrameDesc  = i18n("<p>File Name: %1<p>", m_blackFrameURL.fileName());
    blackFrameDesc.append(i18n("<p>Make/Model: %1/%2</p>", info.make, info.model));

    QString hplist;

    for (QList <HotPixelProps>::const_iterator it = m_hotPixels.constBegin() ;
         it != m_hotPixels.constEnd() ; ++it)
    {
        hplist.append(QString::fromUtf8("[%1,%2] ").arg((*it).x()).arg((*it).y()));
    }

    blackFrameDesc.append(i18n("<p>Hot Pixels: %1</p>", hplist));

    setToolTip(0, blackFrameDesc);

    emitSlotParsed();
}

void BlackFrameListViewItem::emitSlotParsed()
{
    emit signalParsed(m_hotPixels, m_blackFrameURL);
}

// ----------------------------------------------------------------------------

BlackFrameListView::BlackFrameListView(QWidget* const parent)
    : QTreeWidget(parent)
{
    setColumnCount(3);
    setRootIsDecorated(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAllColumnsShowFocus(true);
    setIconSize(QSize(THUMB_WIDTH, THUMB_WIDTH));

    QStringList labels;
    labels.append(i18n("Preview"));
    labels.append(i18n("Size"));
    labels.append(i18nc("This is a column which will contain the amount of HotPixels "
                        "found in the black frame file", "HP"));
    setHeaderLabels(labels);

    connect(this, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotSelectionChanged()));
}

BlackFrameListView::~BlackFrameListView()
{
}

void BlackFrameListView::slotSelectionChanged()
{
    BlackFrameListViewItem* const item = dynamic_cast<BlackFrameListViewItem*>(currentItem());

    if (item)
    {
        item->emitSlotParsed();
    }
}

void BlackFrameListView::slotParsed(const QList<HotPixelProps>& hotPixels, const QUrl& blackFrameURL)
{
    emit signalBlackFrameSelected(hotPixels, blackFrameURL);
}

} // namespace Digikam
