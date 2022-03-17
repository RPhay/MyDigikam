/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-03-17
 * Description : Methods to list FFMPEG features.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FFMPEG_CONFIG_HELPER_H
#define DIGIKAM_FFMPEG_CONFIG_HELPER_H

// Qt includes

#include <QString>
#include <QStringList>
#include <QMap>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

typedef QMap<QString, QStringList> FFMpegProperties;

class DIGIKAM_EXPORT FFMpegConfigHelper
{
public:

    /**
     * Return a map of video Codec Name with a list of properties:
     *    - Codecs description.
     *    - Read support.
     *    - Write support.
     */
    static FFMpegProperties getVideoCodecsProperties();

    /**
     * Return a map of audio Codec Name with a list of properties:
     *    - Codecs description.
     *    - Read support.
     *    - Write support.
     */
    static FFMpegProperties getAudioCodecsProperties();

    /**
     * Return a file extensions supported a list of properties:
     *    - Format description.
     */
    static FFMpegProperties getExtensionsProperties();

private:

    FFMpegConfigHelper()  = delete;
    ~FFMpegConfigHelper() = delete;

    Q_DISABLE_COPY(FFMpegConfigHelper)
};

} // namespace Digikam

#endif // DIGIKAM_FFMPEG_CONFIG_HELPER_H
