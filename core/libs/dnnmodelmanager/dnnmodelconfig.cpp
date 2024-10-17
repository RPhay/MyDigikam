/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2024-10-13
 * Description : digiKam DNNModelConfig for generic OpenCV config
 *
 * SPDX-FileCopyrightText: 2024 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dnnmodelconfig.h"

// Qt includes

#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

bool DNNModelConfig::loadModel()
{
    if (checkFilename())
    {
        // do nothing.  Config don't have a cv::dnn::Net pointer

        return modelLoaded = true;  // set modelLoaded and return true
    }

    return false;
}

} // namespace Digikam
