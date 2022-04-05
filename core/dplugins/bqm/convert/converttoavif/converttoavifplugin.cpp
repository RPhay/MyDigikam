/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-04-03
 * Description : a BQM plugin to convert to AVIF.
 *
 * Copyright (C) 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "converttoavifplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "converttoavif.h"

namespace DigikamBqmConvertToAvifPlugin
{

ConvertToAvifPlugin::ConvertToAvifPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

ConvertToAvifPlugin::~ConvertToAvifPlugin()
{
}

QString ConvertToAvifPlugin::name() const
{
    return i18nc("@title", "Convert To AVIF");
}

QString ConvertToAvifPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ConvertToAvifPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-generic"));
}

QString ConvertToAvifPlugin::description() const
{
    return i18nc("@info", "A tool to convert images to AVIF format");
}

QString ConvertToAvifPlugin::details() const
{
    return xi18nc("@info", "<p>This Batch Queue Manager tool can convert images data to AVIF format.</p>"
                  "<p>AV1 Image File Format is an image file format specification for storing images or image sequences compressed with AV1 "
                  "in the ISOBMFF container format.</p>"
                  "<p>See details about this format from <a href='https://en.wikipedia.org/wiki/AVIF'>this page</a>.</p>");
}

QList<DPluginAuthor> ConvertToAvifPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2022"))
            ;
}

void ConvertToAvifPlugin::setup(QObject* const parent)
{
    ConvertToAVIF* const tool = new ConvertToAVIF(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmConvertToAvifPlugin
