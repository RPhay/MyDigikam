/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-09-19
 * Description : Scanning a single item - private containers.
 *
 * SPDX-FileCopyrightText: 2007-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_SCANNER_P_H
#define DIGIKAM_ITEM_SCANNER_P_H

#include "itemscanner.h"

// Qt includes

#include <QImageReader>
#include <QElapsedTimer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "coredburl.h"
#include "coredbaccess.h"
#include "coredb.h"
#include "similaritydbaccess.h"
#include "similaritydb.h"
#include "collectionlocation.h"
#include "collectionmanager.h"
#include "facetagseditor.h"
#include "itemcomments.h"
#include "itemcopyright.h"
#include "itemextendedproperties.h"
#include "itemhistorygraph.h"
#include "metaenginesettings.h"
#include "tagregion.h"
#include "tagscache.h"
#include "iostream"
#include "dimagehistory.h"
#include "itemhistorygraphdata.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemScannerCommit
{

public:

    enum Operation
    {
        NoOp,
        AddItem,
        UpdateItem
    };

public:

    ItemScannerCommit();

public:

    Operation                        operation;

    qlonglong                        copyImageAttributesId;

    bool                             commitItemInformation;
    bool                             commitImageMetadata;
    bool                             commitVideoMetadata;
    bool                             commitItemPosition;
    bool                             commitItemComments;
    bool                             commitItemCopyright;
    bool                             commitFaces;
    bool                             commitIPTCCore;
    bool                             hasColorTag;
    bool                             hasPickTag;

    DatabaseFields::ItemInformation  imageInformationFields;
    QVariantList                     imageInformationInfos;

    QVariantList                     imageMetadataInfos;
    QVariantList                     imagePositionInfos;

    CaptionsMap                      captions;
    QString                          headline;
    CaptionsMap                      titles;

    Template                         copyrightTemplate;
    QMultiMap<QString, QVariant>     metadataFacesMap;

    QVariantList                     iptcCoreMetadataInfos;

    QList<int>                       tagIds;
    QString                          historyXml;
    QString                          uuid;
};

// ---------------------------------------------------------------------------------------

class Q_DECL_HIDDEN LessThanByProximityToSubject
{
public:

    explicit LessThanByProximityToSubject(const ItemInfo& subject);

    bool operator()(const ItemInfo& a, const ItemInfo& b);

public:

    ItemInfo subject;
};

// ---------------------------------------------------------------------------

class Q_DECL_HIDDEN ItemScanner::Private
{
public:

    explicit Private();
    ~Private();

public:

    bool                   hasImage;
    bool                   hasMetadata;
    bool                   loadedFromDisk;

    QFileInfo              fileInfo;

    DMetadata*             metadata;
    DImg                   img;
    ItemScanInfo           scanInfo;
    ItemScanner::ScanMode  scanMode;

    bool                   hasHistoryToResolve;

    ItemScannerCommit      commit;

    QElapsedTimer          timer;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_SCANNER_P_H
