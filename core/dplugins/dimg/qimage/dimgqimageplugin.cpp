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
    return i18n("QImage based DImg loader");
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
    return i18n("A DImg image loader based on QImage plugins");
}

QString DImgQImagePlugin::details() const
{
    return i18n("<p>This plugin permit to load and save image with DImg using "
                "QImage plugins</p>"
                "<p>See <a href='https://doc.qt.io/qt-5/qimage.html'>Qt documentation</a> for details.</p>"
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
    formats.removeAll(QByteArray("PGX"));   // JPEG2000 alternative

    QString ret;

    foreach (const QByteArray& ba, formats)
    {
        ret += QString::fromUtf8("%1 ").arg(QString::fromUtf8(ba).toUpper());
    }

    return ret;
}

bool DImgQImagePlugin::canRead(const QString& filePath) const
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

    // Ignore native loaders.

    if (
        mimeType.contains(QLatin1String("image/jpeg"))  ||
        mimeType.contains(QLatin1String("image/jpg"))   ||
        mimeType.contains(QLatin1String("image/jpe"))   ||
        mimeType.contains(QLatin1String("image/png"))   ||
        mimeType.contains(QLatin1String("image/tif"))   ||
        mimeType.contains(QLatin1String("image/tiff"))  ||
        mimeType.contains(QLatin1String("image/x-pgf")) ||
        mimeType.contains(QLatin1String("image/jp2"))   ||
        mimeType.contains(QLatin1String("image/jpx"))   ||
        mimeType.contains(QLatin1String("image/jpc"))   ||
        mimeType.contains(QLatin1String("image/j2k"))   ||
        mimeType.contains(QLatin1String("image/pgx"))
       )
    {
        return false;
    }

    // Ignore RAW files

    QString ext         = QFileInfo(filePath).suffix().toUpper();
    QString rawFilesExt = QLatin1String(DRawDecoder::rawFiles());

    if (rawFilesExt.toUpper().contains(ext))
    {
        return false;
    }

    return true;
}

bool DImgQImagePlugin::canWrite(const QString& format) const
{
    QString blackList = QLatin1String(DRawDecoder::rawFiles());                              // Ignore RAW files
    blackList.append(QLatin1String(" JPEG JPG JPE PNG TIF TIFF PGF JP2 JPX JPC J2K PGX "));  // Ignore native loaders

    if (blackList.toUpper().contains(format))
    {
        return false;
    }

    // NOTE: Native loaders support are previously black-listed.
    // For ex, if tiff is supported in write mode by QImage it will nerver be handled.

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

DImgLoader* DImgQImagePlugin::loader(DImg* const image) const
{
    return new DImgQImageLoader(image);
}

} // namespace DigikamQImageDImgPlugin
