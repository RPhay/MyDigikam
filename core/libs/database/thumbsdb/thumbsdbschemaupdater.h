/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-05-29
 * Description : Thumbnail DB schema update
 *
 * SPDX-FileCopyrightText: 2007-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_THUMBS_DB_SCHEMA_UPDATER_H
#define DIGIKAM_THUMBS_DB_SCHEMA_UPDATER_H

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class ThumbsDbAccess;
class InitializationObserver;

class DIGIKAM_EXPORT ThumbsDbSchemaUpdater
{
public:

    static int schemaVersion();

public:

    explicit ThumbsDbSchemaUpdater(ThumbsDbAccess* const dbAccess);
    ~ThumbsDbSchemaUpdater();

    bool update();
    void setObserver(InitializationObserver* const observer);

private:

    bool startUpdates();
    bool makeUpdates();
    bool createDatabase();
    bool createTables();
    bool createIndices();
    bool createTriggers();
    bool updateV1ToV2();
    bool updateV2ToV3();

private:

    // Disable
    ThumbsDbSchemaUpdater(const ThumbsDbSchemaUpdater&)            = delete;
    ThumbsDbSchemaUpdater& operator=(const ThumbsDbSchemaUpdater&) = delete;

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_THUMBS_DB_SCHEMA_UPDATER_H
