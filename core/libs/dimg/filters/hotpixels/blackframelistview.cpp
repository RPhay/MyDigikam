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
      m_blackFrameUrl(url)
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

QUrl BlackFrameListViewItem::frameUrl() const
{
    return m_blackFrameUrl;
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

    float xRatio   = 0.0;
    float yRatio   = 0.0;
    float hpThumbX = 0.0;
    float hpThumbY = 0.0;
    QRect hpRect;

    xRatio = (float)size.width()  / (float)m_parser->image().width();
    yRatio = (float)size.height() / (float)m_parser->image().height();

    // Draw hot pixels one by one

    QList<HotPixelProps>::const_iterator it1;

    for (it1 = m_hotPixels.constBegin() ; it1 != m_hotPixels.constEnd() ; ++it1)
    {
        hpRect   = (*it1).rect;
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

    setIcon(PREVIEW, QPixmap::fromImage(thumb));

    // Image size

    if (!m_parser->image().size().isEmpty())
    {
        setText(SIZE, QString::fromUtf8("%1x%2").arg(m_parser->image().width()).arg(m_parser->image().height()));
    }

    // Amount of hot pixels

    setText(HOTPIXELS, QString::number(m_hotPixels.count()));

    // Descriptions as tooltip (file name, camera model, and hot pixels list)

    DMetadata meta(m_blackFrameUrl.toLocalFile());
    PhotoInfoContainer info = meta.getPhotographInformation();

    QString blackFrameDesc  = i18n("<p>File Name: %1<p>", m_blackFrameUrl.fileName());
    blackFrameDesc.append(i18n("<p>Make/Model: %1/%2</p>", info.make, info.model));

    QString hplist;

    for (QList <HotPixelProps>::const_iterator it2 = m_hotPixels.constBegin() ;
         it2 != m_hotPixels.constEnd() ; ++it2)
    {
        hplist.append(QString::fromUtf8("[%1,%2] ").arg((*it2).x()).arg((*it2).y()));
    }

    blackFrameDesc.append(i18n("<p>Hot Pixels: %1</p>", hplist));

    setToolTip(PREVIEW,   blackFrameDesc);
    setToolTip(SIZE,      blackFrameDesc);
    setToolTip(HOTPIXELS, blackFrameDesc);

    emitParsedData();
}

void BlackFrameListViewItem::emitParsedData()
{
    emit signalParsed(m_hotPixels, m_blackFrameUrl);
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
        item->emitParsedData();
    }
}

void BlackFrameListView::slotParsed(const QList<HotPixelProps>& hotPixels, const QUrl& blackFrameUrl)
{
    emit signalBlackFrameSelected(hotPixels, blackFrameUrl);
}

} // namespace Digikam
