/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-03-06
 * Description : a Brightness/Contrast/Gamma image filter.
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010      by Martin Klapetek <martin dot klapetek at gmail dot com>
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

#include "bcgcontainer.h"

// Local includes

#include "dimg.h"
#include "dimgthreadedfilter.h"

namespace Digikam
{

BCGContainer::BCGContainer()
  : channel    (LuminosityChannel),
    brightness (0.0),
    contrast   (0.0),
    gamma      (1.0)
{
}

bool BCGContainer::isDefault() const
{
    return (*this == BCGContainer());
}

bool BCGContainer::operator==(const BCGContainer& other) const
{
    return (
            (channel    == other.channel)    &&
            (brightness == other.brightness) &&
            (contrast   == other.contrast)   &&
            (gamma      == other.gamma)
           );
}

void BCGContainer::writeToFilterAction(FilterAction& action, const QString& prefix) const
{
    action.addParameter(prefix + QLatin1String("channel"),    channel);
    action.addParameter(prefix + QLatin1String("brightness"), brightness);
    action.addParameter(prefix + QLatin1String("contrast"),   contrast);
    action.addParameter(prefix + QLatin1String("gamma"),      gamma);
}

BCGContainer BCGContainer::fromFilterAction(const FilterAction& action, const QString& prefix)
{
    BCGContainer settings;
    settings.channel    = action.parameter(prefix + QLatin1String("channel"),    settings.channel);
    settings.brightness = action.parameter(prefix + QLatin1String("brightness"), settings.brightness);
    settings.contrast   = action.parameter(prefix + QLatin1String("contrast"),   settings.contrast);
    settings.gamma      = action.parameter(prefix + QLatin1String("gamma"),      settings.gamma);

    return settings;
}

} // namespace Digikam
