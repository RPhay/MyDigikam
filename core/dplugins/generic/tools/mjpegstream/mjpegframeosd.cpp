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
  : m_titleShowDate   (true),
    m_titleShowRelDate(false),
    m_titleRelDate    (0),
    m_titlePos        (QPoint(30, 30)),
    m_titleFnt        (QFont(QLatin1String("Monospace"))),
    m_titleAlign      (Qt::AlignLeft),
    m_titleBg         (Qt::darkGray),
    m_descPos         (QPoint(30, 500)),
    m_descFnt         (QFont(QLatin1String("Monospace"))),
    m_descAlign       (Qt::AlignLeft),
    m_descBg          (Qt::darkGray),
    m_commentPos      (QPoint(30, 560)),
    m_commentFnt      (QFont(QLatin1String("Monospace"))),
    m_commentAlign    (Qt::AlignLeft),
    m_commentBg       (Qt::darkGray)
{
    m_titleFnt.setStyleHint(QFont::Monospace);
    m_titleFnt.setPixelSize(12);
    m_titleFnt.setBold(true);
    m_descFnt.setStyleHint(QFont::Monospace);
    m_descFnt.setPixelSize(12);
    m_descFnt.setBold(true);
    m_commentFnt.setStyleHint(QFont::Monospace);
    m_commentFnt.setPixelSize(12);
    m_commentFnt.setBold(true);
}

MjpegFrameOsd::~MjpegFrameOsd()
{
}

void MjpegFrameOsd::insertOsdToFrame(QImage& frm, const MjpegFrameOsd& osd)
{
    // Title section

    QString mess = osd.m_title;

    if (osd.m_titleShowDate)
    {
        // date must be provided by caller

        mess.append(osd.m_titleDate.toString(QLatin1String("\ndd-MM-yyyy hh:mm:ss.zzz\n")));
    }

    if (osd.m_titleShowRelDate)
    {
        // jour = 24 * 60 * 60 * 1000 = 86 400 000

        int nbj   = osd.m_titleRelDate  / 86400000;
        int reste =  osd.m_titleRelDate  % 86400000;

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

    QFontMetrics titleMt(osd.m_titleFnt);
    p.setFont(osd.m_titleFnt);

    QRect titleRect = titleMt.boundingRect(0, 0, frm.width(), frm.height(), 0, mess);
    QRect bgTitleRect(osd.m_titlePos.x(),
                      osd.m_titlePos.y() - titleRect.height() + 1,
                      titleRect.width(),
                      titleRect.height() + 3);

    p.fillRect(bgTitleRect, osd.m_titleBg);

    p.setPen(QPen(Qt::white));
    p.drawText(bgTitleRect, osd.m_titleAlign, mess);

    if (!osd.m_titleLogo.isNull())
    {
        p.drawImage(osd.m_titlePos.x(), osd.m_titlePos.y() + 5, osd.m_titleLogo);
    }

    // Description section

    if (!osd.m_desc.isEmpty())
    {
        QFontMetrics descMt(osd.m_descFnt);
        p.setFont(osd.m_descFnt);

        QRect descRect = descMt.boundingRect(0, 0, frm.width(), frm.height(),
                                             0, osd.m_desc);
        QRect bgDescRect(osd.m_descPos.x(),
                         osd.m_descPos.y() - descRect.height() + 1,
                         descRect.width(),
                         descRect.height() + 3);

        p.fillRect(bgDescRect, osd.m_descBg);

        p.setPen(QPen(Qt::white));
        p.drawText(bgDescRect, osd.m_descAlign, osd.m_desc);
    }

    // Comment section

    if (!osd.m_comment.isEmpty())
    {
        QFontMetrics commentMt(osd.m_commentFnt);
        p.setFont(osd.m_commentFnt);

        QRect commentRect = commentMt.boundingRect(0, 0, frm.width(), frm.height(),
                                                   0, osd.m_comment);
        QRect bgCommentRect(osd.m_commentPos.x(),
                            osd.m_commentPos.y() - commentRect.height() + 1,
                            commentRect.width(),
                            commentRect.height() + 3);

        p.fillRect(bgCommentRect, osd.m_commentBg);

        p.setPen(QPen(Qt::white));
        p.drawText(bgCommentRect, osd.m_commentAlign, osd.m_comment);
    }
}

} // namespace DigikamGenericMjpegStreamPlugin
