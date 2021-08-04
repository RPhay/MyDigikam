/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG stram settings container.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_MJPEG_STREAM_SETTINGS_H
#define DIGIKAM_MJPEG_STREAM_SETTINGS_H

// Qt includes

#include <QList>
#include <QString>
#include <QUrl>

// Local includes

#include "mjpegserver.h"
#include "dinfointerface.h"

class KConfigGroup;

using namespace Digikam;

namespace DigikamGenericMjpegStreamPlugin
{

class MjpegStreamSettings
{

public:

    explicit MjpegStreamSettings();
    ~MjpegStreamSettings();

    /**
     * Helper method to compute urlsList from a set of albms selected by end-users from GUI.
     */
    void setCollectionMap(const MjpegServerMap& map);

    /**
     * Read and write settings in config file between sessions.
     */
    void  readSettings(KConfigGroup& group);
    void  writeSettings(KConfigGroup& group);

public:

    int             port;          ///< IP port to use with MJPEG Server.
    bool            loop;          ///< Image stream as loop.
    int             quality;       ///< Jpeg compression [1...100].
    int             delay;         ///< Delay in seconds between inages.
    QList<QUrl>     urlsList;      ///< Ordered list of list to stream.
    int             outSize;       ///< Output JPEG size ID.

    DInfoInterface* iface;         ///< Plugin host interface to handle item properties.
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_STREAM_SETTINGS_H
