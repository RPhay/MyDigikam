/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-05
 * Description : a list view to display black frames - the item
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

#include "blackframelistviewitem.h"

// Qt includes

#include <QList>
#include <QPointer>
#include <QPainter>
#include <QPixmap>
#include <QImage>
#include <QRect>
#include <QSize>

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

    connect(m_parser, SIGNAL(signalHotPixelsParsed(QList<HotPixelProps>)),
            this, SLOT(slotHotPixelsParsed(QList<HotPixelProps>)));

    connect(this, SIGNAL(signalHotPixelsParsed(QList<HotPixelProps>,QUrl)),
            parent, SLOT(slotHotPixelsParsed(QList<HotPixelProps>,QUrl)));

    connect(m_parser, SIGNAL(signalLoadingProgress(float)),
            this, SLOT(slotLoadingProgress(float)));
}

BlackFrameListViewItem::~BlackFrameListViewItem()
{
}

QUrl BlackFrameListViewItem::frameUrl() const
{
    return m_blackFrameUrl;
}

void BlackFrameListViewItem::slotLoadingProgress(float v)
{
    setText(SIZE,      i18n("Loading Frame"));
    setText(HOTPIXELS, QString::fromLatin1("%1 %").arg((int)(v*100)));
}

void BlackFrameListViewItem::slotHotPixelsParsed(const QList<HotPixelProps>& hotPixels)
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

    QString blackFrameDesc  = i18n("<p>File Name: %1</p>", m_blackFrameUrl.fileName());
    blackFrameDesc.append(i18n("<p>Make/Model: %1/%2</p>", info.make, info.model));
    blackFrameDesc.append(i18n("<p>Serial Number: %1</p>", meta.getCameraSerialNumber()));

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

    emitHotPixelsParsed();
}

void BlackFrameListViewItem::emitHotPixelsParsed()
{
    emit signalHotPixelsParsed(m_hotPixels, m_blackFrameUrl);
}

} // namespace Digikam
