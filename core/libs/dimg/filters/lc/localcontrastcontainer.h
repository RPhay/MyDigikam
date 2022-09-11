/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-09
 * Description : Local Contrast settings container
 *               LDR ToneMapper zynaddsubfx.sourceforge.net/other/tonemapping
 *
 * SPDX-FileCopyrightText: 2009      by Nasca Octavian Paul <zynaddsubfx at yahoo dot com>
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LOCAL_CONTRAST_CONTAINER_H
#define DIGIKAM_LOCAL_CONTRAST_CONTAINER_H

#define TONEMAPPING_MAX_STAGES 4

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT LocalContrastContainer
{

public:

    explicit LocalContrastContainer();
    ~LocalContrastContainer();

    double getPower(int nstage)  const;
    double getBlur(int nstage)   const;

public:

    bool stretchContrast;

    int  lowSaturation;
    int  highSaturation;
    int  functionId;

    struct
    {
        bool  enabled;
        double power;
        double blur;
    }
    stage[TONEMAPPING_MAX_STAGES];
};

} // namespace Digikam

#endif // DIGIKAM_LOCAL_CONTRAST_CONTAINER_H
