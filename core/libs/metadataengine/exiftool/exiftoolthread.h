/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-05-13
 * Description : ExifTool thread for the ExifTool process
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2022      by Maik Qualmann <metzpinguin dot at gmail dot com>
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

#ifndef DIGIKAM_EXIFTOOL_THREAD_H
#define DIGIKAM_EXIFTOOL_THREAD_H

// Qt includes

#include <QThread>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT ExifToolThread : public QThread
{
    Q_OBJECT

public:

    explicit ExifToolThread(QObject* const parent);
    ~ExifToolThread() override;

protected:

    /**
     * Main thread loop.
     */
    void run() override;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_THREAD_H
