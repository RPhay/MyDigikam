/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-20
 * Description : QImage DImg plugin.
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

#include "dimgqimageplugin.h"

// Qt includes

#include <QImageReader>
#include <QImageWriter>
#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimgqimageloader.h"

namespace DigikamQImageDImgPlugin
{

DImgQImagePlugin::DImgQImagePlugin(QObject* const parent)
    : DPluginDImg(parent)
{
}

DImgQImagePlugin::~DImgQImagePlugin()
{
}

QString DImgQImagePlugin::name() const
{
    return i18n("QImage loader");
}

QString DImgQImagePlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon DImgQImagePlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-generic"));
}

QString DImgQImagePlugin::description() const
{
    return i18n("An image loader based on QImage plugins");
}

QString DImgQImagePlugin::details() const
{
    return i18n("<p>This plugin allow to load and save image using QImage plugins from Qt Framework.</p>"
                "<p>See <a href='https://doc.qt.io/qt-5/qimage.html'>Qt Framework documentation</a> for details.</p>"
    );
}

QList<DPluginAuthor> DImgQImagePlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Renchi Raju"),
                             QString::fromUtf8("renchi dot raju at gmail dot com"),
                             QString::fromUtf8("(C) 2005"))
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2006-2021"))
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
    QString ret;

    foreach (const QByteArray& ba, QImageReader::supportedImageFormats())
    {
        ret += QString::fromUtf8("%1 ").arg(QString::fromUtf8(ba).toUpper());
    }

    return ret;
}

int DImgQImagePlugin::canRead(const QFileInfo& fileInfo, bool magic) const
{
    QString filePath = fileInfo.filePath();
    QString format   = fileInfo.suffix().toUpper();

    if (!magic)
    {
        QString mimeType(QMimeDatabase().mimeTypeForFile(filePath).name());

        // Ignore non image format.

        if (
            mimeType.startsWith(QLatin1String("video/")) ||
            mimeType.startsWith(QLatin1String("audio/"))
           )
        {
            return 0;
        }

        foreach (const QByteArray& ba, QImageReader::supportedImageFormats())
        {
            if (QString::fromUtf8(ba).toUpper() == format)
            {
                return 80;
            }
        }
    }

    return 0;
}

int DImgQImagePlugin::canWrite(const QString& format) const
{
    foreach (const QByteArray& ba, QImageWriter::supportedImageFormats())
    {
        if (QString::fromUtf8(ba).toUpper() == format.toUpper())
        {
            return 80;
        }
    }

    return 0;
}

DImgLoader* DImgQImagePlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgQImageLoader(image);
}

} // namespace DigikamQImageDImgPlugin
