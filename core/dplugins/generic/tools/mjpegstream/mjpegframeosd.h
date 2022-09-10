/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG frame on screen display.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021 by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MJPEG_FRAME_OSD_H
#define DIGIKAM_MJPEG_FRAME_OSD_H

// Qt includes

#include <QUrl>
#include <QColor>
#include <QPoint>
#include <QFont>
#include <QImage>
#include <QDateTime>

// Local includes

#include "mjpegstreamsettings.h"

namespace DigikamGenericMjpegStreamPlugin
{

class MjpegFrameOsd
{

public:

    explicit MjpegFrameOsd();
    ~MjpegFrameOsd();

public:

    QString       m_desc;
    QPoint        m_descPos;
    QFont         m_descFnt;
    Qt::Alignment m_descAlign;
    QColor        m_descBg;

public:

    /**
     * Insert OSD on frame.
     */
    void insertOsdToFrame(QImage& frame,
                          const QUrl& url,
                          const MjpegStreamSettings& settings);

    /**
     * Populate OSD items properties base on Url
     */
    void PopulateOSD(QImage& frame,
                     const QUrl& url,
                     const MjpegStreamSettings& settings);

    /**
     * print comments
     */
    void printComments(const QString& comments);

    /**
     * print tags
     */
    void printTags(QStringList& tags);

    /**
     * Insert message OSD on broken frame or end frame
     */
    void insertMessageOsdToFrame(QImage &frame,
                                 const QSize& JPEGsize,
                                 const QString& mess);
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_FRAME_OSD_H
