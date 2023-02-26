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

#ifndef AV_PLAYER_OSD_H
#define AV_PLAYER_OSD_H

// Qt includes

#include <QPoint>
#include <QFont>

// Local includes

#include "QtAV_Global.h"
#include "QtAV_Statistics.h"

using namespace QtAV;

namespace AVPlayer
{

class OSD
{
public:

    enum ShowType
    {
        ShowCurrentTime         = 1,
        ShowCurrentAndTotalTime = 1 << 1,
        ShowRemainTime          = 1 << 2,
        ShowPercent             = 1 << 3,
        ShowNone
    };

public:

    OSD();
    virtual ~OSD();

    void setShowType(ShowType type);
    ShowType showType()          const;
    void useNextShowType();
    bool hasShowType(ShowType t) const;
    QString text(Statistics* statistics);

protected:

    ShowType mShowType;
    int      mSecsTotal;

private:

    Q_DISABLE_COPY(OSD);
};

} // namespace AVPlayer

#endif // AV_PLAYER_OSD_H
