/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface
 *               Exiv2: www.exiv2.org
 *               Exif : www.exif.org/Exif2-2.PDF
 *               Iptc : www.iptc.org/std/IIM/4.1/specification/IIMV4.1.pdf
 *               Xmp  : www.adobe.com/devnet/xmp/pdfs/xmp_specification.pdf
 *                      www.iptc.org/std/Iptc4xmpCore/1.0/specification/Iptc4xmpCore_1.0-spec-XMPSchema_8.pdf
 *               Paper: www.metadataworkinggroup.com/pdf/mwg_guidance.pdf
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
#include "metaengine_data_p.h"

// Local includes

#include "digikam_debug.h"
#include "digikam_version.h"

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace Digikam
{

MetaEngine::MetaEngine()
    : d(new Private)
{
}

MetaEngine::MetaEngine(const MetaEngine& metadata)
    : d(new Private)
{
    d->copyPrivateData(metadata.d);
}

MetaEngine::MetaEngine(const MetaEngineData& data)
    : d(new Private)
{
    setData(data);
}

MetaEngine::MetaEngine(const QString& filePath)
    : d(new Private)
{
    // NOTE: use dynamic binding as this virtual method can be re-implemented in derived classes.

    this->load(filePath);
}

MetaEngine::~MetaEngine()
{
    delete d;
}

MetaEngine& MetaEngine::operator=(const MetaEngine& metadata)
{
    d->copyPrivateData(metadata.d);

    return *this;
}

//-- Statics methods ----------------------------------------------

bool MetaEngine::initializeExiv2()
{

#ifdef _XMP_SUPPORT_

    if (!Exiv2::XmpParser::initialize())
    {
        return false;
    }

    /**
     * It cleans up memory used by Adobe XMP SDK automatically at application exit.
     * See Bug #166424 for details.
     */
    ::atexit(Exiv2::XmpParser::terminate);

#endif // _XMP_SUPPORT_

#if EXIV2_TEST_VERSION(0,27,4)

    // For Base Media File Format (aka CR3, HEIF, HEIC, and AVIF)

    s_metaEngineSupportBmff = Exiv2::enableBMFF(true);

#endif

    return true;
}

bool MetaEngine::supportBmff()
{
    return s_metaEngineSupportBmff;
}

bool MetaEngine::supportXmp()
{

#ifdef _XMP_SUPPORT_

    return true;

#else

    return false;

#endif // _XMP_SUPPORT_

}

bool MetaEngine::supportMetadataWriting(const QString& /*typeMime*/)
{
/* FIXME : use Exiv2 API to return right writings support
    if      (typeMime == QLatin1String("image/jpeg"))
    {
        return true;
    }
    else if (typeMime == QLatin1String("image/tiff"))
    {
        return true;
    }
    else if (typeMime == QLatin1String("image/png"))
    {
        return true;
    }
    else if (typeMime == QLatin1String("image/jp2"))
    {
        return true;
    }
    else if (typeMime == QLatin1String("image/x-raw"))
    {
        return true;
    }
    else if (typeMime == QLatin1String("image/pgf"))
    {
        return true;
    }
*/
    return true;
}

QString MetaEngine::Exiv2Version()
{

#if EXIV2_TEST_VERSION(0,27,0)

    return QString::fromStdString(Exiv2::versionString());

#else

    return QLatin1String(Exiv2::version());

#endif

}

//-- General methods ----------------------------------------------

MetaEngineData MetaEngine::data() const
{
    MetaEngineData data;
    data.d = d->data;

    return data;
}

void MetaEngine::setData(const MetaEngineData& data)
{
    if (data.d)
    {
        d->data = data.d;
    }
    else
    {
        // MetaEngineData can have a null pointer,
        // but we never want a null pointer in Private.
        d->data->clear();
    }
}

bool MetaEngine::loadFromData(const QByteArray& imgData)
{
    if (imgData.isEmpty())
        return false;

    QMutexLocker lock(&s_metaEngineMutex);

    try
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open((Exiv2::byte*)imgData.data(), imgData.size());

        d->filePath.clear();
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

        return true;
    }
    catch (Exiv2::AnyError& e)
    {
        d->printExiv2ExceptionError(QLatin1String("Cannot load metadata using Exiv2 "), e);
    }
    catch (...)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool MetaEngine::changedMetadata() const
{
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

        // Check comment difference

        std::string orgCom = image->comment();
        std::string newCom = d->itemComments();

        if (newCom.empty())
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "Comment Removed";
        }

        if (orgCom != newCom)
        {
            qCDebug(DIGIKAM_METAENGINE_LOG) << "Changed Comment"
                                            << newCom.c_str()
                                            << "old comment"
                                            << orgCom.c_str();
        }

        // Parse differences in Exif

        Exiv2::ExifData orgExif = image->exifData();
        Exiv2::ExifData newExif = d->exifMetadata();

        for (Exiv2::ExifData::const_iterator it = newExif.begin() ; it != newExif.end() ; ++it)
        {
            Exiv2::ExifData::const_iterator it2 = orgExif.findKey(Exiv2::ExifKey(it->key()));

            if (it2 == orgExif.end())
            {
                // Orignal Exif do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "New Exif tag" << it->key().c_str()
                                                << "with value"   << it->toString().c_str();
            }
            else
            {
                // Original Exif has already the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Changed Exif tag" << it->key().c_str()
                                                << "old value"        << it2->toString().c_str()
                                                << "new value"        << it->toString().c_str();
            }
        }

        // Check for removed tags.

        for (Exiv2::ExifData::const_iterator it = orgExif.begin() ; it != orgExif.end() ; ++it)
        {
            Exiv2::ExifData::const_iterator it2 = newExif.findKey(Exiv2::ExifKey(it->key()));

            if (it2 == newExif.end())
            {
                // New Exif do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Removed Exif tag" << it->key().c_str();
            }
        }

        // Parse differences in Iptc

        Exiv2::IptcData orgIptc = image->iptcData();
        Exiv2::IptcData newIptc = d->iptcMetadata();

        for (Exiv2::IptcData::const_iterator it = newIptc.begin() ; it != newIptc.end() ; ++it)
        {
            Exiv2::IptcData::const_iterator it2 = orgIptc.findKey(Exiv2::IptcKey(it->key()));

            if (it2 == orgIptc.end())
            {
                // Orignal Iptc do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "New Iptc tag" << it->key().c_str()
                                                << "with value"   << it->toString().c_str();
            }
            else
            {
                // Original Iptc has already the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Changed Iptc tag" << it->key().c_str()
                                                << "old value"        << it2->toString().c_str()
                                                << "new value"        << it->toString().c_str();
            }
        }

        // Check for removed tags.

        for (Exiv2::IptcData::const_iterator it = orgIptc.begin() ; it != orgIptc.end() ; ++it)
        {
            Exiv2::IptcData::const_iterator it2 = newIptc.findKey(Exiv2::IptcKey(it->key()));

            if (it2 == newIptc.end())
            {
                // New Iptc do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Removed Iptc tag" << it->key().c_str();
            }
        }

#ifdef _XMP_SUPPORT_

        // Parse differences in Xmp

        Exiv2::XmpData orgXmp = image->xmpData();
        Exiv2::XmpData newXmp = d->xmpMetadata();

        for (Exiv2::XmpData::const_iterator it = newXmp.begin() ; it != newXmp.end() ; ++it)
        {
            Exiv2::XmpData::const_iterator it2 = orgXmp.findKey(Exiv2::XmpKey(it->key()));

            if (it2 == orgXmp.end())
            {
                // Orignal Xmp do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "New Xmp tag" << it->key().c_str()
                                                << "with value"  << it->toString().c_str();
            }
            else
            {
                // Original Xmp has already the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Changed Xmp tag" << it->key().c_str()
                                                << "old value"       << it2->toString().c_str()
                                                << "new value"       << it->toString().c_str();
            }
        }

        // Check for removed tags.

        for (Exiv2::XmpData::const_iterator it = orgXmp.begin() ; it != orgXmp.end() ; ++it)
        {
            Exiv2::XmpData::const_iterator it2 = newXmp.findKey(Exiv2::XmpKey(it->key()));

            if (it2 == newXmp.end())
            {
                // New Xmp do not have the tag.

                qCDebug(DIGIKAM_METAENGINE_LOG) << "Removed Xmp tag" << it->key().c_str();
            }
        }

#endif // _XMP_SUPPORT_

        return true;
    }
    catch (Exiv2::AnyError& e)
    {
        d->printExiv2ExceptionError(QLatin1String("Cannot load metadata using Exiv2 "), e);
    }
    catch (...)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Default exception from Exiv2";
    }

    return false;
}

bool MetaEngine::isEmpty() const
{
    if (!hasComments() && !hasExif() && !hasIptc() && !hasXmp())
        return true;

    return false;
}

QSize MetaEngine::getPixelSize() const
{
    return d->pixelSize;
}

QString MetaEngine::getMimeType() const
{
    return d->mimeType;
}

void MetaEngine::setWriteRawFiles(const bool on)
{
    d->writeRawFiles = on;
}

bool MetaEngine::writeRawFiles() const
{
    return d->writeRawFiles;
}

void MetaEngine::setWriteDngFiles(const bool on)
{
    d->writeDngFiles = on;
}

bool MetaEngine::writeDngFiles() const
{
    return d->writeDngFiles;
}

void MetaEngine::setUseXMPSidecar4Reading(const bool on)
{
    d->useXMPSidecar4Reading = on;
}

bool MetaEngine::useXMPSidecar4Reading() const
{
    return d->useXMPSidecar4Reading;
}

void MetaEngine::setUseCompatibleFileName(const bool on)
{
    d->useCompatibleFileName = on;
}

bool MetaEngine::useCompatibleFileName() const
{
    return d->useCompatibleFileName;
}

void MetaEngine::setMetadataWritingMode(const int mode)
{
    d->metadataWritingMode = mode;
}

int MetaEngine::metadataWritingMode() const
{
    return d->metadataWritingMode;
}

void MetaEngine::setUpdateFileTimeStamp(bool on)
{
    d->updateFileTimeStamp = on;
}

bool MetaEngine::updateFileTimeStamp() const
{
    return d->updateFileTimeStamp;
}

bool MetaEngine::setProgramId() const
{
    QString version(digiKamVersion());
    QLatin1String software("digiKam");
    return setItemProgramId(software, version);
}

} // namespace Digikam

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif
