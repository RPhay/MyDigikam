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
