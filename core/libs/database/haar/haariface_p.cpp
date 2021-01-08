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

void DatabaseBlob::read(const QByteArray& array, Haar::SignatureData* const data)
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
        stream >> data->avg[i];
    }

    // read coefficients

    for (int i = 0 ; i < 3 ; ++i)
    {
        for (int j = 0 ; j < Haar::NumberOfCoefficients ; ++j)
        {
            stream >> data->sig[i][j];
        }
    }
}

QByteArray DatabaseBlob::write(Haar::SignatureData* const data)
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
        stream << data->avg[i];
    }

    // write coefficients

    for (int i = 0 ; i < 3 ; ++i)
    {
        for (int j = 0 ; j < Haar::NumberOfCoefficients ; ++j)
        {
            stream << data->sig[i][j];
        }
    }

    return array;
}

// -----------------------------------------------------------------------------------------------------

HaarIface::Private::Private()
    : useSignatureCache (false),
      data              (nullptr),
      bin               (nullptr),
      signatureCache    (nullptr),
      albumCache        (nullptr),
      signatureQuery    (QString::fromUtf8("SELECT imageid, matrix FROM ImageHaarMatrix;"))
{
}

HaarIface::Private::~Private()
{
    delete data;
    delete bin;
    delete signatureCache;
    delete albumCache;
}

void HaarIface::Private::createLoadingBuffer()
{
    if (!data)
    {
        data = new Haar::ImageData;
    }
}

void HaarIface::Private::createWeightBin()
{
    if (!bin)
    {
        bin = new Haar::WeightBin;
    }
}

void HaarIface::Private::setSignatureCacheEnabled(bool cache, const QSet<qlonglong>& imageIds)
{
    setSignatureCacheEnabled(cache);

    // stop here if we disable cached signatures

    if (!cache || imageIds.isEmpty())
    {
        return;
    }

    // Remove all ids from the fully created signatureCache that are not needed for the duplicates search.
    // This is usually faster then starting a query for every single id in imageIds.

    for (SignatureCache::iterator it = signatureCache->begin() ;
         it != signatureCache->end() ; )
    {
        if (!imageIds.contains(it.key()))
        {
            it = signatureCache->erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void HaarIface::Private::setSignatureCacheEnabled(bool cache)
{
    delete signatureCache;
    signatureCache    = nullptr;
    delete albumCache;
    albumCache        = nullptr;

    if (cache)
    {
        signatureCache = new SignatureCache();
        albumCache     = new AlbumCache();
    }

    useSignatureCache = cache;

    // stop here if we disable cached signatures

    if (!cache)
    {
        return;
    }

    // Variables for data read from DB

    DatabaseBlob        blob;
    qlonglong           imageid;
    int                 albumid;
    Haar::SignatureData targetSig;

    // reference for easier access

    SignatureCache& signatureCache = *this->signatureCache;
    AlbumCache&     albumCache     = *this->albumCache;

    DbEngineSqlQuery query         = SimilarityDbAccess().backend()->prepareQuery(signatureQuery);

    if (!SimilarityDbAccess().backend()->exec(query))
    {
        return;
    }

    const QHash<qlonglong, QPair<int, int> >& itemAlbumHash = CoreDbAccess().db()->getAllItemsWithAlbum();

    while (query.next())
    {
        imageid = query.value(0).toLongLong();

        if (itemAlbumHash.contains(imageid))
        {
            blob.read(query.value(1).toByteArray(), &targetSig);
            albumid                 = itemAlbumHash.value(imageid).second;
            signatureCache[imageid] = targetSig;
            albumCache[imageid]     = albumid;
        }
    }
}

} // namespace Digikam
