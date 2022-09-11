/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Face scan settings
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facescansettings.h"

namespace Digikam
{

FaceScanSettings::FaceScanSettings()
    : wholeAlbums           (false),
      useFullCpu            (false),
      useYoloV3             (false),
      accuracy              (0.7),
      task                  (Detect),
      alreadyScannedHandling(Skip)
{
}

FaceScanSettings::~FaceScanSettings()
{
}

} // namespace Digikam
