/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-03-17
 * Description : Methods to list FFMPEG features.
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
