/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "MathHelper.h"

#ifdef Q_CC_MSVC

#   include <float.h>

qreal msvc_asinh(qreal x)
{
    if (_isnan(x))
    {
        errno = EDOM;
        return x;
    }

    return (log(x + sqrt(x * x + 1.0)));
}

qreal msvc_atanh(qreal x)
{
    return (0.5 * log((1.0 + x) / (1.0 - x)));
}

#endif  // Q_CC_MSVC

