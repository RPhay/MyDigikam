/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-17
 * Description : Haar Database interface
 *
 * Copyright (C) 2016-2018 by Mario Frank <mario dot frank at uni minus potsdam dot de>
 * Copyright (C) 2003      by Ricardo Niederberger Cabral <nieder at mail dot ru>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
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

#include "haariface_p.h"

namespace Digikam
{

void DatabaseBlob::read(const QByteArray& array, Haar::SignatureData& data)
{
    QDataStream stream(array);

    // check version

    qint32 version;
    stream >> version;

    if (version != Version)
    {
        qCDebug(DIGIKAM_DATABASE_LOG) << "Unsupported binary version of Haar Blob in database";
        return;
    }

    stream.setVersion(QDataStream::Qt_4_3);

    // read averages

    for (int i = 0 ; i < 3 ; ++i)
    {
        stream >> data.avg[i];
    }

    // read coefficients

    for (int i = 0 ; i < 3 ; ++i)
    {
        for (int j = 0 ; j < Haar::NumberOfCoefficients ; ++j)
        {
            stream >> data.sig[i][j];
        }
    }
}

QByteArray DatabaseBlob::write(Haar::SignatureData& data)
{
    QByteArray array;
    array.reserve(sizeof(qint32) + 3*sizeof(double) + 3*sizeof(qint32)*Haar::NumberOfCoefficients);
    QDataStream stream(&array, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_4_3);

    // write version

    stream << (qint32)Version;

    // write averages

    for (int i = 0 ; i < 3 ; ++i)
    {
        stream << data.avg[i];
    }

    // write coefficients

    for (int i = 0 ; i < 3 ; ++i)
    {
        for (int j = 0 ; j < Haar::NumberOfCoefficients ; ++j)
        {
            stream << data.sig[i][j];
        }
    }

    return array;
}

// -----------------------------------------------------------------------------------------------------

const QString HaarIface::Private::signatureQuery = QString::fromUtf8("SELECT imageid, matrix FROM ImageHaarMatrix;");
const Haar::WeightBin HaarIface::Private::weightBin;

HaarIface::Private::Private()
    : m_data(new Haar::ImageData)
{
}

HaarIface::Private::~Private()
{
}

void HaarIface::Private::rebuildSignatureCache(const QSet<qlonglong>& imageIds)
{
    m_signatureCache.reset(new SignatureCache);
    m_albumCache.reset(new AlbumCache);

    // Variables for data read from DB

    DatabaseBlob        blob;
    qlonglong           imageid;
    int                 albumid;
    Haar::SignatureData targetSig;

    // reference for easier access

    SignatureCache& signatureCache = *m_signatureCache;
    AlbumCache&     albumCache     = *m_albumCache;

    DbEngineSqlQuery query         = SimilarityDbAccess().backend()->prepareQuery(signatureQuery);

    if (!SimilarityDbAccess().backend()->exec(query))
    {
        return;
    }

    QHash<qlonglong, QPair<int, int> > itemAlbumHash = CoreDbAccess().db()->getAllItemsWithAlbum();

    // Remove all ids from the fully created itemAlbumHash that are not needed for the duplicates search.
    // This is usually faster then starting a query for every single id in imageIds.

    if (!imageIds.isEmpty())
    {
        for (auto it = itemAlbumHash.begin() ; it != itemAlbumHash.end() ; )
        {
            if (!imageIds.contains(it.key()))
            {
                it = itemAlbumHash.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    const bool filterByAlbumRoots = !m_albumRootsToSearch.isEmpty();

    while (query.next())
    {
        imageid = query.value(0).toLongLong();

        if (itemAlbumHash.contains(imageid))
        {
            // <albumroootid, albumid>
            const QPair<int, int>& albumPair = itemAlbumHash.value(imageid);

            if (filterByAlbumRoots)
            {
                if (!m_albumRootsToSearch.contains(albumPair.first))
                {
                    continue;
                }
            }

            blob.read(query.value(1).toByteArray(), targetSig);
            albumid                 = albumPair.second;
            signatureCache[imageid] = targetSig;
            albumCache[imageid]     = albumid;
        }
    }
}

bool HaarIface::Private::hasSignatureCache() const
{
    return !(m_signatureCache.isNull() || m_signatureCache->isEmpty());
}

bool HaarIface::Private::retrieveSignatureFromCache(qlonglong imageId, Haar::SignatureData& data)
{
    if (!hasSignatureCache())
    {
        return false;
    }

    if (m_signatureCache->contains(imageId))
    {
        data = m_signatureCache.get()->value(imageId);

        return true;
    }

    return false;
}

void HaarIface::Private::setImageDataFromImage(const QImage& image)
{
    m_data->fillPixelData(image);
}

void HaarIface::Private::setImageDataFromImage(const DImg& image)
{
    m_data->fillPixelData(image);
}

SignatureCache* HaarIface::Private::signatureCache() const
{
    return m_signatureCache.get();
}

AlbumCache* HaarIface::Private::albumCache() const
{
    return m_albumCache.get();
}

Haar::ImageData* HaarIface::Private::imageData() const
{
    return m_data.get();
}

void HaarIface::Private::setAlbumRootsToSearch(const QSet<int>& albumRootIds)
{
    m_albumRootsToSearch = albumRootIds;
}

const QSet<int>& HaarIface::Private::albumRootsToSearch() const
{
    return m_albumRootsToSearch;
}

} // namespace Digikam
