/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-02
 * Description : a BQM plugin to convert to HEIF.
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

#include "converttoheifplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "converttoheif.h"

namespace DigikamBqmConvertToHeifPlugin
{

ConvertToHeifPlugin::ConvertToHeifPlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

ConvertToHeifPlugin::~ConvertToHeifPlugin()
{
}

QString ConvertToHeifPlugin::name() const
{
    return i18n("Convert To HEIF");
}

QString ConvertToHeifPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon ConvertToHeifPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-generic"));
}

QString ConvertToHeifPlugin::description() const
{
    return i18n("A tool to convert images to HEIF format");
}

QString ConvertToHeifPlugin::details() const
{
    return i18n("<p>This Batch Queue Manager tool can convert images data to HEIF format.</p>"
                "<p>High Efficiency Image File Format (HEIF), also known as High Efficiency Image Coding (HEIC), "
                "is a file format for individual images and image sequences. It was developed by the "
                "Moving Picture Experts Group (MPEG) and it claims that twice as much information can be "
                "stored in a HEIF image as in a JPEG image of the same size, resulting in a better quality image. "
                "HEIF also supports animation, and is capable of storing more information than an animated GIF "
                "at a small fraction of the size.</p>"
                "<p>See <a href='https://en.wikipedia.org/wiki/High_Efficiency_Image_File_Format'>"
                "High Efficiency Image File Format</a> for details.</p>");
}

QList<DPluginAuthor> ConvertToHeifPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            ;
}

void ConvertToHeifPlugin::setup(QObject* const parent)
{
    ConvertToHEIF* const tool = new ConvertToHEIF(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmConvertToHeifPlugin
