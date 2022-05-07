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

#ifndef QTAV_PLAYER_OSDFILTER_H
#define QTAV_PLAYER_OSDFILTER_H

// Local includes

#include "Filter.h"
#include "FilterContext.h"
#include "OSD.h"

using namespace QtAV;

namespace QtAVPlayer
{

class OSDFilter : public VideoFilter,
                  public OSD
{
public:

    OSDFilter(QObject* parent = nullptr);

    bool isSupported(VideoFilterContext::Type ct) const
    {
        return ct == (VideoFilterContext::QtPainter || ct == VideoFilterContext::X11);
    }

protected:

    void process(Statistics* statistics, VideoFrame* frame);
};

} // namespace QtAVPlayer

#endif // QTAV_PLAYER_OSDFILTER_H
