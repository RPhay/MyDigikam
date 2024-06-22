/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-05-29
 * Description : Face database access wrapper.
 *
 * SPDX-FileCopyrightText: 2007-2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "digikam_export.h"
#include "dbengineparameters.h"
#include "collectionscannerobserver.h"

namespace Digikam
{

class FaceDb;
class FaceDbAccessStaticPriv;
class FaceDbBackend;
class DbEngineErrorHandler;

class DIGIKAM_GUI_EXPORT FaceDbAccess
{
public:

    /**
     * This class is written in analogy to CoreDbAccess
     * (some features stripped off).
     * For documentation, see coredbaccess.h
     */
    FaceDbAccess();
    ~FaceDbAccess();

    FaceDb*        db()         const;
    FaceDbBackend* backend()    const;
    QString        lastError()  const;

    /**
     * Set the "last error" message. This method is not for public use.
     */
    void setLastError(const QString& error);

    static DbEngineParameters parameters();

public:

    static void initDbEngineErrorHandler(DbEngineErrorHandler* const errorhandler);
    static void setParameters(const DbEngineParameters& parameters);
    static bool checkReadyForUse(InitializationObserver* const observer);
    static bool isInitialized();
    static void cleanUpDatabase();

private:

    explicit FaceDbAccess(bool);

    // Disable
    FaceDbAccess(const FaceDbAccess&)            = delete;
    FaceDbAccess& operator=(const FaceDbAccess&) = delete;

private:

    friend class FaceDbAccessUnlock;
    static FaceDbAccessStaticPriv* d;
};

// ------------------------------------------------------------------------------------------

class FaceDbAccessUnlock
{
public:

    /**
     * Acquire an object of this class if you want to assure
     * that the FaceDbAccess is _not_ held during the lifetime of the object.
     * At creation, the lock is obtained shortly, then all locks are released.
     * At destruction, all locks are acquired again.
     * If you need to access any locked structures during lifetime, acquire a new
     * FaceDbAccess.
     */
    FaceDbAccessUnlock();
    explicit FaceDbAccessUnlock(FaceDbAccess* const access);
    ~FaceDbAccessUnlock();

private:

    int count = 0;

private:

    // Disable
    FaceDbAccessUnlock(const FaceDbAccessUnlock&)            = delete;
    FaceDbAccessUnlock& operator=(const FaceDbAccessUnlock&) = delete;
};

} // namespace Digikam
