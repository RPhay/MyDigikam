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
#include "qimageloader.h"

namespace DigikamDImgQImagePlugin
{

class Q_DECL_HIDDEN QImageDImgPlugin::Private
{
public:

    explicit Private()
    {
    }

};

QImageDImgPlugin::QImageDImgPlugin(QObject* const parent)
    : DPluginDImg(parent),
      d(new Private)
{
}

QImageDImgPlugin::~QImageDImgPlugin()
{
    delete d;
}

QString QImageDImgPlugin::name() const
{
    return QString::fromUtf8("QImage based DImg loader");
}

QString QImageDImgPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon QImageDImgPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("digikam"));
}

QString QImageDImgPlugin::description() const
{
    return QString::fromUtf8("A DImg image loader based on QImage plugins");
}

QString QImageDImgPlugin::details() const
{
    return QString::fromUtf8("<p>This plugin permit to load and save image with DImg using "
                             "QImage plugins</p>"
    );
}

QList<DPluginAuthor> QImageDImgPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2019"))
            ;
}

void QImageDImgPlugin::setup(QObject* const /*parent*/)
{
    // Nothing to do
}

QString QImageDImgPlugin::loaderName() const
{
    return QLatin1String("QIMAGE");
}

QString QImageDImgPlugin::typeMimes() const
{
    return QLatin1String("");
}

bool QImageDImgPlugin::isSupported(const QString& filePath) const
{
    return false;
}

DImgLoader* QImageDImgPlugin::loader(DImg* const image) const
{
    return new QImageLoader(image);
}

QWidget* QImageDImgPlugin::settingsWidget() const
{
    return nullptr;
}

} // namespace DigikamDImgQImagePlugin
