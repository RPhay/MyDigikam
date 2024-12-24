/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-11-10
 * Description : Performs face detection and recognition
 *
 * SPDX-FileCopyrightText: 2024      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes
#include <QIcon>

// local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT MLPipelinePackageFoundation
{
public:

    MLPipelinePackageFoundation()                           = default;
    virtual ~MLPipelinePackageFoundation()                  = default;

    quint64 size            = 0;
    QIcon   thumbnailIcon;

private:

    MLPipelinePackageFoundation(MLPipelinePackageFoundation&) = delete;

};

}