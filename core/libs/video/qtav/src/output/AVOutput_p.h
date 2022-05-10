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

#ifndef QTAV_AV_OUTPUT_P_H
#define QTAV_AV_OUTPUT_P_H

#include "AVOutput.h"

// Qt includes

#include <QList>
#include <QMutex>
#include <QWaitCondition>

// Local includes

#include "QtAV_Global.h"

namespace QtAV
{

class AVOutput;
class AVDecoder;
class Filter;
class VideoFilterContext;
class Statistics;
class OutputSet;

class QTAV_PRIVATE_EXPORT AVOutputPrivate : public DPtrPrivate<AVOutput>
{
public:

    AVOutputPrivate()
      : paused(false)
      , available(true)
      , statistics(nullptr)
      , filter_context(nullptr)
    {
    }

    virtual ~AVOutputPrivate();

    bool                paused;
    bool                available;
    QMutex              mutex;              // pause
    QWaitCondition      cond;               // pause

    // paintEvent is in main thread, copy it(only dynamic information) is better.
    // the static data are copied from AVPlayerCore when open

    Statistics*         statistics;         // do not own the ptr. just use AVPlayerCore's statistics ptr
    VideoFilterContext* filter_context;     // create internally by the renderer with correct type
    QList<Filter*>      filters;
    QList<Filter*>      pending_uninstall_filters;
    QList<OutputSet*>   output_sets;
};

} // namespace QtAV

#endif // QTAV_AV_OUTPUT_P_H
