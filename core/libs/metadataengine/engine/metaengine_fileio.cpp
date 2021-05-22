/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface.
 *               File I/O methods
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "metaengine_p.h"

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "digikam_version.h"

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace Digikam
{

void MetaEngine::setFilePath(const QString& path)
{
    d->filePath = path;
}

QString MetaEngine::getFilePath() const
{
    return d->filePath;
}

QString MetaEngine::sidecarFilePathForFile(const QString& path, bool useLR)
{
    if (path.isEmpty())
    {
        return QString();
    }

    QFileInfo info(path);
    QString pathForLR = path;
    pathForLR.chop(info.suffix().size());
    pathForLR.append(QLatin1String("xmp"));

    if (useLR || QFileInfo::exists(pathForLR))
    {
        return pathForLR;
    }

    return path + QLatin1String(".xmp");
}

QUrl MetaEngine::sidecarUrl(const QUrl& url)
{
    return sidecarUrl(url.toLocalFile());
}

QUrl MetaEngine::sidecarUrl(const QString& path)
{
    return QUrl::fromLocalFile(sidecarFilePathForFile(path));
}

QString MetaEngine::sidecarPath(const QString& path)
{
    return sidecarFilePathForFile(path);
}

bool MetaEngine::hasSidecar(const QString& path)
{
    return QFileInfo::exists(sidecarFilePathForFile(path));
}

QString MetaEngine::backendName(Backend t)
{
    switch (t)
    {
        case LibRawBackend:
        {
            return QLatin1String("LibRaw");
        }

        case LibHeifBackend:
        {
            return QLatin1String("LibHeif");
        }

        case ImageMagickBackend:
        {
            return QLatin1String("ImageMagick");
        }

        case FFMpegBackend:
        {
            return QLatin1String("FFMpeg");
        }

        case NoBackend:
        {
            return QLatin1String("No Backend");
        }

        default:
        {
            return QLatin1String("Exiv2");
        }
    }
}

bool MetaEngine::load(const QString& filePath, Backend* backend)
{
    if (backend)
    {
        *backend = NoBackend;
    }

    if (filePath.isEmpty())
    {
        return false;
    }

    d->filePath      = filePath;
    bool hasLoaded   = false;

    QMutexLocker lock(&s_metaEngineMutex);

    try
    {
        Exiv2::Image::AutoPtr image;

#if defined Q_OS_WIN && defined EXV_UNICODE_PATH

        image        = Exiv2::ImageFactory::open((const wchar_t*)filePath.utf16());

#elif defined Q_OS_WIN

        image        = Exiv2::ImageFactory::open(QFile::encodeName(filePath).constData());

#else

        image        = Exiv2::ImageFactory::open(filePath.toUtf8().constData());

#endif

        image->readMetadata();

        // Size and mimetype ---------------------------------

        d->pixelSize = QSize(image->pixelWidth(), image->pixelHeight());
        d->mimeType  = QString::fromStdString(image->mimeType());

        // Image comments ---------------------------------

        d->itemComments() = image->comment();

        // Exif metadata ----------------------------------

        d->exifMetadata() = image->exifData();

        // Iptc metadata ----------------------------------

        d->iptcMetadata() = image->iptcData();

#ifdef _XMP_SUPPORT_

        // Xmp metadata -----------------------------------
        d->xmpMetadata() = image->xmpData();

#endif // _XMP_SUPPORT_


        if (backend)
        {
            *backend = Exiv2Backend;
        }

        hasLoaded = true;

    }
    catch (Exiv2::AnyError& e)
    {
        d->printExiv2ExceptionError(QString::fromUtf8("Cannot load metadata from file with Exiv2 backend: %1").arg(getFilePath()), e);
    }
    catch (...)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Default exception from Exiv2";
    }

    hasLoaded |= loadFromSidecarAndMerge(filePath);

    return hasLoaded;
}

bool MetaEngine::loadFromSidecarAndMerge(const QString& filePath)
{
    if (filePath.isEmpty())
    {
        return false;
    }

    d->filePath    = filePath;
    bool hasLoaded = false;

#ifdef _XMP_SUPPORT_

    QMutexLocker lock(&s_metaEngineMutex);

    try
    {
        if (d->useXMPSidecar4Reading)
        {
            QString xmpSidecarPath = sidecarFilePathForFile(filePath);
            QFileInfo xmpSidecarFileInfo(xmpSidecarPath);

            Exiv2::Image::AutoPtr xmpsidecar;

            if (xmpSidecarFileInfo.exists() && xmpSidecarFileInfo.isReadable())
            {
                // Read sidecar data

#if defined Q_OS_WIN && defined EXV_UNICODE_PATH

                xmpsidecar = Exiv2::ImageFactory::open((const wchar_t*)xmpSidecarPath.utf16());

#elif defined Q_OS_WIN

                xmpsidecar = Exiv2::ImageFactory::open(QFile::encodeName(xmpSidecarPath).constData());

#else

                xmpsidecar = Exiv2::ImageFactory::open(xmpSidecarPath.toUtf8().constData());

#endif

                xmpsidecar->readMetadata();

                // Merge

#if EXIV2_TEST_VERSION(0,27,99)

                d->loadSidecarData(std::move(xmpsidecar));

#else

                d->loadSidecarData(xmpsidecar);

#endif

                hasLoaded = true;
            }
        }
    }
    catch (Exiv2::AnyError& e)
    {
        d->printExiv2ExceptionError(QString::fromUtf8("Cannot load XMP sidecar from file with Exiv2 backend: %1").arg(getFilePath()), e);
    }
    catch (...)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Default exception from Exiv2";
    }

#endif // _XMP_SUPPORT_

    return hasLoaded;
}

bool MetaEngine::save(const QString& imageFilePath, bool setVersion) const
{
    if (setVersion && !setProgramId())
    {
        return false;
    }

    // If our image is really a symlink, we should follow the symlink so that
    // when we delete the file and rewrite it, we are honoring the symlink
    // (rather than just deleting it and putting a file there).
    //
    // However, this may be surprising to the user when they are writing sidecar
    // files.  They might expect them to show up where the symlink is.  So, we
    // shouldn't follow the link when figuring out what the filename for the
    // sidecar should be.
    //
    // Note, we are not yet handling the case where the sidecar itself is a
    // symlink.

    QString regularFilePath = imageFilePath; // imageFilePath might be a
                                             // symlink.  Below we will change
                                             // regularFile to the pointed to
                                             // file if so.
    QFileInfo givenFileInfo(imageFilePath);

    if (givenFileInfo.isSymLink())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "filePath" << imageFilePath << "is a symlink."
                                        << "Using target" << givenFileInfo.canonicalFilePath();

        regularFilePath = givenFileInfo.canonicalFilePath(); // Walk all the symlinks
    }

    // NOTE: see B.K.O #137770 & #138540 : never touch the file if is read only.

    QFileInfo finfo(regularFilePath);
    QFileInfo dinfo(finfo.path());

    if (!dinfo.isWritable())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "Dir" << dinfo.filePath() << "is read-only. Metadata not saved.";
        return false;
    }

    bool writeToFile                     = false;
    bool writeToSidecar                  = false;
    bool writeToSidecarIfFileNotPossible = false;
    bool writtenToFile                   = false;
    bool writtenToSidecar                = false;

    qCDebug(DIGIKAM_METAENGINE_LOG) << "MetaEngine::metadataWritingMode" << d->metadataWritingMode;

    switch (d->metadataWritingMode)
    {
        case WRITE_TO_SIDECAR_ONLY:
        {
            writeToSidecar = true;
            break;
        }

        case WRITE_TO_FILE_ONLY:
        {
            writeToFile    = true;
            break;
        }

        case WRITE_TO_SIDECAR_AND_FILE:
        {
            writeToFile    = true;
            writeToSidecar = true;
            break;
        }

        case WRITE_TO_SIDECAR_ONLY_FOR_READ_ONLY_FILES:
        {
            writeToFile                     = true;
            writeToSidecarIfFileNotPossible = true;
            break;
        }
    }

    if (writeToFile)
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "Will write Metadata to file" << finfo.absoluteFilePath();
        writtenToFile = d->saveToFile(finfo);

        if (writtenToFile)
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "Metadata for file" << finfo.fileName() << "written to file.";
        }
    }

    if (writeToSidecar || (writeToSidecarIfFileNotPossible && !writtenToFile))
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "Will write XMP sidecar for file" << finfo.fileName();
        writtenToSidecar = d->saveToXMPSidecar(regularFilePath);

        if (writtenToSidecar)
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "Metadata for file" << finfo.fileName() << "written to XMP sidecar.";
        }
    }

    return (writtenToFile || writtenToSidecar);
}

bool MetaEngine::applyChanges(bool setVersion) const
{
    if (d->filePath.isEmpty())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "Failed to apply changes: file path is empty!";
        return false;
    }

    return save(d->filePath, setVersion);
}

bool MetaEngine::exportChanges(const QString& exvTmpFile, QStringList& removedTags) const
{
    if (d->filePath.isEmpty())
    {
        qCDebug(DIGIKAM_METAENGINE_LOG) << "Failed to export changes: file path is empty!";
        return false;
    }

    QMutexLocker lock(&s_metaEngineMutex);

    try
    {
        Exiv2::Image::AutoPtr image;

        qCDebug(DIGIKAM_METAENGINE_LOG) << "List of changes to perform on:" << getFilePath();

#if defined Q_OS_WIN && defined EXV_UNICODE_PATH

        image = Exiv2::ImageFactory::open((const wchar_t*)getFilePath().utf16());

#elif defined Q_OS_WIN

        image = Exiv2::ImageFactory::open(QFile::encodeName(getFilePath()).constData());

#else

        image = Exiv2::ImageFactory::open(getFilePath().toUtf8().constData());

#endif

        image->readMetadata();

        // --- Parse differences in Exif

        Exiv2::ExifData orgExif = image->exifData();
        Exiv2::ExifData newExif = d->exifMetadata();
        Exiv2::ExifData chgExif;

        for (Exiv2::ExifData::const_iterator it = newExif.begin() ; it != newExif.end() ; ++it)
        {
            Exiv2::ExifData::const_iterator it2 = orgExif.findKey(Exiv2::ExifKey(it->key()));

            if      (it2 == orgExif.end())
            {
                // Original Exif do not have the tag.

                chgExif[it->key().c_str()] = newExif[it->key().c_str()];

                qCDebug(DIGIKAM_METAENGINE_LOG) << "New Exif tag" << it->key().c_str()
                                                << "with value"   << it->toString().c_str();
            }
            else if (getExifTagData(it2->key().c_str()) != getExifTagData(it->key().c_str()))
            {
                // Original Exif has already the tag.

                chgExif[it->key().c_str()] = newExif[it->key().c_str()];

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Changed Exif tag" << it->key().c_str()
                                                << "old value"        << it2->toString().c_str()
                                                << "new value"        << it->toString().c_str();
            }
        }

        // Check for removed Exif tags.

        for (Exiv2::ExifData::const_iterator it = orgExif.begin() ; it != orgExif.end() ; ++it)
        {
            Exiv2::ExifData::const_iterator it2 = newExif.findKey(Exiv2::ExifKey(it->key()));

            if (it2 == newExif.end())
            {
                // New Exif do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Removed Exif tag" << it->key().c_str();
                removedTags <<  QString::fromStdString(it->key());
            }
        }

        // --- Parse differences in Iptc

        Exiv2::IptcData orgIptc = image->iptcData();
        Exiv2::IptcData newIptc = d->iptcMetadata();
        Exiv2::IptcData chgIptc;

        for (Exiv2::IptcData::const_iterator it = newIptc.begin() ; it != newIptc.end() ; ++it)
        {
            Exiv2::IptcData::const_iterator it2 = orgIptc.findKey(Exiv2::IptcKey(it->key()));

            if      (it2 == orgIptc.end())
            {
                // Original Iptc do not have the tag.

                chgIptc[it->key().c_str()] = newIptc[it->key().c_str()];

                qCDebug(DIGIKAM_METAENGINE_LOG) << "New Iptc tag" << it->key().c_str()
                                                << "with value"   << it->toString().c_str();
            }
            else if (getIptcTagData(it2->key().c_str()) != getIptcTagData(it->key().c_str()))
            {
                // Original Iptc has already the tag.

                chgIptc[it->key().c_str()] = newIptc[it->key().c_str()];

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Changed Iptc tag" << it->key().c_str()
                                                << "old value"        << it2->toString().c_str()
                                                << "new value"        << it->toString().c_str();
            }
        }

        // Check for removed Iptc tags.

        for (Exiv2::IptcData::const_iterator it = orgIptc.begin() ; it != orgIptc.end() ; ++it)
        {
            Exiv2::IptcData::const_iterator it2 = newIptc.findKey(Exiv2::IptcKey(it->key()));

            if (it2 == newIptc.end())
            {
                // New Iptc do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Removed Iptc tag" << it->key().c_str();
                removedTags <<  QString::fromStdString(it->key());
            }
        }

#ifdef _XMP_SUPPORT_

        // --- Parse differences in Xmp

        Exiv2::XmpData orgXmp = image->xmpData();
        Exiv2::XmpData newXmp = d->xmpMetadata();
        Exiv2::XmpData chgXmp;

        for (Exiv2::XmpData::const_iterator it = newXmp.begin() ; it != newXmp.end() ; ++it)
        {
            Exiv2::XmpData::const_iterator it2 = orgXmp.findKey(Exiv2::XmpKey(it->key()));

            if      (it2 == orgXmp.end())
            {
                // Original Xmp do not have the tag.

                chgXmp[it->key().c_str()] = newXmp[it->key().c_str()];

                qCDebug(DIGIKAM_METAENGINE_LOG) << "New Xmp tag" << it->key().c_str()
                                                << "with value"  << it->toString().c_str();
            }
            else if (getXmpTagVariant(it2->key().c_str()) != getXmpTagVariant(it->key().c_str()))
            {
                // Original Xmp has already the tag.

                chgXmp[it->key().c_str()] = newXmp[it->key().c_str()];

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Changed Xmp tag" << it->key().c_str()
                                                << "old value"       << it2->toString().c_str()
                                                << "new value"       << it->toString().c_str();
            }
        }

        // Check for removed Xmp tags.

        for (Exiv2::XmpData::const_iterator it = orgXmp.begin() ; it != orgXmp.end() ; ++it)
        {
            Exiv2::XmpData::const_iterator it2 = newXmp.findKey(Exiv2::XmpKey(it->key()));

            if (it2 == newXmp.end())
            {
                // New Xmp do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Removed Xmp tag" << it->key().c_str();
                removedTags <<  QString::fromStdString(it->key());
            }
        }

#endif // _XMP_SUPPORT_

        // Create target EXV container with list of changed tags.

        Exiv2::Image::AutoPtr targetExv = Exiv2::ImageFactory::create(Exiv2::ImageType::exv, exvTmpFile.toStdString());
        targetExv->setExifData(chgExif);
        targetExv->setIptcData(chgIptc);
        targetExv->setXmpData(chgXmp);
        targetExv->writeMetadata();

        return true;
    }
    catch (Exiv2::AnyError& e)
    {
        d->printExiv2ExceptionError(QLatin1String("Cannot export changes with Exiv2 backend: "), e);
    }
    catch (...)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Default exception from Exiv2";
    }

    return false;
}

} // namespace Digikam

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif
