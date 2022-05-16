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

#ifndef QTAV_FILTER_P_H
#define QTAV_FILTER_P_H

#include "Filter.h"

// Local includes

#include "QtAV_Global.h"

namespace QtAV
{

class Filter;
class VideoFilterContext;
class Statistics;

class QTAV_PRIVATE_EXPORT FilterPrivate : public DPtrPrivate<Filter>
{
public:

    FilterPrivate()
      : enabled        (true),
        owned_by_target(false)
    {
    }

    virtual ~FilterPrivate()
    {
    }

    bool enabled;
    bool owned_by_target;

private:

    Q_DISABLE_COPY(FilterPrivate);
};

class QTAV_PRIVATE_EXPORT VideoFilterPrivate : public FilterPrivate
{
public:

    VideoFilterPrivate()
      : context(nullptr)
    {
    }

    virtual ~VideoFilterPrivate()
    {
    }

    VideoFilterContext* context; ///< used only when is necessary

private:

    Q_DISABLE_COPY(VideoFilterPrivate);
};

class QTAV_PRIVATE_EXPORT AudioFilterPrivate : public FilterPrivate
{
public:

    AudioFilterPrivate()
    {
    }

    virtual ~AudioFilterPrivate()
    {
    }

private:

    Q_DISABLE_COPY(AudioFilterPrivate);
};

} // namespace QtAV

#endif // QTAV_FILTER_P_H
