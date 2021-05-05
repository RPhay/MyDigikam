/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-05
 * Description : a list view to display black frames - the item
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QLocale>
#include <QScopedPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dmetadata.h"
#include "ditemtooltip.h"
#include "itempropertiestab.h"
#include "blackframeparser.h"

namespace Digikam
{

class Q_DECL_HIDDEN BlackFrameListViewItem::Private
{
public:

    explicit Private()
      : parser(nullptr)
    {
    }

    QList<HotPixelProps> hotPixels;

    QUrl                 blackFrameUrl;

    BlackFrameParser*    parser;

    QString              toolTipStr;
};

BlackFrameListViewItem::BlackFrameListViewItem(QTreeWidget* const parent, const QUrl& url)
    : QObject        (parent),
      QTreeWidgetItem(parent),
      d              (new Private)
{
    d->blackFrameUrl = url;
    d->parser        = new BlackFrameParser(this);
    d->parser->parseBlackFrame(url);

    connect(d->parser, SIGNAL(signalHotPixelsParsed(QList<HotPixelProps>)),
            this, SLOT(slotHotPixelsParsed(QList<HotPixelProps>)));

    connect(this, SIGNAL(signalHotPixelsParsed(QList<HotPixelProps>,QUrl)),
            parent, SLOT(slotHotPixelsParsed(QList<HotPixelProps>,QUrl)));

    connect(d->parser, SIGNAL(signalLoadingProgress(float)),
            this, SLOT(slotLoadingProgress(float)));
}

BlackFrameListViewItem::~BlackFrameListViewItem()
{
    delete d;
}

QUrl BlackFrameListViewItem::frameUrl() const
{
    return d->blackFrameUrl;
}

QString BlackFrameListViewItem::toolTipString() const
{
    return d->toolTipStr;
}

void BlackFrameListViewItem::slotLoadingProgress(float v)
{
    setText(SIZE,      i18n("Loading Frame"));
    setText(HOTPIXELS, QString::fromLatin1("%1 %").arg((int)(v*100)));
}

void BlackFrameListViewItem::slotHotPixelsParsed(const QList<HotPixelProps>& hotPixels)
{
    d->hotPixels  = hotPixels;

    // First scale it down to the size

    QSize size   = QSize(THUMB_WIDTH, THUMB_WIDTH/3*2);
    QImage thumb = d->parser->image().smoothScale(size, Qt::KeepAspectRatio).copyQImage();

    // And draw the hot pixel positions on the thumb

    QPainter p(&thumb);

    // Take scaling into account

    float xRatio   = 0.0;
    float yRatio   = 0.0;
    float hpThumbX = 0.0;
    float hpThumbY = 0.0;
    QRect hpRect;

    xRatio = (float)size.width()  / (float)d->parser->image().width();
    yRatio = (float)size.height() / (float)d->parser->image().height();

    // Draw hot pixels one by one

    QList<HotPixelProps>::const_iterator it1;

    for (it1 = d->hotPixels.constBegin() ; it1 != d->hotPixels.constEnd() ; ++it1)
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

    if (!d->parser->image().size().isEmpty())
    {
        setText(SIZE, QString::fromUtf8("%1x%2").arg(d->parser->image().width()).arg(d->parser->image().height()));
    }

    // Amount of hot pixels

    setText(HOTPIXELS, QString::number(d->hotPixels.count()));

    // Descriptions as tooltip (file name, camera model, and hot pixels list)

    QString value;
    QString header = i18n("Black Frame");

    QScopedPointer<DMetadata> meta(new DMetadata(d->blackFrameUrl.toLocalFile()));
    PhotoInfoContainer info = meta->getPhotographInformation();

    d->toolTipStr.clear();

    DToolTipStyleSheet cnt;
    QString tip    = cnt.tipHeader;

    d->toolTipStr += cnt.headBeg + header + cnt.headEnd;

    d->toolTipStr += cnt.cellBeg + i18n("File Name:") + cnt.cellMid;
    d->toolTipStr += d->blackFrameUrl.fileName() + cnt.cellEnd;

    QString make   = info.make;
    QString model  = info.model;
    ItemPropertiesTab::shortenedMakeInfo(make);
    ItemPropertiesTab::shortenedModelInfo(model);
    d->toolTipStr += cnt.cellBeg + i18n("Make/Model:") + cnt.cellMid;
    d->toolTipStr += QString::fromUtf8("%1/%2").arg(make).arg(model) + cnt.cellEnd;

    d->toolTipStr += cnt.cellBeg + i18nc("@info: creation date", "Created:") + cnt.cellMid;
    d->toolTipStr += QLocale().toString(info.dateTime, QLocale::ShortFormat) + cnt.cellEnd;

    d->toolTipStr += cnt.cellBeg + i18n("Serial Number:") + cnt.cellMid;
    d->toolTipStr += meta->getCameraSerialNumber() + cnt.cellEnd;

    QString hplist;

    for (QList <HotPixelProps>::const_iterator it2 = d->hotPixels.constBegin() ;
         it2 != d->hotPixels.constEnd() ; ++it2)
    {
        hplist.append(QString::fromUtf8("[%1,%2] ").arg((*it2).x()).arg((*it2).y()));
    }

    d->toolTipStr += cnt.cellBeg + i18n("Hot Pixels:") + cnt.cellMid;
    d->toolTipStr += cnt.elidedText(hplist, Qt::ElideRight) + cnt.cellEnd;
    d->toolTipStr += cnt.tipFooter;

    emitHotPixelsParsed();
}

void BlackFrameListViewItem::emitHotPixelsParsed()
{
    emit signalHotPixelsParsed(d->hotPixels, d->blackFrameUrl);
}

} // namespace Digikam
