/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
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

#include "OSDFilter.h"

// Qt includes

#include <QPainter>

// Local includes

#include "QtAV_Statistics.h"
#include "digikam_debug.h"

namespace AVPlayer
{

OSDFilter::OSDFilter(QObject* const parent)
    : VideoFilter(parent),
      OSD        ()
{
}

void OSDFilter::process(Statistics* statistics, VideoFrame* frame)
{
    Q_UNUSED(frame);

    if (mShowType == ShowNone)
        return;

/*
    /qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
        << QString::asprintf("ctx=%p tid=%p main tid=%p",
            ctx, QThread::currentThread(), qApp->thread());
*/

    context()->drawPlainText(context()->rect, Qt::AlignCenter, text(statistics));
}

} // namespace AVPlayer
