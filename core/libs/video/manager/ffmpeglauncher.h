/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-07-04
 * Description : FFmpeg launcher to encode frames as video
 *
 * SPDX-FileCopyrightText: 2017-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FFMPEG_LAUNCHER_H
#define DIGIKAM_FFMPEG_LAUNCHER_H

// Local includes

#include "processlauncher.h"
#include "vidslidesettings.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT FFmpegLauncher : public ProcessLauncher
{
    Q_OBJECT

public:

    explicit FFmpegLauncher(QObject* const parent);
    ~FFmpegLauncher() override;

    void setSettings(VidSlideSettings* const settings);
    void encodeFrames();

private:

    VidSlideSettings* m_settings = nullptr;
};

} // namespace Digikam

#endif // DIGIKAM_FFMPEG_LAUNCHER_H
