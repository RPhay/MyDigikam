/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-20
 * Description : QImage DImg plugin.
 *
 * Copyright (C) 2019      by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Lua script inspired from Darktable Gimp plugin
 * Copyright (C) 2015-2017 by Tobias Ellinghaus
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

#include "dimgqimageplugin.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimgqimageloader.h"

namespace DigikamQImageDImgPlugin
{

class Q_DECL_HIDDEN DImgQImagePlugin::Private
{
public:

    explicit Private()
    {
    }

};

DImgQImagePlugin::DImgQImagePlugin(QObject* const parent)
    : DPluginDImg(parent),
      d(new Private)
{
}

DImgQImagePlugin::~DImgQImagePlugin()
{
    delete d;
}

QString DImgQImagePlugin::name() const
{
    return QString::fromUtf8("QImage based DImg loader");
}

QString DImgQImagePlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgQImagePlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("digikam"));
}

QString DImgQImagePlugin::description() const
{
    return QString::fromUtf8("A DImg image loader based on QImage plugins");
}

QString DImgQImagePlugin::details() const
{
    return QString::fromUtf8("<p>This plugin permit to load and save image with DImg using "
                             "QImage plugins</p>"
    );
}

QList<DPluginAuthor> DImgQImagePlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            ;
}

void DImgQImagePlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString DImgQImagePlugin::loaderName() const
{
    return QLatin1String("QIMAGE");
}

QString DImgQImagePlugin::typeMimes() const
{
    return QLatin1String("");
}

bool DImgQImagePlugin::isSupported(const QString& filePath) const
{
    return false;
}

DImgLoader* DImgQImagePlugin::loader(DImg* const image) const
{
    return new DImgQImageLoader(image);
}

} // namespace DigikamQImageDImgPlugin
