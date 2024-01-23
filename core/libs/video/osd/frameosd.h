/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : frame on screen display.
 *
 * SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021      by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FRAME_OSD_H
#define DIGIKAM_FRAME_OSD_H

// Qt includes

#include <QUrl>
#include <QColor>
#include <QPoint>
#include <QFont>
#include <QImage>
#include <QDateTime>

// Local includes

#include "digikam_export.h"
#include "frameosdsettings.h"
#include "dinfointerface.h"

namespace Digikam
{

class DIGIKAM_EXPORT FrameOsd
{

public:

    explicit FrameOsd();
    ~FrameOsd();

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
                          const FrameOsdSettings& settings,
                          DInfoInterface* const info);

    /**
     * Populate OSD items properties base on Url
     */
    void populateOSD(const QUrl& url,
                     const FrameOsdSettings& settings,
                     DInfoInterface* const info);

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

} // namespace Digikam

#endif // DIGIKAM_FRAME_OSD_H
