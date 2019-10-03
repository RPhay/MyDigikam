/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-20
 * Description : QImage DImg plugin.
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QMimeDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimgqimageloader.h"
#include "drawdecoder.h"

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
    return i18n("<p>This plugin permit to load and save image using QImage plugins from Qt Framework.</p>"
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
                             QString::fromUtf8("(C) 2006-2019"))
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
    QList<QByteArray> formats = QImageReader::supportedImageFormats();

    qDebug(DIGIKAM_DIMG_LOG_QIMAGE) << "QImage support this formats:" << formats;

    formats.removeAll(QByteArray("JPEG"));  // JPEG file format
    formats.removeAll(QByteArray("JPG"));   // JPEG file format
    formats.removeAll(QByteArray("JPE"));   // JPEG file format
    formats.removeAll(QByteArray("PNG"));
    formats.removeAll(QByteArray("TIFF"));
    formats.removeAll(QByteArray("TIF"));
    formats.removeAll(QByteArray("PGF"));
    formats.removeAll(QByteArray("JP2"));   // JPEG2000 file format
    formats.removeAll(QByteArray("JPX"));   // JPEG2000 file format
    formats.removeAll(QByteArray("JPC"));   // JPEG2000 code stream
    formats.removeAll(QByteArray("J2K"));   // JPEG2000 code stream
    formats.removeAll(QByteArray("PGX"));   // JPEG2000 WM format
    formats.removeAll(QByteArray("HEIC"));

    QString rawFilesExt = QString(DRawDecoder::rawFiles()).remove(QLatin1String("*.")).toUpper();

    foreach (const QString& str, rawFilesExt.split(QLatin1Char(' ')))
    {
        formats.removeAll(str.toLatin1());             // All Raw image formats
    }

    QString ret;

    foreach (const QByteArray& ba, formats)
    {
        ret += QString::fromUtf8("%1 ").arg(QString::fromUtf8(ba).toUpper());
    }

    return ret;
}

bool DImgQImagePlugin::canRead(const QString& filePath, bool magic) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File " << filePath << " does not exist";
        return false;
    }

    if (!magic)
    {
        QString mimeType(QMimeDatabase().mimeTypeForFile(filePath).name());

        // Ignore non image format.

        if (
            mimeType.startsWith(QLatin1String("video/")) ||
            mimeType.startsWith(QLatin1String("audio/"))
           )
        {
            return false;
        }

        QString format    = fileInfo.suffix().toUpper();
        QString blackList = QString(DRawDecoder::rawFiles()).remove(QLatin1String("*.")).toUpper();  // Ignore RAW files
        blackList.append(QLatin1String(" JPEG JPG JPE PNG TIF TIFF PGF JP2 JPX JPC J2K PGX HEIC ")); // Ignore native loaders

        return (!blackList.toUpper().contains(format));
    }

    return false;
}

bool DImgQImagePlugin::canWrite(const QString& format) const
{
    QString blackList = QString(DRawDecoder::rawFiles()).remove(QLatin1String("*.")).toUpper();  // Ignore RAW files
    blackList.append(QLatin1String(" JPEG JPG JPE PNG TIF TIFF PGF JP2 JPX JPC J2K PGX HEIC ")); // Ignore native loaders

    if (blackList.toUpper().contains(format))
    {
        return false;
    }

    // NOTE: Native loaders support are previously black-listed.
    // For ex, if tiff is supported in write mode by QImage it will never be handled.

    QList<QByteArray> formats = QImageWriter::supportedImageFormats();

    foreach (const QByteArray& ba, formats)
    {
        if (QString::fromUtf8(ba).toUpper().contains(format.toUpper()))
        {
            return true;
        }
    }

    return false;
}

DImgLoader* DImgQImagePlugin::loader(DImg* const image, const DRawDecoding&) const
{
    return new DImgQImageLoader(image);
}

} // namespace DigikamQImageDImgPlugin
