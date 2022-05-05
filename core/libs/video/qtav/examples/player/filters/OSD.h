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

#ifndef QTAV_OSD_H
#define QTAV_OSD_H

#include "QtAV_Global.h"
#include <QPoint>
#include <QFont>

namespace QtAV {

class Statistics;
class OSD
{
public:
    enum ShowType {
        ShowCurrentTime = 1,
        ShowCurrentAndTotalTime = 1<<1,
        ShowRemainTime = 1<<2,
        ShowPercent = 1<<3,
        ShowNone
    };

    OSD();
    virtual ~OSD();
    void setShowType(ShowType type);
    ShowType showType() const;
    void useNextShowType();
    bool hasShowType(ShowType t) const;
    QString text(Statistics* statistics);
protected:
    ShowType mShowType;
    int mSecsTotal;
};

}//namespace QtAV
#endif // QTAV_OSD_H
