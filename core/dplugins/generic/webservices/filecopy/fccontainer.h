/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-07-05
 * Description : File copy settings container.
 *
 * Copyright (C) 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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

#ifndef DIGIKAM_FC_CONTAINER_H
#define DIGIKAM_FC_CONTAINER_H

// Qt includes

#include <QUrl>

// Local includes

#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericFileCopyPlugin
{

class FCContainer
{

public:

    enum FileCopyType
    {
        CopyFile = 0,
        FullSymLink,
        RelativeSymLink
    };

    enum ImageFormat
    {
        JPEG = 0,
        PNG
    };

public:

    FCContainer()
      : iface                (nullptr),
        behavior             (CopyFile),
        imageFormat          (JPEG),
        imageResize          (1024),
        imageCompression     (75),
        overwrite            (false),
        albumPath            (false),
        removeMetadata       (false),
        changeImageProperties(false)
    {
    };

    ~FCContainer()
    {
    };

public:


    DInfoInterface* iface;

    QUrl            destUrl;

    int             behavior;
    int             imageFormat;
    int             imageResize;
    int             imageCompression;

    bool            overwrite;
    bool            albumPath;
    bool            removeMetadata;
    bool            changeImageProperties;
};

} // namespace DigikamGenericFileCopyPlugin

#endif // DIGIKAM_FC_CONTAINER_H
