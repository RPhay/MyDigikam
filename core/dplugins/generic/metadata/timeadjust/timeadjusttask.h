/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-12-31
 * Description : time adjust actions using threads.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2018-2021 by Maik Qualmann <metzpinguin at gmail dot com>
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

#ifndef DIGIKAM_TIME_ADJUST_TASK_H
#define DIGIKAM_TIME_ADJUST_TASK_H

// Qt includes

#include <QUrl>

// Local includes

#include "timeadjustthread.h"
#include "actionthreadbase.h"
#include "timeadjustcontainer.h"

using namespace Digikam;

namespace DigikamGenericTimeAdjustPlugin
{

class TimeAdjustTask : public ActionJob
{
    Q_OBJECT

public:

    explicit TimeAdjustTask(const QUrl& url, TimeAdjustThread* const thread);
    ~TimeAdjustTask() override;

    void setSettings(const TimeAdjustContainer& settings);
    void setItemsMap(const QMap<QUrl, int>& itemsMap);

Q_SIGNALS:

    void signalProcessStarted(const QUrl&);
    void signalDateTimeForUrl(const QUrl&, const QDateTime&, bool);
    void signalProcessEnded(const QUrl&, const QDateTime&, const QDateTime&, int);

protected:

    void run() override;

private:

    // Disable
    TimeAdjustTask(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

// ------------------------------------------------------------------

class TimePreviewTask : public ActionJob
{
    Q_OBJECT

public:

    explicit TimePreviewTask(const QUrl& url, TimeAdjustThread* const thread);
    ~TimePreviewTask() override;

    void setSettings(const TimeAdjustContainer& settings);
    void setItemsList(const QMap<QUrl, int>& itemsMap);

Q_SIGNALS:

    void signalPreviewReady(const QUrl&, const QDateTime&, const QDateTime&);

protected:

    void run() override;

private:

    // Disable
    TimePreviewTask(QObject*) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericTimeAdjustPlugin

#endif // DIGIKAM_TIME_ADJUST_TASK
