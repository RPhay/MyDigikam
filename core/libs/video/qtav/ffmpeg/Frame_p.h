/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_FRAME_P_H
#define QTAV_FRAME_P_H

#include "Frame.h"

// Qt includes

#include <QVector>
#include <QVariant>
#include <QSharedData>

// Local includes

#include "QtAV_Global.h"

namespace QtAV
{

class Q_DECL_HIDDEN FramePrivate : public QSharedData
{
    Q_DISABLE_COPY(FramePrivate)

public:

    FramePrivate()
        : timestamp (0),
          data_align(1)
    {
    }

    virtual ~FramePrivate()
    {
    }

public:

    QVector<uchar*> planes;     // slice
    QVector<int>    line_sizes; // stride
    QVariantMap     metadata;
    QByteArray      data;
    qreal           timestamp;
    int             data_align;
};

} // namespace QtAV

#endif // QTAV_FRAME_P_H
