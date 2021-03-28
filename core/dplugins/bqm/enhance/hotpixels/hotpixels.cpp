/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-07
 * Description : Hot Pixels Fixer batch tool.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "hotpixels.h"

// Qt includes

#include <QWidget>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dimg.h"
#include "hotpixelcontainer.h"
#include "hotpixelfixer.h"
#include "hotpixelprops.h"

namespace DigikamBqmHotPixelsPlugin
{

HotPixels::HotPixels(QObject* const parent)
    : BatchTool(QLatin1String("HotPixels"), EnhanceTool, parent),
      m_settingsView(nullptr)
{
}

HotPixels::~HotPixels()
{
}

BatchTool* HotPixels::clone(QObject* const parent) const
{
    return new HotPixels(parent);
}

void HotPixels::registerSettingsWidget()
{
    m_settingsWidget = new QWidget;
    m_settingsView   = new HotPixelSettings(m_settingsWidget);

    connect(m_settingsView, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings HotPixels::defaultSettings()
{
    BatchToolSettings prm;
    HotPixelContainer defaultPrm = m_settingsView->defaultSettings();

    prm.insert(QLatin1String("BlackFrameUrl"), defaultPrm.blackFrameUrl);
    prm.insert(QLatin1String("HotPixelsList"), HotPixelProps::toStringList(defaultPrm.hotPixelsList));
    prm.insert(QLatin1String("FilterMethod"),  (int)defaultPrm.filterMethod);

    return prm;
}

void HotPixels::slotAssignSettings2Widget()
{
    HotPixelContainer prm;
    prm.blackFrameUrl = settings()[QLatin1String("BlackFrameUrl")].toUrl();
    prm.hotPixelsList = HotPixelProps::fromStringList(settings()[QLatin1String("HotPixelsList")].toStringList());
    prm.filterMethod  = (HotPixelContainer::InterpolationMethod)settings()[QLatin1String("FilterMethod")].toInt();
    m_settingsView->setSettings(prm);
}

void HotPixels::slotSettingsChanged()
{
    BatchToolSettings prm;
    HotPixelContainer currentPrm = m_settingsView->settings();

    prm.insert(QLatin1String("BlackFrameUrl"), currentPrm.blackFrameUrl);
    prm.insert(QLatin1String("HotPixelsList"), HotPixelProps::toStringList(currentPrm.hotPixelsList));
    prm.insert(QLatin1String("FilterMethod"),  (int)currentPrm.filterMethod);

    BatchTool::slotSettingsChanged(prm);
}

bool HotPixels::toolOperations()
{
    if (!loadToDImg())
    {
        return false;
    }

    HotPixelContainer prm;

    prm.blackFrameUrl = settings()[QLatin1String("BlackFrameUrl")].toUrl();
    prm.hotPixelsList = HotPixelProps::fromStringList(settings()[QLatin1String("HotPixelsList")].toStringList());
    prm.filterMethod  = (HotPixelContainer::InterpolationMethod)settings()[QLatin1String("FilterMethod")].toInt();

    HotPixelFixer hpf(&image(), nullptr, prm);
    applyFilter(&hpf);

    return (savefromDImg());
}

} // namespace DigikamBqmHotPixelsPlugin
