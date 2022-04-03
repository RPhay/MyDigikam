/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-02
 * Description : HEIF image Converter batch tool.
 *
 * Copyright (C) 2020-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "converttoheif.h"

// Qt includes

#include <QFileInfo>
#include <QWidget>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <ksharedconfig.h>

// Local includes

#include "dimg.h"
#include "dpluginloader.h"

namespace DigikamBqmConvertToHeifPlugin
{

ConvertToHEIF::ConvertToHEIF(QObject* const parent)
    : BatchTool(QLatin1String("ConvertToHEIF"), ConvertTool, parent),
      m_changeSettings(true)
{
}

ConvertToHEIF::~ConvertToHEIF()
{
}

BatchTool* ConvertToHEIF::clone(QObject* const parent) const
{
    return new ConvertToHEIF(parent);
}

void ConvertToHEIF::registerSettingsWidget()
{
    DImgLoaderSettings* const HEIFBox = DPluginLoader::instance()->exportWidget(QLatin1String("HEIF"));

    connect(HEIFBox, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    m_settingsWidget            = HEIFBox;

    BatchTool::registerSettingsWidget();
}

BatchToolSettings ConvertToHEIF::defaultSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("ImageViewer Settings"));
    int compression           = group.readEntry(QLatin1String("HEIFCompression"), 3);
    bool lossLessCompression  = group.readEntry(QLatin1String("HEIFLossLess"),    true);
    BatchToolSettings settings;
    settings.insert(QLatin1String("quality"),  compression);
    settings.insert(QLatin1String("lossless"), lossLessCompression);

    return settings;
}

void ConvertToHEIF::slotAssignSettings2Widget()
{
    m_changeSettings = false;

    DImgLoaderSettings* const HEIFBox = dynamic_cast<DImgLoaderSettings*>(m_settingsWidget);

    if (HEIFBox)
    {
        DImgLoaderPrms set;
        set.insert(QLatin1String("quality"),  settings()[QLatin1String("quality")].toInt());
        set.insert(QLatin1String("lossless"), settings()[QLatin1String("lossless")].toBool());
        HEIFBox->setSettings(set);
    }

    m_changeSettings = true;
}

void ConvertToHEIF::slotSettingsChanged()
{
    if (m_changeSettings)
    {
        DImgLoaderSettings* const HEIFBox = dynamic_cast<DImgLoaderSettings*>(m_settingsWidget);

        if (HEIFBox)
        {
            BatchToolSettings settings;
            settings.insert(QLatin1String("quality"),  HEIFBox->settings()[QLatin1String("quality")].toInt());
            settings.insert(QLatin1String("lossless"), HEIFBox->settings()[QLatin1String("lossless")].toBool());
            BatchTool::slotSettingsChanged(settings);
        }
    }
}

QString ConvertToHEIF::outputSuffix() const
{
    return QLatin1String("heif");
}

bool ConvertToHEIF::toolOperations()
{
    if (!loadToDImg())
    {
        return false;
    }

    bool lossless = settings()[QLatin1String("lossless")].toBool();
    image().setAttribute(QLatin1String("quality"), lossless ? 0 : settings()[QLatin1String("quality")].toInt());

    return (savefromDImg());
}

} // namespace DigikamBqmConvertToHeifPlugin
