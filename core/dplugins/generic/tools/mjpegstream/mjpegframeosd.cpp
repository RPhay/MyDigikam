/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG frame on screen display.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
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

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "itempropertiestab.h"

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
    m_messFnt.setPixelSize(8);
    m_messFnt.setBold(true);

}

MjpegFrameOsd::~MjpegFrameOsd()
{
}

void MjpegFrameOsd::PopulateOSD(QImage& frm,
                                const QUrl& url,
                                const MjpegStreamSettings& settings)
{
    // First stage is to get the information map from host application.

    DInfoInterface::DInfoMap info = settings.iface->itemInfo(url);
    DItemInfo item(info);

    QString comment  = item.comment();
    QString title    = item.title();
    QStringList tags = item.keywords();
    int rating       = item.rating();

    QString str;

    m_descFnt = settings.osdFont;

    // Display tag names.

    if (settings.printTags)
    {
        printTags(tags);
    }

    // Display descs.

    if (settings.printTitle)
    {
        str.clear();

        if (!title.isEmpty())
        {
            str     += title;
            m_desc.append(QString::fromLatin1("\n%1").arg(str));
        }
    }

    // Display Captions if no Titles.

    if (settings.printCapIfNoTitle)
    {
        str.clear();

        if (title.isEmpty())
        {
            str     += comment;
            printComments(str);
        }
    }

    // Display image File Name.

    if (settings.printName)
    {
        str.clear();
        QString name = url.fileName();

        if (!name.isEmpty())
        {
            str += name;
            m_desc.append(QString::fromLatin1("\n%1").arg(str));
        }
    }

    //  Display Comments.

    if (settings.printComment)
    {
        str.clear();

        if (title.isEmpty())
        {
            str     += comment;
            printComments(str);
        }
    }

    // Display Make and Model.

    if (settings.printMakeModel)
    {
        str.clear();

        QString make  = item.make();
        QString model = item.model();

        if (!make.isEmpty())
        {
            ItemPropertiesTab::shortenedMakeInfo(make);
            str = make;
        }

        if (!model.isEmpty())
        {
            if (!make.isEmpty())
            {
                str += QLatin1String(" / ");
            }

            ItemPropertiesTab::shortenedModelInfo(model);
            str += model;
        }

        if (!str.isEmpty())
        {
            m_desc.append(QString::fromLatin1("\n%1").arg(str));
        }
    }

    // Display Lens model.

    if (settings.printLensModel)
    {
        str.clear();

        QString lens = item.lens();

        if (!lens.isEmpty())
        {
            str += lens;
            m_desc.append(QString::fromLatin1("\n%1").arg(str));
        }
    }

    // Display Exposure and Sensitivity.

    if (settings.printExpoSensitivity)
    {
        str.clear();

        QString exposureTime = item.exposureTime();
        QString sensitivity  = item.sensitivity();

        if (!exposureTime.isEmpty())
        {
            str = exposureTime;
        }

        if (!sensitivity.isEmpty())
        {
            if (!exposureTime.isEmpty())
            {
                str += QLatin1String(" / ");
            }

            str += i18n("%1 ISO", sensitivity);
        }

        if (!str.isEmpty())
        {
            m_desc.append(QString::fromLatin1("\n%1").arg(str));
        }
    }

    // Display Aperture and Focal.

    if (settings.printApertureFocal)
    {
        str.clear();

        QString aperture        = item.aperture();
        QString focalLength     = item.focalLength();
        QString focalLength35mm = item.focalLength35mm();

        if (!aperture.isEmpty())
        {
            str = aperture;
        }

        if (focalLength35mm.isEmpty())
        {
            if (!focalLength.isEmpty())
            {
                if (!aperture.isEmpty())
                {
                    str += QLatin1String(" / ");
                }

                str += focalLength;
            }
        }
        else
        {
            if (!aperture.isEmpty())
            {
                str += QLatin1String(" / ");
            }

            if (!focalLength.isEmpty())
            {
                str += QString::fromUtf8("%1 (%2)").arg(focalLength).arg(focalLength35mm);
            }
            else
            {
                str += QString::fromUtf8("%1").arg(focalLength35mm);
            }
        }

        if (!str.isEmpty())
        {
            m_desc.append(QString::fromLatin1("\n%1").arg(str));
        }
    }

    // Display Creation Date.

    if (settings.printDate)
    {
        QDateTime dateTime = item.dateTime();

        if (dateTime.isValid())
        {
            // str = QLocale().toString(dateTime, QLocale::ShortFormat);
            m_descDate = dateTime;
        }
    }

    // Display rating

    if (settings.printRating)
    {
        if (rating != -1)
        {
            m_desc.append(QString::fromLatin1("\n%1/5").arg(rating));
        }
    }
}

void MjpegFrameOsd::printTags(QStringList& tags)
{
    tags.sort();

    QString str = tags.join(QLatin1String(", "));

    if (!str.isEmpty())
    {
        m_desc.append(QString::fromLatin1("%1").arg(str));
    }
}

void MjpegFrameOsd::printComments(const QString& comments)
{
    QStringList commentsByLines;

    uint commentsIndex = 0;     // Comments QString index

    while (commentsIndex < (uint)comments.length())
    {
        QString newLine;
        bool breakLine = false; // End Of Line found
        uint currIndex;         // Comments QString current index

        // Check minimal lines dimension

        uint maxStringLen = 80;

        uint commentsLinesLengthLocal = maxStringLen;

        for (currIndex = commentsIndex ;
             (currIndex < (uint)comments.length()) && !breakLine ; ++currIndex)
        {
            if ((comments.at(currIndex) == QLatin1Char('\n')) || comments.at(currIndex).isSpace())
            {
                breakLine = true;
            }
        }

        if (commentsLinesLengthLocal <= (currIndex - commentsIndex))
        {
            commentsLinesLengthLocal = (currIndex - commentsIndex);
        }

        breakLine = false;

        for (currIndex = commentsIndex ;
             (currIndex <= (commentsIndex + commentsLinesLengthLocal)) &&
             (currIndex < (uint)comments.length()) && !breakLine ;
             ++currIndex)
        {
            breakLine = (comments.at(currIndex) == QLatin1Char('\n')) ? true : false;

            if (breakLine)
            {
                newLine.append(QLatin1Char(' '));
            }
            else
            {
                newLine.append(comments.at(currIndex));
            }
        }

        commentsIndex = currIndex; // The line is ended

        if (commentsIndex != (uint)comments.length())
        {
            while (!newLine.endsWith(QLatin1Char(' ')))
            {
                newLine.truncate(newLine.length() - 1);
                --commentsIndex;
            }
        }

        commentsByLines.prepend(newLine.trimmed());
    }

    for (int i = 0 ; i < (int)commentsByLines.count() ; ++i)
    {
        m_desc.append(QString::fromLatin1("\n%1").arg(commentsByLines.at(i)));
    }

}


void MjpegFrameOsd::insertOsdToFrame(QImage& frm,
                                     const QUrl& url,
                                     const MjpegStreamSettings& settings)
{
    // Populate items properties based on url,
    // eg. album name, rating, tags, labels, date, etc.

    PopulateOSD(frm, url, settings);

    // Description section

    QString mess = m_desc;

    if (m_descShowDate)
    {
        // date must be provided by caller

        mess.append(m_descDate.toString(QLatin1String("\ndd-MM-yyyy hh:mm:ss.zzz")));
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
                     m_descPos.y(),
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

void MjpegFrameOsd::insertMessageOsdToFrame(QImage &frm,
                                            const QSize& JPEGsize,
                                            const QString& mess)
{
    QString message = mess;
    QPainter p(&frm);

    if (JPEGsize.width() <= 480 && JPEGsize.height() <= 480)
    {
        m_messFnt.setPixelSize(18);
    }
    else
    {
        m_messFnt.setPixelSize(60);
    }

    //---

    QFontMetrics messMt(m_messFnt);
    p.setFont(m_messFnt);

    QRect messRect = messMt.boundingRect(0, 0, frm.width(), frm.height(), 0, message);

    // print message in the middle of frame

    QRect bgErreurRect(m_messPos.x(),
                       m_messPos.y(),
                       messRect.width(),
                       messRect.height() + 3);

    p.fillRect(bgErreurRect, m_messBg);

    p.setPen(QPen(Qt::white));
    p.drawText(bgErreurRect, m_messAlign, message);
}

} // namespace DigikamGenericMjpegStreamPlugin
