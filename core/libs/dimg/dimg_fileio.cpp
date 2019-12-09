/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-06-14
 * Description : digiKam 8/16 bits image management API
 *               Files input output
 *
 * Copyright (C) 2005-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "dimg.h"
#include "dimg_p.h"

namespace Digikam
{

bool DImg::loadItemInfo(const QString& filePath, bool loadMetadata, bool loadICCData, bool loadUniqueHash, bool loadImageHistory)
{
    DImgLoader::LoadFlags loadFlags = DImgLoader::LoadItemInfo;

    if (loadMetadata)
    {
        loadFlags |= DImgLoader::LoadMetadata;
    }

    if (loadICCData)
    {
        loadFlags |= DImgLoader::LoadICCData;
    }

    if (loadUniqueHash)
    {
        loadFlags |= DImgLoader::LoadUniqueHash;
    }

    if (loadImageHistory)
    {
        loadFlags |= DImgLoader::LoadImageHistory;
    }

    return load(filePath, loadFlags, nullptr, DRawDecoding());
}

bool DImg::load(const QString& filePath,
                DImgLoaderObserver* const observer,
                const DRawDecoding& rawDecodingSettings)
{
    return load(filePath, DImgLoader::LoadAll, observer, rawDecodingSettings);
}

bool DImg::load(const QString& filePath,
                bool loadMetadata,
                bool loadICCData,
                bool loadUniqueHash,
                bool loadImageHistory,
                DImgLoaderObserver* const observer,
                const DRawDecoding& rawDecodingSettings)
{
    DImgLoader::LoadFlags loadFlags = DImgLoader::LoadItemInfo | DImgLoader::LoadImageData;

    if (loadMetadata)
    {
        loadFlags |= DImgLoader::LoadMetadata;
    }

    if (loadICCData)
    {
        loadFlags |= DImgLoader::LoadICCData;
    }

    if (loadUniqueHash)
    {
        loadFlags |= DImgLoader::LoadUniqueHash;
    }

    if (loadImageHistory)
    {
        loadFlags |= DImgLoader::LoadImageHistory;
    }

    return load(filePath, loadFlags, observer, rawDecodingSettings);
}

bool DImg::load(const QString& filePath,
                int loadFlagsInt,
                DImgLoaderObserver* const observer,
                const DRawDecoding& rawDecodingSettings)
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isReadable())
    {
        qCDebug(DIGIKAM_DIMG_LOG) << "File" << filePath << "does not exist";
        return false;
    }

    QList<DPluginDImg*> pluginList  = m_priv->pluginsForFile(fileInfo, false);
    DImgLoader::LoadFlags loadFlags = (DImgLoader::LoadFlags)loadFlagsInt;
    setAttribute(QLatin1String("originalFilePath"), filePath);

    FileReadLocker lock(filePath);

    // First step we check the file extension to find the right loader.

    foreach (DPluginDImg* const plug, pluginList)
    {
        if (observer && !observer->continueQuery())
        {
            return false;
        }

        if (loadFlags & DImgLoader::LoadPreview && !plug->previewSupported())
        {
            continue;
        }

        qCDebug(DIGIKAM_DIMG_LOG) << filePath << ":" << plug->loaderName() << "file identified";
        FORMAT format            = m_priv->loaderNameToFormat(plug->loaderName());
        DImgLoader* const loader = plug->loader(this, rawDecodingSettings);
        setAttribute(QLatin1String("detectedFileFormat"), format);
        loader->setLoadFlags(loadFlags);

        if (loader->load(filePath, observer))
        {
            m_priv->null       = !loader->hasLoadedData();
            m_priv->alpha      = loader->hasAlpha();
            m_priv->sixteenBit = loader->sixteenBit();
            setAttribute(QLatin1String("isReadOnly"), loader->isReadOnly());
            delete loader;

            return true;
        }

        delete loader;
    }

    if (observer && !observer->continueQuery())
    {
        return false;
    }

    // In the second step we check the magic bytes to find the right loader.

    pluginList = m_priv->pluginsForFile(fileInfo, true);

    foreach (DPluginDImg* const plug, pluginList)
    {
        if (observer && !observer->continueQuery())
        {
            return false;
        }

        if (loadFlags & DImgLoader::LoadPreview && !plug->previewSupported())
        {
            continue;
        }

        qCDebug(DIGIKAM_DIMG_LOG) << filePath << ":" << plug->loaderName() << "file identified (magic)";
        FORMAT format            = m_priv->loaderNameToFormat(plug->loaderName());
        DImgLoader* const loader = plug->loader(this, rawDecodingSettings);
        setAttribute(QLatin1String("detectedFileFormat"), format);
        loader->setLoadFlags(loadFlags);

        if (loader->load(filePath, observer))
        {
            m_priv->null       = !loader->hasLoadedData();
            m_priv->alpha      = loader->hasAlpha();
            m_priv->sixteenBit = loader->sixteenBit();
            setAttribute(QLatin1String("isReadOnly"), loader->isReadOnly());
            delete loader;

            return true;
        }

        delete loader;
    }

    if (pluginList.isEmpty() && !(loadFlags & DImgLoader::LoadPreview))
    {
        qCWarning(DIGIKAM_DIMG_LOG) << filePath << ": Unknown image format !!!";
        return false;
    }

    if (observer                  &&
        observer->continueQuery() &&
        !(loadFlags & DImgLoader::LoadPreview))
    {
        qCWarning(DIGIKAM_DIMG_LOG) << filePath << ": Cannot load file !!!";
    }

    return false;
}

QString DImg::formatToMimeType(FORMAT frm)
{
    QString format;

    switch (frm)
    {
        case (NONE):
        {
            return format;
        }

        case (JPEG):
        {
            format = QLatin1String("JPG");
            break;
        }

        case (TIFF):
        {
            format = QLatin1String("TIF");
            break;
        }

        case (PNG):
        {
            format = QLatin1String("PNG");
            break;
        }

        case (JP2K):
        {
            format = QLatin1String("JP2");
            break;
        }

        case (PGF):
        {
            format = QLatin1String("PGF");
            break;
        }

        default:
        {
            // For QImage or ImageMagick based.
            break;
        }
    }

    return format;
}

bool DImg::save(const QString& filePath, FORMAT frm, DImgLoaderObserver* const observer)
{
    if (isNull())
    {
        return false;
    }

    return(save(filePath, formatToMimeType(frm), observer));
}

bool DImg::save(const QString& filePath, const QString& format, DImgLoaderObserver* const observer)
{
    qCDebug(DIGIKAM_DIMG_LOG) << "Saving to " << filePath << " with format: " << format;

    if (isNull())
    {
        return false;
    }

    if (format.isEmpty())
    {
        return false;
    }

    QString frm = format.toUpper();
    setAttribute(QLatin1String("savedFilePath"), filePath);

    FileWriteLocker lock(filePath);

    DPluginDImg* const plug = m_priv->pluginForFormat(frm);
    DImg copyForSave        = copy();

    if (frm == QLatin1String("JPEG") || frm == QLatin1String("JPG") || frm == QLatin1String("JPE"))
    {
        // JPEG does not support transparency, so we shall provide an image without alpha channel.
        // This is only necessary if the image has an alpha channel, and there are actually transparent pixels
        if (hasTransparentPixels())
        {
            copyForSave.removeAlphaChannel();
        }
    }

    if (plug)
    {
        DImgLoader* const loader = plug->loader(&copyForSave);
        copyForSave.setAttribute(QLatin1String("savedFormat-isReadOnly"), loader->isReadOnly());
        bool ret                 = loader->save(filePath, observer);
        delete loader;

        return ret;
    }

    qCWarning(DIGIKAM_DIMG_LOG) << filePath << " : Unknown save format !!!";

    return false;
}

DImg::FORMAT DImg::fileFormat(const QString& filePath)
{
    FORMAT format = DImg::NONE;

    if (filePath.isEmpty())
    {
        return format;
    }

    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists() || !fileInfo.isReadable())
    {
        return format;
    }

    QList<DPluginDImg*> pluginList = DImg::Private::pluginsForFile(fileInfo, false);

    if (!pluginList.isEmpty())
    {
        QString name = pluginList.first()->loaderName();
        format       = DImg::Private::loaderNameToFormat(name);
    }

    return format;
}

QDateTime DImg::creationDateFromFilesystem(const QFileInfo& fileInfo) const
{
    // creation date is not what it seems on Unix
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    QDateTime ctime = fileInfo.birthTime();
#else
    QDateTime ctime = fileInfo.created();
#endif

    QDateTime mtime = fileInfo.lastModified();

    if (ctime.isNull())
    {
        return mtime;
    }

    if (mtime.isNull())
    {
        return ctime;
    }

    return qMin(ctime, mtime);
}

} // namespace Digikam
