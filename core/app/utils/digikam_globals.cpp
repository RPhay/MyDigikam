/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-09-08
 * Description : global macros, variables and flags
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "digikam_globals_p.h"

namespace Digikam
{

QShortcut* defineShortcut(QWidget* const w, const QKeySequence& key, const QObject* receiver, const char* slot)
{
    QShortcut* const s = new QShortcut(w);
    s->setKey(key);
    s->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(s, SIGNAL(activated()), receiver, slot);

    return s;
}

QStringList supportedImageMimeTypes(QIODevice::OpenModeFlag mode, QString& allTypes)
{
    QStringList       formats;
    QList<QByteArray> supported;

    switch (mode)
    {
        case QIODevice::ReadOnly:
        {
            supported = QImageReader::supportedImageFormats();
            break;
        }

        case QIODevice::WriteOnly:
        {
            supported = QImageWriter::supportedImageFormats();
            break;
        }

        case QIODevice::ReadWrite:
        {
            supported = QImageWriter::supportedImageFormats() + QImageReader::supportedImageFormats();
            break;
        }

        default:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Unsupported mode!";
            break;
        }
    }

    foreach (const QByteArray& frm, supported)
    {
        if (QString::fromLatin1(frm).contains(QLatin1String("tif"),  Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("tiff"), Qt::CaseInsensitive))
        {
            continue;
        }

        if (QString::fromLatin1(frm).contains(QLatin1String("jpg"),  Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("jpeg"), Qt::CaseInsensitive))
        {
            continue;
        }

#ifdef HAVE_JASPER

        if (QString::fromLatin1(frm).contains(QLatin1String("jp2"),  Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("j2k"),  Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("jpx"),  Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("jpc"),  Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("pgx"),  Qt::CaseInsensitive))
        {
            continue;
        }

#endif // HAVE_JASPER

#ifdef HAVE_X265

        if (QString::fromLatin1(frm).contains(QLatin1String("heic"), Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("heif"), Qt::CaseInsensitive))
        {
            continue;
        }

#endif // HAVE_X265

#ifdef HAVE_IMAGE_MAGICK

        if (QString::fromLatin1(frm).contains(QLatin1String("fts"),  Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("fit"),  Qt::CaseInsensitive) ||
            QString::fromLatin1(frm).contains(QLatin1String("fits"), Qt::CaseInsensitive))
        {
            continue;
        }

#endif // HAVE_IMAGE_MAGICK

        formats.append(i18n("%1 Image (%2)", QString::fromLatin1(frm).toUpper(), QLatin1String("*.") + QLatin1String(frm)));
        allTypes.append(QString::fromLatin1("*.%1 ").arg(QLatin1String(frm)));
    }

    formats.append(i18n("TIFF Image (*.tiff *.tif)"));
    allTypes.append(QLatin1String("*.tiff *.tif "));
    formats.append(i18n("JPEG Image (*.jpg *.jpeg *.jpe)"));
    allTypes.append(QLatin1String("*.jpg *.jpeg *.jpe "));

#ifdef HAVE_JASPER

    formats.append(i18n("JPEG2000 Image (*.jp2 *.j2k *.jpx *.pgx)"));
    allTypes.append(QLatin1String("*.jp2 *.j2k *.jpx *.pgx "));

#endif // HAVE_JASPER

    formats << i18n("Progressive Graphics file (*.pgf)");
    allTypes.append(QLatin1String("*.pgf "));

#ifdef HAVE_X265

    formats << i18n("High Efficiency Image Coding (*.heic *.heif)");
    allTypes.append(QLatin1String("*.heic *.heif "));

#endif // HAVE_X265

#ifdef HAVE_IMAGE_MAGICK

    formats << i18n("Flexible Image Transport System (*.fts *.fit *.fits)");
    allTypes.append(QLatin1String("*.fts *.fit *.fits "));

#endif // HAVE_IMAGE_MAGICK

    if (mode != QIODevice::WriteOnly)
    {
        formats << i18n("Raw Images (%1)", DRawDecoder::rawFiles());
        allTypes.append(DRawDecoder::rawFiles());
        formats << i18n("All supported files (%1)", allTypes);
    }

    return formats;
}

bool isReadableImageFile(const QString& filePath)
{
    QFileInfo info(filePath);

    if (info.isFile() && !info.isSymLink() && !info.isDir() && !info.isRoot())
    {
        QString path    = info.absoluteFilePath();
        QMimeType mtype = QMimeDatabase().mimeTypeForFile(path);
        QString suffix  = info.suffix().toUpper();

        // Add extra check of the image extensions that are still
        // unknown in older Qt versions or have an application mime type.

        if (mtype.name().startsWith(QLatin1String("image/")) ||
            (suffix == QLatin1String("PGF"))                 ||
            (suffix == QLatin1String("KRA"))                 ||
            (suffix == QLatin1String("CR3"))                 ||
            (suffix == QLatin1String("HEIC"))                ||
            (suffix == QLatin1String("HEIF"))                ||
            DRawDecoder::rawFiles().contains(suffix))
        {
            return true;
        }
    }

    return false;
}

void showRawCameraList()
{
    RawCameraDlg* const dlg = new RawCameraDlg(qApp->activeWindow());
    dlg->show();
}

QString toolButtonStyleSheet()
{
    return QLatin1String("QToolButton { padding: 1px; background-color: "
                         "  qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                         "  stop: 0 rgba(100, 100, 100, 50%), "
                         "  stop: 1 rgba(170, 170, 170, 50%)); "
                         "border: 1px solid rgba(170, 170, 170, 10%); } "

                         "QToolButton:hover { border-color: white; } "

                         "QToolButton:pressed { background-color: "
                         "  qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                         "  stop: 0 rgba(40, 40, 40, 50%), "
                         "  stop: 1 rgba(90, 90, 90, 50%)); "
                         "border-color: white; } "

                         "QToolButton:checked { background-color: "
                         "  qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                         "  stop: 0 rgba(40, 40, 40, 50%), "
                         "  stop: 1 rgba(90, 90, 90, 50%)); } "

                         "QToolButton:disabled { background-color: "
                         "  qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                         "  stop: 0 rgba(40, 40, 40, 50%), "
                         "  stop: 1 rgba(50, 50, 50, 50%)); }");
}

} // namespace Digikam
