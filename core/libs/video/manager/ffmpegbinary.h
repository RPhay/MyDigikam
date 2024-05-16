/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : Autodetect FFmpeg binary program
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FFMPEG_BINARY_H
#define DIGIKAM_FFMPEG_BINARY_H

// Local includes

#include "dbinaryiface.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FFmpegBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit FFmpegBinary(QObject* const parent = nullptr);
    ~FFmpegBinary() override = default;

    static QString ffmpegToolBin();
};

} // namespace Digikam

#endif // DIGIKAM_FFMPEG_BINARY_H
