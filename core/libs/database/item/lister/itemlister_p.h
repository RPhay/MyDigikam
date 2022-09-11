/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Listing information from database - private containers.
 *
 * SPDX-FileCopyrightText: 2007-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer  <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2018      by Mario Frank    <mario dot frank at uni minus potsdam dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ITEM_LISTER_P_H
#define DIGIKAM_ITEM_LISTER_P_H

#include "itemlister.h"

// C++ includes

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cerrno>
#include <limits>

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QRegularExpression>
#include <QDir>

// Local includes

#include "digikam_globals.h"
#include "digikam_debug.h"
#include "coredb.h"
#include "coredbaccess.h"
#include "coredbbackend.h"
#include "collectionmanager.h"
#include "collectionlocation.h"
#include "itemquerybuilder.h"
#include "dmetadata.h"
#include "haariface.h"
#include "dbenginesqlquery.h"
#include "tagscache.h"
#include "itemtagpair.h"
#include "dbjobsthread.h"
#include "dbjobinfo.h"
#include "similaritydbaccess.h"
#include "similaritydb.h"

namespace Digikam
{

/**
 * Used by QSet
 */
inline uint qHash(const ItemListerRecord& key)
{
    return key.imageID;
}

class Q_DECL_HIDDEN ItemLister::Private
{

public:

    explicit Private()
      : recursive              (true),
        listOnlyAvailableImages(true)
    {
    }

public:

    bool recursive;
    bool listOnlyAvailableImages;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_LISTER_P_H
