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

#ifndef DIGIKAM_HAAR_IFACE_P_H
#define DIGIKAM_HAAR_IFACE_P_H

#include "haariface.h"

// C++ includes

#include <fstream>
#include <cmath>
#include <cstring>

// Qt includes

#include <QByteArray>
#include <QDataStream>
#include <QImage>
#include <QImageReader>
#include <QMap>

// Local includes

#include "digikam_debug.h"
#include "jpegutils.h"
#include "dimg.h"
#include "iteminfo.h"
#include "coredbaccess.h"
#include "coredbtransaction.h"
#include "coredb.h"
#include "coredbbackend.h"
#include "coredbsearchxml.h"
#include "dbenginesqlquery.h"
#include "similaritydb.h"
#include "similaritydbaccess.h"

using namespace std;

// TODO: Always store similarities in the similaritydb

namespace Digikam
{

typedef QMap<qlonglong, Haar::SignatureData> SignatureCache;
typedef QMap<qlonglong, int>                 AlbumCache;

/**
 * This class encapsulates the Haar signature in a QByteArray
 * that can be stored as a BLOB in the database.
 *
 * Reading and writing is done in a platform-independent manner, which
 * induces a certain overhead, but which is necessary IMO.
 */
class Q_DECL_HIDDEN DatabaseBlob
{
public:

    enum
    {
        Version = 1
    };

public:

    DatabaseBlob() = default;

    /**
     * Read the QByteArray into the Haar::SignatureData.
     */
    void read(const QByteArray& array, Haar::SignatureData* const data);

    QByteArray write(Haar::SignatureData* const data);
};

// -----------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN HaarIface::Private
{
public:

    explicit Private();
    ~Private();

public:

    void createLoadingBuffer();
    void createWeightBin();
    void setSignatureCacheEnabled(bool cache, const QSet<qlonglong>& imageIds);
    void setSignatureCacheEnabled(bool cache);

public:

    bool             useSignatureCache;
    Haar::ImageData* data;
    Haar::WeightBin* bin;
    SignatureCache*  signatureCache;
    AlbumCache*      albumCache;

    QString          signatureQuery;
    QSet<int>        albumRootsToSearch;
};

} // namespace Digikam

#endif // DIGIKAM_HAAR_IFACE_P_H
