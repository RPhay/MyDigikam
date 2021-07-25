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

#ifndef DIGIKAM_MJPEG_FRAME_THREAD_H
#define DIGIKAM_MJPEG_FRAME_THREAD_H

// Qt includes

#include <QImage>
#include <QByteArray>
#include <QColor>
#include <QPoint>
#include <QFont>
#include <QImage>
#include <QDateTime>

// Libwestbox includes

#include "actionthread.h"
#include "mjpegserver.h"

using namespace Digikam;

namespace DigikamGenericMjpegStreamPlugin
{

class MjpegFrameThread : public ActionThreadBase
{
    Q_OBJECT

public:

    explicit MjpegFrameThread(QObject* const parent);
    ~MjpegFrameThread() override;

    void createFrameJob(const MjpegServerMap&);

Q_SIGNALS:

    void signalFrameChanged(const QByteArray&);
};

// ---------------------------------------------------------------

class MjpegFrame : public ActionJob
{
    Q_OBJECT

public:

    class OSDProperties
    {

    public:

        OSDProperties()
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

        QString       m_title;
        bool          m_titleShowDate;
        QDateTime     m_titleDate;          ///< Local date to show
        bool          m_titleShowRelDate;
        quint64       m_titleRelDate;       ///< Relative date to show  get from chrono device
        QPoint        m_titlePos;
        QFont         m_titleFnt;
        Qt::Alignment m_titleAlign;
        QColor        m_titleBg;
        QImage        m_titleLogo;

        // -----

        QString       m_desc;
        QPoint        m_descPos;
        QFont         m_descFnt;
        Qt::Alignment m_descAlign;
        QColor        m_descBg;

        // -----

        QString       m_comment;
        QPoint        m_commentPos;
        QFont         m_commentFnt;
        Qt::Alignment m_commentAlign;
        QColor        m_commentBg;
    };

public:

    explicit MjpegFrame();
    ~MjpegFrame();

    /**
     * Handle JPEG quality settings.
     * Compression: [ 1 (high) ... 100 (low) ].
     * Default = 75 (normal and good quality compression).
     */
    bool setQuality(int qa);
    int  quality() const;

    /// Setup the list of albums to share with MJPEG server.
    void setCollectionMap(const MjpegServerMap&);

public:

    /**
     * Insert time-stamp OSD on frame.
     */
    static void insertOSDToFrame(QImage& frame, const OSDProperties& osd = OSDProperties());

    /**
     * Convert a QImage to a byte-aaray off JPEG data file.
     */
    static QByteArray imageToJPEGArray(const QImage& frame, int quality);

Q_SIGNALS:

    /**
     * Emit JPEG frames from generator.
     */
    void signalFrameChanged(const QByteArray& frame);

private:

    void run();

private:

    int            m_quality;       ///< jpeg compression [1...100].
    QList<QUrl>    m_urlsList;      ///< Ordered list of list to stream.
    MjpegServerMap m_collectionMap; ///< The current albums collection to share.
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_FRAME_THREAD_H
