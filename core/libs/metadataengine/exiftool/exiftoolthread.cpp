/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-05-13
 * Description : ExifTool thread for the ExifTool process
 *
 * SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2022      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "exiftoolthread.h"

// Local includes

#include "digikam_debug.h"
#include "exiftoolprocess.h"

namespace Digikam
{

ExifToolThread::ExifToolThread(QObject* const parent)
    : QThread(parent)
{
}

ExifToolThread::~ExifToolThread()
{
    quit();
    wait();
}

void ExifToolThread::run()
{
    ExifToolProcess* const proc = ExifToolProcess::instance();
    proc->moveToThread(this);
    proc->initExifTool();

    Q_EMIT exifToolProcessStarted();

    exec();

    if (
        ExifToolProcess::isCreated()                 &&
        (ExifToolProcess::instance()->thread() == this)
       )
    {
        ExifToolProcess::instance()->shutDownExifTool();
    }
}

} // namespace Digikam

#include "moc_exiftoolthread.cpp"
