/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-05-13
 * Description : ExifTool thread for the ExifTool process
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2022      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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

    exec();

    if (ExifToolProcess::isCreated())
    {
        delete ExifToolProcess::internalPtr;
    }
}

} // namespace Digikam
