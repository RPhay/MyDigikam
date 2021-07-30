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

#ifndef DIGIKAM_MJPEG_FRAME_OSD_H
#define DIGIKAM_MJPEG_FRAME_OSD_H

// Qt includes

#include <QByteArray>
#include <QColor>
#include <QPoint>
#include <QFont>
#include <QImage>
#include <QDateTime>

namespace DigikamGenericMjpegStreamPlugin
{

class MjpegFrameOsd
{

public:

    MjpegFrameOsd();
    ~MjpegFrameOsd();

public:

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

public:

    /**
     * Insert OSD on frame.
     */
    void insertOsdToFrame(QImage& frame, const MjpegFrameOsd& osd = MjpegFrameOsd());
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_FRAME_OSD_H
