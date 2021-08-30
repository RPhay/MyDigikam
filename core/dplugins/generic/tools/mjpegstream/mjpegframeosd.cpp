/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG frame on screen display.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "mjpegframeosd.h"

// Qt includes

#include <QString>
#include <QBuffer>
#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QDateTime>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegFrameOsd::MjpegFrameOsd()
  : m_desc           (QLatin1String("")),
    m_descShowDate   (true),
    m_descShowRelDate(false),
    m_descRelDate    (0),
    m_descPos        (QPoint(0,0)),
    m_descFnt        (QFont(QLatin1String("Monospace"))),
    m_descAlign      (Qt::AlignLeft),
    m_descBg         (Qt::darkGray),

    m_messPos       (QPoint(0, 0)),
    m_messFnt       (QFont(QLatin1String("Monospace"))),
    m_messAlign     (Qt::AlignLeft),
    m_messBg        (Qt::darkGray)
{
    m_descFnt.setStyleHint(QFont::Monospace);
    m_descFnt.setPixelSize(8);
    m_descFnt.setBold(true);
    m_messFnt.setStyleHint(QFont::Monospace);
    m_descFnt.setPixelSize(8);
    m_messFnt.setBold(true);

}

MjpegFrameOsd::~MjpegFrameOsd()
{
}

void MjpegFrameOsd::insertOsdToFrame(QImage& frm,
                                     const QUrl& url,
                                     const MjpegStreamSettings& settings)
{
    // TODO: use settings.iface to populate items properties based on url,
    // eg. album name, rating, tags, labels, date, etc.

    // Description section

    QString mess = m_desc;

    if (m_descShowDate)
    {
        // date must be provided by caller

        mess.append(m_descDate.toString(QLatin1String("\ndd-MM-yyyy hh:mm:ss.zzz\n")));
    }

    if (m_descShowRelDate)
    {
        // jour = 24 * 60 * 60 * 1000 = 86 400 000

        int nbj   = m_descRelDate  / 86400000;
        int reste = m_descRelDate  % 86400000;

        // h = 60 * 60 * 1000 = 3600000

        int nbh   = reste  / 3600000;
        reste     = reste % 3600000;

        // m =  60 * 1000 = 60000

        int nbm   = reste  / 60000;
        reste     = reste % 60000;

        // s =  1000

        int nbs   = reste  / 1000;
        int nbms  = reste % 1000;
        QString extraDateFormat = QString::number(nbj) +
                                  QLatin1String(":")   +
                                  QString::number(nbh) +
                                  QLatin1String(":")   +
                                  QString::number(nbm) +
                                  QLatin1String(":")   +
                                  QString::number(nbs) +
                                  QLatin1String(":")   +
                                  QString::number(nbms)+
                                  QLatin1String(" ms");
        mess.append(extraDateFormat);

        //mess.append(QLatin1String(" ms"));
    }

    QPainter p(&frm);

    QFontMetrics descMt(m_descFnt);
    p.setFont(m_descFnt);

    QRect descRect = descMt.boundingRect(0, 0, frm.width(), frm.height(), 0, mess);
    QRect bgdescRect(m_descPos.x(),
                     m_descPos.y() - descRect.height() + 1,
                     descRect.width(),
                     descRect.height() + 3);

    p.fillRect(bgdescRect, m_descBg);

    p.setPen(QPen(Qt::white));
    p.drawText(bgdescRect, m_descAlign, mess);

    if (!m_descLogo.isNull())
    {
        p.drawImage(m_descPos.x(), m_descPos.y() + 5, m_descLogo);
    }

    m_desc.clear();
}

} // namespace DigikamGenericMjpegStreamPlugin
