/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-19
 * Description : digiKam plugin definition for DImg image loader.
 *
 * Copyright (C) 2019-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

QMap<QString, QStringList> DPluginDImg::extraAboutData() const
{
    QMap<QString, QStringList> map;
    QMimeDatabase mimeDb;

    foreach (const QString& ext, typeMimes().split(QLatin1Char(' ')))
    {
        if (!ext.isEmpty())
        {
            QMimeType mime = mimeDb.mimeTypeForFile(QString::fromLatin1("foo.%1").arg(ext));

            if (mime.name() == QLatin1String("application/octet-stream"))
            {
                map.insert(ext,
                           QStringList() << i18nc("@info: type of image", "%1 image", ext)
                                         << ((canRead(QFileInfo(QString::fromLatin1("foo.%1").arg(ext)), false) != 0) ?
                                             i18nc("@info: can read file format",     "yes") :
                                             i18nc("@info: cannot read file format",  "no"))
                                         << ((canWrite(ext) != 0) ?
                                             i18nc("@info: can write file format",    "yes") :
                                             i18nc("@info: cannot write file format", "no"))
                );
            }
            else
            {
                map.insert(ext,
                           QStringList() << mime.comment()
                                         << ((canRead(QFileInfo(QString::fromLatin1("foo.%1").arg(ext)), false) != 0) ?
                                             i18nc("@info: can read file format",     "yes") :
                                             i18nc("@info: cannot read file format",  "no"))
                                         << ((canWrite(ext) != 0) ?
                                             i18nc("@info: can write file format",    "yes") :
                                             i18nc("@info: cannot write file format", "no"))
                );
            }
        }
    }

    return map;
}

QStringList DPluginDImg::extraAboutDataRowTitles() const
{
    return QStringList() << i18nc("@title: DPlugin properties", "Extension")
                         << i18nc("@title: DPlugin properties", "Description")
                         << i18nc("@title: DPlugin properties", "Read")
                         << i18nc("@title: DPlugin properties", "Write");
}

QString DPluginDImg::extraAboutDataTitle() const
{
    return i18n("Type-Mimes");
}

} // namespace Digikam
