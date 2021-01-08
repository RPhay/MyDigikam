/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-14
 * Description : DImg image loader interface
 *
 * Copyright (C) 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dimgloader.h"

// C++ includes

#include <limits>
#include <new>
#include <cstddef>

// Qt includes

#include <QScopedPointer>

// Local includes

#include "digikam_debug.h"
#include "dimg_p.h"
#include "dmetadata.h"
#include "dimgloaderobserver.h"
#include "kmemoryinfo.h"

namespace Digikam
{

DImgLoader::DImgLoader(DImg* const image)
    : m_image(image),
      m_loadFlags(LoadAll)
{
}

DImgLoader::~DImgLoader()
{
}

void DImgLoader::setLoadFlags(LoadFlags flags)
{
    m_loadFlags = flags;
}

bool DImgLoader::hasLoadedData() const
{
    return ((m_loadFlags & (LoadImageData | LoadPreview)) && m_image->m_priv->data);
}

int DImgLoader::granularity(DImgLoaderObserver* const observer, int total, float progressSlice)
{
    // Splits expect total value into the chunks where checks shall occur
    // and combines this with a possible correction factor from observer.
    // Progress slice is the part of 100% concerned with the current granularity
    // (E.g. in a loop only the values from 10% to 90% are used, then progressSlice is 0.8)
    // Current default is 1/20, that is progress info every 5%
    int granularity = 0;

    if (observer)
    {
        granularity = (int)((total / (20 * progressSlice)) / observer->granularity());
    }

    return (granularity ? granularity : 1);
}

unsigned char*& DImgLoader::imageData()
{
    return m_image->m_priv->data;
}

quint64 DImgLoader::imageNumBytes() const
{
    return m_image->numBytes();
}

unsigned int& DImgLoader::imageWidth()
{
    return m_image->m_priv->width;
}

unsigned int& DImgLoader::imageHeight()
{
    return m_image->m_priv->height;
}

bool DImgLoader::imageHasAlpha() const
{
    return m_image->hasAlpha();
}

bool DImgLoader::imageSixteenBit() const
{
    return m_image->sixteenBit();
}

int DImgLoader::imageBitsDepth() const
{
    return m_image->bitsDepth();
}

int DImgLoader::imageBytesDepth() const
{
    return m_image->bytesDepth();
}

void DImgLoader::imageSetIccProfile(const IccProfile& profile)
{
    m_image->setIccProfile(profile);
}

QVariant DImgLoader::imageGetAttribute(const QString& key) const
{
    return m_image->attribute(key);
}

QString DImgLoader::imageGetEmbbededText(const QString& key) const
{
    return m_image->embeddedText(key);
}

void DImgLoader::imageSetAttribute(const QString& key, const QVariant& value)
{
    m_image->setAttribute(key, value);
}

QMap<QString, QString>& DImgLoader::imageEmbeddedText() const
{
    return m_image->m_priv->embeddedText;
}

void DImgLoader::imageSetEmbbededText(const QString& key, const QString& text)
{
    m_image->setEmbeddedText(key, text);
}

void DImgLoader::loadingFailed()
{
    if (m_image->m_priv->data)
    {
        delete [] m_image->m_priv->data;
    }

    m_image->m_priv->data   = nullptr;
    m_image->m_priv->width  = 0;
    m_image->m_priv->height = 0;
}

qint64 DImgLoader::checkAllocation(qint64 fullSize)
{
    if ((quint64)fullSize >= std::numeric_limits<size_t>::max())
    {
        qCWarning(DIGIKAM_DIMG_LOG) << "Cannot allocate buffer of size" << fullSize;
        return 0;
    }

    // Do extra check if allocating serious amounts of memory.
    // At the time of writing (2011), I consider 100 MB as "serious".
    if (fullSize > (qint64)(100 * 1024 * 1024))
    {
        KMemoryInfo memory = KMemoryInfo::currentInfo();

        int res = memory.isValid();

        if      (res == -1)
        {
            qCWarning(DIGIKAM_DIMG_LOG) << "Not a recognized platform to get memory information";
            return -1;
        }
        else if (res == 0)
        {
            qCWarning(DIGIKAM_DIMG_LOG) << "Error to get physical memory information form a recognized platform";
            return 0;
        }

        qint64 available = memory.bytes(KMemoryInfo::AvailableMemory);

        if (fullSize > available)
        {
            qCWarning(DIGIKAM_DIMG_LOG) << "Not enough memory to allocate buffer of size " << fullSize;
            qCWarning(DIGIKAM_DIMG_LOG) << "Available memory size is " << available;
            return 0;
        }
    }

    return fullSize;
}

bool DImgLoader::readMetadata(const QString& filePath)
{
    if (!((m_loadFlags & LoadMetadata) || (m_loadFlags & LoadUniqueHash) || (m_loadFlags & LoadImageHistory)))
    {
        return false;
    }

    QScopedPointer<DMetadata> metaDataFromFile(new DMetadata);
    
    if (!metaDataFromFile->load(filePath))
    {
        m_image->setMetadata(MetaEngineData());
        return false;
    }

    m_image->setMetadata(metaDataFromFile->data());

    if (m_loadFlags & LoadImageHistory)
    {
        DImageHistory history = DImageHistory::fromXml(metaDataFromFile->getItemHistory());
        HistoryImageId id     = m_image->createHistoryImageId(filePath, HistoryImageId::Current);
        history << id;

        m_image->setItemHistory(history);
        imageSetAttribute(QLatin1String("originalImageHistory"), QVariant::fromValue(history));
    }

    return true;
}

bool DImgLoader::saveMetadata(const QString& filePath)
{
    QScopedPointer<DMetadata> metaDataToFile(new DMetadata(filePath));
    metaDataToFile->setData(m_image->getMetadata());

    return metaDataToFile->applyChanges(true);
}

bool DImgLoader::checkExifWorkingColorSpace() const
{
    QScopedPointer<DMetadata> metaData(new DMetadata(m_image->getMetadata()));
    IccProfile profile = metaData->getIccProfile();

    if (!profile.isNull())
    {
        m_image->setIccProfile(profile);
        return true;
    }

    return false;
}

void DImgLoader::storeColorProfileInMetadata()
{
    IccProfile profile = m_image->getIccProfile();

    if (profile.isNull())
    {
        return;
    }

    QScopedPointer<DMetadata> metaData(new DMetadata(m_image->getMetadata()));
    metaData->setIccProfile(profile);
    m_image->setMetadata(metaData->data());
}

void DImgLoader::purgeExifWorkingColorSpace()
{
    QScopedPointer<DMetadata> meta(new DMetadata(m_image->getMetadata()));
    meta->removeExifColorSpace();
    m_image->setMetadata(meta->data());
}

unsigned char* DImgLoader::new_failureTolerant(size_t unsecureSize)
{
    return new_failureTolerant<unsigned char>(unsecureSize);
}

unsigned char* DImgLoader::new_failureTolerant(quint64 w, quint64 h, uint typesPerPixel)
{
    return new_failureTolerant<unsigned char>(w, h, typesPerPixel);
}

unsigned short* DImgLoader::new_short_failureTolerant(size_t unsecureSize)
{
    return new_failureTolerant<unsigned short>(unsecureSize);
}

unsigned short* DImgLoader::new_short_failureTolerant(quint64 w, quint64 h, uint typesPerPixel)
{
    return new_failureTolerant<unsigned short>(w, h, typesPerPixel);
}

} // namespace Digikam
