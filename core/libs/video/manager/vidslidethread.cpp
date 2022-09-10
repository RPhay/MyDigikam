/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "vidslidethread.h"

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"

namespace Digikam
{

VidSlideThread::VidSlideThread(QObject* const parent)
    : ActionThreadBase(parent)
{
    setObjectName(QLatin1String("VidSlideThread"));
}

VidSlideThread::~VidSlideThread()
{
    cancel();
    wait();
}

void VidSlideThread::processStream(VidSlideSettings* const settings)
{
    ActionJobCollection collection;

    VidSlideTask* const t = new VidSlideTask(settings);

    connect(t, SIGNAL(signalProgress(int)),
            this, SIGNAL(signalProgress(int)));

    connect(t, SIGNAL(signalDone(bool)),
            this, SIGNAL(signalDone(bool)));

    connect(t, SIGNAL(signalMessage(QString,bool)),
            this, SIGNAL(signalMessage(QString,bool)));

    collection.insert(t, 0);

    appendJobs(collection);
}

} // namespace Digikam
