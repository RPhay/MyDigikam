/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : a MJPEG frame generator.
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

#include "mjpegframethread.h"

// Qt includes

#include <QString>
#include <QBuffer>
#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QDateTime>

// Local includes

#include "digikam_debug.h"
#include "previewloadthread.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegFrameThread::MjpegFrameThread(QObject* const parent)
    : ActionThreadBase(parent)
{
}

MjpegFrameThread::~MjpegFrameThread()
{
    // cancel the thread
    cancel();
    // wait for the thread to finish
    wait();
}

void MjpegFrameThread::createFrameJob(const MjpegServerMap& map)
{
    ActionJobCollection collection;

    MjpegFrameTask* const t = new MjpegFrameTask();
    t->setCollectionMap(map);
    t->setQuality(75);
    t->setInterval(5);
    t->setLoop(true);

    connect(t, SIGNAL(signalFrameChanged(QByteArray)),
            this, SIGNAL(signalFrameChanged(QByteArray)));

    collection.insert(t, 0);

    appendJobs(collection);
}

// -----------------------------------------------------------

MjpegFrameTask::MjpegFrameTask()
    : ActionJob (nullptr),
      m_loop    (true),
      m_quality (75),
      m_interval(5)
{
}

MjpegFrameTask::~MjpegFrameTask()
{
}

bool MjpegFrameTask::setQuality(int qa)
{
    if ((qa < 1) || (qa > 100))
    {
        qCWarning(DIGIKAM_GENERAL_LOG) << "Error: quality value is out of range: " << qa;
        return false;
    }

    m_quality = qa;
    qCDebug(DIGIKAM_GENERAL_LOG) << "JPEG quality:" << m_quality;

    return true;
}

int MjpegFrameTask::quality() const
{
    return m_quality;
}

void MjpegFrameTask::setInterval(int inter)
{
    m_interval = inter;
}

int MjpegFrameTask::interval() const
{
    return m_interval;
}

void MjpegFrameTask::setLoop(bool b)
{
    m_loop = b;
}

bool MjpegFrameTask::isLoop() const
{
    return m_loop;
}

void MjpegFrameTask::setCollectionMap(const MjpegServerMap& map)
{
    m_urlsList.clear();

    for (MjpegServerMap::const_iterator it = map.constBegin() ; it != map.constEnd() ; ++it)
    {
        m_urlsList.append(it.value());
    }
}

void MjpegFrameTask::insertOSDToFrame(QImage& frm, const OSDProperties& osd)
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
        // date must be provided by caller
        // Julian TODO

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

QByteArray MjpegFrameTask::imageToJPEGArray(const QImage& frame)
{
    QByteArray outbuf;
    QBuffer buffer(&outbuf);
    buffer.open(QIODevice::WriteOnly);
    frame.save(&buffer, "JPEG", m_quality);

    return outbuf;
}

void MjpegFrameTask::run()
{
    OSDProperties osd;
    QImage img;
    DImg dimg;

    do
    {
        foreach (const QUrl& url, m_urlsList)
        {
            if (m_cancel)
            {
                break;
            }

            qCDebug(DIGIKAM_GENERAL_LOG) << "MjpegStream: Generate frame for" << url.toLocalFile();

            osd.m_title          = url.fileName();
            osd.m_titlePos       = QPoint(280, 474);
            osd.m_titleFnt.setPixelSize(15);
            osd.m_titleBg        = Qt::black;
            osd.m_desc           = QLatin1String("digiKam");
            osd.m_descPos        = QPoint(330, 90);
            osd.m_descFnt.setPixelSize(26);
            osd.m_descAlign      = Qt::AlignCenter;
            osd.m_descBg         = Qt::black;

            dimg = PreviewLoadThread::loadSynchronously(url.toLocalFile(), PreviewSettings(), 1920);

            if (dimg.isNull())
            {
                // TODO: generate error frame.
            }
            else
            {
                img = dimg.copyQImage();
// FIXME                insertOSDToFrame(img, osd);

                emit signalFrameChanged(imageToJPEGArray(img));

                QThread::sleep(m_interval);
            }
        }
    }
    while (!m_cancel && m_loop);

    emit signalDone();
}

} // namespace DigikamGenericMjpegStreamPlugin
