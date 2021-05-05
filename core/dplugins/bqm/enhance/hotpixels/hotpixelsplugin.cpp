/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-07
 * Description : a BQM plugin to fix hot pixels
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "hotpixelsplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "hotpixels.h"

namespace DigikamBqmHotPixelsPlugin
{

HotPixelsPlugin::HotPixelsPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

HotPixelsPlugin::~HotPixelsPlugin()
{
}

QString HotPixelsPlugin::name() const
{
    return i18n("Hot Pixels");
}

QString HotPixelsPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon HotPixelsPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("hotpixels"));
}

QString HotPixelsPlugin::description() const
{
    return i18n("A tool to fix hot pixels");
}

QString HotPixelsPlugin::details() const
{
    return i18n("<p>This Batch Queue Manager tool can fix hot pixels in images.</p>");
}

QList<DPluginAuthor> HotPixelsPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Unai Garro"),
                             QString::fromUtf8("ugarro at users dot sourceforge dot net"),
                             QString::fromUtf8("(C) 2005-2006"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2021"))
            ;
}

void HotPixelsPlugin::setup(QObject* const parent)
{
    HotPixels* const tool = new HotPixels(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmHotPixelsPlugin
