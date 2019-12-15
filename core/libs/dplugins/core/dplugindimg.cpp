/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-19
 * Description : digiKam plugin definition for DImg image loader.
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

#include "dplugindimg.h"

// Qt includes

#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_version.h"
#include "digikam_debug.h"

namespace Digikam
{

DPluginDImg::DPluginDImg(QObject* const parent)
    : DPlugin(parent)
{
}

DPluginDImg::~DPluginDImg()
{
}

QMap<QString, QString> DPluginDImg::extraAboutData() const
{
    QMap<QString, QString> map;
    QMimeDatabase mimeDb;

    foreach (const QString& ext, typeMimes().split(QLatin1Char(' ')))
    {
        if (!ext.isEmpty())
        {
            map.insert(ext, mimeDb.mimeTypeForFile(QString::fromLatin1("foo.%1").arg(ext)).comment());
        }
    }

    return map;
};

QString DPluginDImg::extraAboutDataTitle() const
{
    return i18n("Type-Mimes");
};

} // namespace Digikam
