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

#ifndef AV_PLAYER_OSDFILTER_H
#define AV_PLAYER_OSDFILTER_H

// Local includes

#include "Filter.h"
#include "FilterContext.h"
#include "OSD.h"

using namespace QtAV;

namespace AVPlayer
{

class OSDFilter : public VideoFilter,
                  public OSD
{
    Q_OBJECT

public:

    explicit OSDFilter(QObject* const parent = nullptr);

    bool isSupported(VideoFilterContext::Type ct)     const override
    {
        return ((ct == VideoFilterContext::QtPainter) || (ct == VideoFilterContext::X11));
    }

protected:

    void process(Statistics* statistics, VideoFrame* frame) override;
};

} // namespace AVPlayer

#endif // AV_PLAYER_OSDFILTER_H
