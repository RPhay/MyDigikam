/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-12-15
 * Description : white balance color correction settings container
 *
 * SPDX-FileCopyrightText: 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2008      by Guillaume Castagnino <casta at xwing dot info>
 * SPDX-FileCopyrightText: 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_WB_CONTAINER_H
#define DIGIKAM_WB_CONTAINER_H

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class FilterAction;

class DIGIKAM_EXPORT WBContainer
{

public:

    WBContainer();

    bool   isDefault()                                            const;
    bool   operator==(const WBContainer& other)                   const;

    void   writeToFilterAction(FilterAction& action,
                               const QString& prefix = QString()) const;

    static WBContainer fromFilterAction(const FilterAction& action,
                                        const QString& prefix = QString());

public:

    double black;
    double expositionMain;
    double expositionFine;
    double temperature;
    double green;
    double dark;
    double gamma;
    double saturation;
};

} // namespace Digikam

#endif // DIGIKAM_WB_CONTAINER_H
