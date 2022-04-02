/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-18
 * Description : JXL image Converter batch tool.
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "converttojxl.h"

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

namespace DigikamBqmConvertToJxlPlugin
{

ConvertToJXL::ConvertToJXL(QObject* const parent)
    : BatchTool(QLatin1String("ConvertToJXL"), ConvertTool, parent),
      m_changeSettings(true)
{
}

ConvertToJXL::~ConvertToJXL()
{
}

BatchTool* ConvertToJXL::clone(QObject* const parent) const
{
    return new ConvertToJXL(parent);
}

void ConvertToJXL::registerSettingsWidget()
{
    DImgLoaderSettings* const JXLBox = DPluginLoader::instance()->exportWidget(QLatin1String("JXL"));

    if (JXLBox)
    {
        connect(JXLBox, SIGNAL(signalSettingsChanged()),
                this, SLOT(slotSettingsChanged()));
    }

    m_settingsWidget = JXLBox;

    BatchTool::registerSettingsWidget();
}

BatchToolSettings ConvertToJXL::defaultSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(QLatin1String("ImageViewer Settings"));
    int compression           = group.readEntry(QLatin1String("JXLCompression"), 75);
    bool lossLessCompression  = group.readEntry(QLatin1String("JXLLossLess"),    true);
    BatchToolSettings settings;
    settings.insert(QLatin1String("quality"),  compression);
    settings.insert(QLatin1String("lossless"), lossLessCompression);

    return settings;
}

void ConvertToJXL::slotAssignSettings2Widget()
{
    m_changeSettings                 = false;
    DImgLoaderSettings* const JXLBox = dynamic_cast<DImgLoaderSettings*>(m_settingsWidget);

    if (JXLBox)
    {
        DImgLoaderPrms set;
        set.insert(QLatin1String("quality"),  settings()[QLatin1String("quality")].toInt());
        set.insert(QLatin1String("lossless"), settings()[QLatin1String("lossless")].toBool());
        JXLBox->setSettings(set);
    }

    m_changeSettings                 = true;
}

void ConvertToJXL::slotSettingsChanged()
{
    if (m_changeSettings)
    {
        DImgLoaderSettings* const JXLBox = dynamic_cast<DImgLoaderSettings*>(m_settingsWidget);

        if (JXLBox)
        {
            BatchToolSettings settings;
            settings.insert(QLatin1String("quality"),  JXLBox->settings()[QLatin1String("quality")].toInt());
            settings.insert(QLatin1String("lossless"), JXLBox->settings()[QLatin1String("lossless")].toBool());
            BatchTool::slotSettingsChanged(settings);
        }
    }
}

QString ConvertToJXL::outputSuffix() const
{
    return QLatin1String("jxl");
}

bool ConvertToJXL::toolOperations()
{
    if (!loadToDImg())
    {
        return false;
    }

    bool lossless = settings()[QLatin1String("lossless")].toBool();
    image().setAttribute(QLatin1String("quality"), lossless ? 100 : settings()[QLatin1String("quality")].toInt());

    return (savefromDImg());
}

} // namespace DigikamBqmConvertToJxlPlugin
