/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-11
 * Description : shared image loading and caching
 *
 * Copyright (C) 2005-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#include "loadingcache.h"

// Qt includes

#include <QCache>
#include <QMap>

// Local includes

#include "digikam_debug.h"
#include "iccsettings.h"
#include "kmemoryinfo.h"
#include "dmetadata.h"
#include "thumbnailsize.h"

namespace Digikam
{

LoadingProcessListener::LoadingProcessListener()
{
}

LoadingProcessListener::~LoadingProcessListener()
{
}

// -----------------------------------------------------------------------------------

LoadingProcess::LoadingProcess()
{
}

LoadingProcess::~LoadingProcess()
{
}


// -----------------------------------------------------------------------------------

class Q_DECL_HIDDEN LoadingCache::Private
{
public:

    explicit Private(LoadingCache* const q)
      : watch(nullptr),
        q    (q)
    {
    }

    void mapImageFilePath(const QString& filePath, const QString& cacheKey);
    void mapThumbnailFilePath(const QString& filePath, const QString& cacheKey);
    void cleanUpImageFilePathHash();
    void cleanUpThumbnailFilePathHash();
    LoadingCacheFileWatch* fileWatch() const;

public:

    QCache<QString, DImg>           imageCache;
    QCache<QString, QImage>         thumbnailImageCache;
    QCache<QString, QPixmap>        thumbnailPixmapCache;
    QMultiMap<QString, QString>     imageFilePathMap;
    QMultiMap<QString, QString>     thumbnailFilePathMap;
    QHash<LoadingProcess*, QString> loadingDict;

    /// Note: Don't make the mutex recursive, we need to use a wait condition on it
    QMutex                          mutex;

    QWaitCondition                  condVar;

    LoadingCacheFileWatch*          watch;
    LoadingCache*                   q;
};

LoadingCacheFileWatch* LoadingCache::Private::fileWatch() const
{
    // install default watch if no watch is set yet

    if (!watch)
    {
        q->setFileWatch(new LoadingCacheFileWatch);
    }

    return watch;
}

void LoadingCache::Private::mapImageFilePath(const QString& filePath, const QString& cacheKey)
{
    if (imageFilePathMap.size() > (5 * imageCache.size()))
    {
        cleanUpImageFilePathHash();
    }

    imageFilePathMap.insert(filePath, cacheKey);
}

void LoadingCache::Private::mapThumbnailFilePath(const QString& filePath, const QString& cacheKey)
{
    if (thumbnailFilePathMap.size() > (5 * (thumbnailImageCache.size() + thumbnailPixmapCache.size())))
    {
        cleanUpThumbnailFilePathHash();
    }

    thumbnailFilePathMap.insert(filePath, cacheKey);
}

void LoadingCache::Private::cleanUpImageFilePathHash()
{
    // Remove all entries from hash whose value is no longer a key in the cache

    QList<QString> keys = imageCache.keys();
    QMultiMap<QString, QString>::iterator it;

    for (it = imageFilePathMap.begin() ; it != imageFilePathMap.end() ; )
    {
        if (!keys.contains(it.value()))
        {
            it = imageFilePathMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void LoadingCache::Private::cleanUpThumbnailFilePathHash()
{
    QList<QString> keys;

    keys += thumbnailImageCache.keys();
    keys += thumbnailPixmapCache.keys();

    QMultiMap<QString, QString>::iterator it;

    for (it = thumbnailFilePathMap.begin() ; it != thumbnailFilePathMap.end() ; )
    {
        if (!keys.contains(it.value()))
        {
            it = thumbnailFilePathMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

LoadingCache* LoadingCache::m_instance = nullptr;

LoadingCache* LoadingCache::cache()
{
    if (!m_instance)
    {
        m_instance = new LoadingCache;
    }

    return m_instance;
}

void LoadingCache::cleanUp()
{
    delete m_instance;
}

LoadingCache::LoadingCache()
    : d(new Private(this))
{
    KMemoryInfo memory = KMemoryInfo::currentInfo();
    setCacheSize(qBound(60, int(memory.megabytes(KMemoryInfo::TotalRam)*0.05), 200));
    setThumbnailCacheSize(5, 100); // the pixmap number should not be based on system memory, it's graphics memory

    // good place to call it here as LoadingCache is a singleton

    qRegisterMetaType<LoadingDescription>("LoadingDescription");
    qRegisterMetaType<DImg>("DImg");
    qRegisterMetaType<DMetadata>("DMetadata");

    connect(IccSettings::instance(), SIGNAL(signalICCSettingsChanged(ICCSettingsContainer,ICCSettingsContainer)),
            this, SLOT(iccSettingsChanged(ICCSettingsContainer,ICCSettingsContainer)));
}

LoadingCache::~LoadingCache()
{
    delete d->watch;
    delete d;
    m_instance = nullptr;
}

DImg* LoadingCache::retrieveImage(const QString& cacheKey) const
{
    QString filePath(d->imageFilePathMap.key(cacheKey));
    d->fileWatch()->checkFileWatch(filePath);

    return d->imageCache[cacheKey];
}

bool LoadingCache::putImage(const QString& cacheKey, const DImg& img, const QString& filePath) const
{
    bool isInserted = false;

    if (isCacheable(img))
    {
        int cost   = img.numBytes() / 1024;
        isInserted = d->imageCache.insert(cacheKey, new DImg(img), cost);

        if (isInserted && !filePath.isEmpty())
        {
            d->mapImageFilePath(filePath, cacheKey);
            d->fileWatch()->addedImage(filePath);
        }
    }

    return isInserted;
}

void LoadingCache::removeImage(const QString& cacheKey)
{
    d->imageCache.remove(cacheKey);
}

void LoadingCache::removeImages()
{
    d->imageCache.clear();
}

bool LoadingCache::isCacheable(const DImg& img) const
{
    // return whether image fits in cache

    return (quint64)d->imageCache.maxCost() >= (img.numBytes() / 1024);
}

void LoadingCache::addLoadingProcess(LoadingProcess* const process)
{
    d->loadingDict.insert(process, process->cacheKey());
}

LoadingProcess* LoadingCache::retrieveLoadingProcess(const QString& cacheKey) const
{
    return d->loadingDict.key(cacheKey, nullptr);
}

void LoadingCache::removeLoadingProcess(LoadingProcess* const process)
{
    d->loadingDict.remove(process);
}

void LoadingCache::notifyNewLoadingProcess(LoadingProcess* const process, const LoadingDescription& description)
{
    for (QHash<LoadingProcess*, QString>::const_iterator it = d->loadingDict.constBegin() ;
         it != d->loadingDict.constEnd() ; ++it)
    {
        it.key()->notifyNewLoadingProcess(process, description);
    }
}

void LoadingCache::setCacheSize(int megabytes)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "Allowing a cache size of" << megabytes << "MB";
    d->imageCache.setMaxCost(megabytes * 1024);
}

// --- Thumbnails ----

const QImage* LoadingCache::retrieveThumbnail(const QString& cacheKey) const
{
    return d->thumbnailImageCache[cacheKey];
}

const QPixmap* LoadingCache::retrieveThumbnailPixmap(const QString& cacheKey) const
{
    return d->thumbnailPixmapCache[cacheKey];
}

bool LoadingCache::hasThumbnailPixmap(const QString& cacheKey) const
{
    return d->thumbnailPixmapCache.contains(cacheKey);
}

void LoadingCache::putThumbnail(const QString& cacheKey, const QImage& thumb, const QString& filePath)
{

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))

    int cost = thumb.sizeInBytes();

#else

    int cost = thumb.byteCount();

#endif

    if (d->thumbnailImageCache.insert(cacheKey, new QImage(thumb), cost))
    {
        d->mapThumbnailFilePath(filePath, cacheKey);
    }
}

void LoadingCache::putThumbnail(const QString& cacheKey, const QPixmap& thumb, const QString& filePath)
{
    int cost = thumb.width() * thumb.height() * thumb.depth() / 8;

    if (d->thumbnailPixmapCache.insert(cacheKey, new QPixmap(thumb), cost))
    {
        d->mapThumbnailFilePath(filePath, cacheKey);
    }
}

void LoadingCache::removeThumbnail(const QString& cacheKey)
{
    d->thumbnailImageCache.remove(cacheKey);
    d->thumbnailPixmapCache.remove(cacheKey);
}

void LoadingCache::removeThumbnails()
{
    d->thumbnailImageCache.clear();
    d->thumbnailPixmapCache.clear();
}

void LoadingCache::setThumbnailCacheSize(int numberOfQImages, int numberOfQPixmaps)
{
    d->thumbnailImageCache.setMaxCost(numberOfQImages *
                                      ThumbnailSize::maxThumbsSize() *
                                      ThumbnailSize::maxThumbsSize() * 4);

    d->thumbnailPixmapCache.setMaxCost(numberOfQPixmaps *
                                       ThumbnailSize::maxThumbsSize() *
                                       ThumbnailSize::maxThumbsSize() * QPixmap::defaultDepth() / 8);
}

void LoadingCache::setFileWatch(LoadingCacheFileWatch* const watch)
{
    delete d->watch;
    d->watch          = watch;
    d->watch->m_cache = this;
}

QStringList LoadingCache::imageFilePathsInCache() const
{
    d->cleanUpImageFilePathHash();

    return d->imageFilePathMap.uniqueKeys();
}

QStringList LoadingCache::thumbnailFilePathsInCache() const
{
    d->cleanUpThumbnailFilePathHash();

    return d->thumbnailFilePathMap.uniqueKeys();
}

void LoadingCache::notifyFileChanged(const QString& filePath, bool notify)
{
    QList<QString> keys = d->imageFilePathMap.values(filePath);

    foreach (const QString& cacheKey, keys)
    {
        d->imageCache.remove(cacheKey);
    }

    keys = d->thumbnailFilePathMap.values(filePath);

    foreach (const QString& cacheKey, keys)
    {
        d->thumbnailImageCache.remove(cacheKey);
        d->thumbnailPixmapCache.remove(cacheKey);
    }

    if (notify)
    {
        emit fileChanged(filePath);
    }
}

void LoadingCache::iccSettingsChanged(const ICCSettingsContainer& current, const ICCSettingsContainer& previous)
{
    if ((current.enableCM           != previous.enableCM)           ||
        (current.useManagedPreviews != previous.useManagedPreviews) ||
        (current.monitorProfile     != previous.monitorProfile))
    {
        LoadingCache::CacheLock lock(this);
        removeImages();
        removeThumbnails();
    }
}

//---------------------------------------------------------------------------------------------------

LoadingCacheFileWatch::LoadingCacheFileWatch()
    : m_cache(nullptr)
{
}

LoadingCacheFileWatch::~LoadingCacheFileWatch()
{
    if (m_cache)
    {
        LoadingCache::CacheLock lock(m_cache);

        if (m_cache->d->watch == this)
        {
            m_cache->d->watch = nullptr;
        }
    }
}

void LoadingCacheFileWatch::addedImage(const QString& filePath)
{
    if (!m_cache || filePath.isEmpty())
    {
        return;
    }

    QStringList cachePaths = m_cache->imageFilePathsInCache();

    if (m_watchMap.size() > cachePaths.size())
    {
        foreach (const QString& path, m_watchMap.keys())
        {
            if (!cachePaths.contains(path))
            {
                m_watchMap.remove(path);
            }
        }
    }

    QFileInfo info(filePath);

    m_watchMap.insert(filePath, qMakePair(info.size(),
                                          info.lastModified()));
}

void LoadingCacheFileWatch::checkFileWatch(const QString& filePath)
{
    if (!m_cache || filePath.isEmpty())
    {
        return;
    }

    if (m_watchMap.contains(filePath))
    {
        QFileInfo info(filePath);
        QPair<qint64, QDateTime> pair = m_watchMap.value(filePath);

        if ((info.size()         != pair.first) ||
            (info.lastModified() != pair.second))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "LoadingCache file dirty:" << filePath;

            m_cache->notifyFileChanged(filePath);
            m_watchMap.remove(filePath);
        }
    }
}

void LoadingCacheFileWatch::notifyFileChanged(const QString& filePath)
{
    if (m_cache)
    {
        LoadingCache::CacheLock lock(m_cache);
        m_cache->notifyFileChanged(filePath);
    }
}

//---------------------------------------------------------------------------------------------------

LoadingCache::CacheLock::CacheLock(LoadingCache* const cache)
    : m_cache(cache)
{
    m_cache->d->mutex.lock();
}

LoadingCache::CacheLock::~CacheLock()
{
    m_cache->d->mutex.unlock();
}

void LoadingCache::CacheLock::wakeAll()
{
    // obviously the mutex is locked when this function is called

    m_cache->d->condVar.wakeAll();
}

void LoadingCache::CacheLock::timedWait()
{
    // same as above, the mutex is certainly locked

    m_cache->d->condVar.wait(&m_cache->d->mutex, 1000);
}

} // namespace Digikam
