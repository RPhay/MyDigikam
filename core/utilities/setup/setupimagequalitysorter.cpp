/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality setup page
 *
 * Copyright (C) 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupimagequalitysorter.h"

// Local includes

#include "imagequalitysettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupImageQualitySorter::Private
{
public:

    explicit Private()
      : settingsWidget(nullptr)
    {
    }

    ImageQualitySettings* settingsWidget;
};

// --------------------------------------------------------

SetupImageQualitySorter::SetupImageQualitySorter(QWidget* const parent)
    : QScrollArea(parent),
      d(new Private)
{
    d->settingsWidget = new ImageQualitySettings(viewport());
    setWidget(d->settingsWidget);
    setWidgetResizable(true);

    d->settingsWidget->readSettings();
}

SetupImageQualitySorter::~SetupImageQualitySorter()
{
    delete d;
}

void SetupImageQualitySorter::applySettings()
{
    d->settingsWidget->applySettings();
}

void SetupImageQualitySorter::readSettings()
{
    d->settingsWidget->readSettings();
}

ImageQualityContainer SetupImageQualitySorter::getImageQualityContainer()
{
    return d->settingsWidget->getImageQualityContainer();
}

} // namespace Digikam
