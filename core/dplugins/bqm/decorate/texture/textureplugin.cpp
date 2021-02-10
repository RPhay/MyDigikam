/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-13
 * Description : a BQM plugin to add texture
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

#include "textureplugin.h"

// Qt includes

#include <QPointer>
#include <QString>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "texture.h"

namespace DigikamBqmTexturePlugin
{

TexturePlugin::TexturePlugin(QObject* const parent)
    : DPluginBqm(parent)
{
}

TexturePlugin::~TexturePlugin()
{
}

QString TexturePlugin::name() const
{
    return i18n("Add Texture");
}

QString TexturePlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon TexturePlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("texture"));
}

QString TexturePlugin::description() const
{
    return i18n("A tool to apply a texture over images");
}

QString TexturePlugin::details() const
{
    return i18n("<p>This Batch Queue Manager tool can apply a texture over images.</p>");
}

QList<DPluginAuthor> TexturePlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Marcel Wiesweg"),
                             QString::fromUtf8("marcel dot wiesweg at gmx dot de"),
                             QString::fromUtf8("(C) 2006-2010"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2005-2021"))
            ;
}

void TexturePlugin::setup(QObject* const parent)
{
    Texture* const tool = new Texture(parent);
    tool->setPlugin(this);

    addTool(tool);
}

} // namespace DigikamBqmTexturePlugin
