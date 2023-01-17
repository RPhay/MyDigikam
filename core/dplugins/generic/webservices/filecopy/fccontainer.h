/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-07-05
 * Description : File copy settings container.
 *
 * SPDX-FileCopyrightText: 2020 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
        sidecars             (false),
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

    bool            sidecars;
    bool            overwrite;
    bool            albumPath;
    bool            removeMetadata;
    bool            changeImageProperties;
};

} // namespace DigikamGenericFileCopyPlugin

#endif // DIGIKAM_FC_CONTAINER_H
