/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#ifndef QAV_AUDIOTHREAD_H
#define QAV_AUDIOTHREAD_H

#include "AVThread.h"

namespace QtAV {

class AudioDecoder;
class AudioFrame;
class AudioThreadPrivate;
class AudioThread : public AVThread
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(AudioThread)
public:
    explicit AudioThread(QObject *parent = 0);

protected:
    void applyFilters(AudioFrame& frame);
    virtual void run();
};

} //namespace QtAV
#endif // QAV_AUDIOTHREAD_H
