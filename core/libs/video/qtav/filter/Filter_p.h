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

class DIGIKAM_EXPORT FilterPrivate : public DPtrPrivate<Filter>
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

class DIGIKAM_EXPORT VideoFilterPrivate : public FilterPrivate
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

class DIGIKAM_EXPORT AudioFilterPrivate : public FilterPrivate
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
