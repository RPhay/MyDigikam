/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-05-13
 * Description : ExifTool thread for the ExifTool process
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2022      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

Q_SIGNALS:

    void exifToolProcessStarted();

protected:

    /**
     * Main thread loop.
     */
    void run() override;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_THREAD_H
