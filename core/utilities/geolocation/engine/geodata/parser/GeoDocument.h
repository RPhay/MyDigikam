/*
    SPDX-FileCopyrightText: 2008 Nikolas Zimmermann <zimmermann@kde.org>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "digikam_export.h"

namespace Marble
{

/**
 * @short A shared base class between GeoDataDocument/GeoSourceDocument
 */
class DIGIKAM_EXPORT GeoDocument
{
protected:

    GeoDocument();

public:

    virtual ~GeoDocument();

    virtual bool isGeoDataDocument() const;
    virtual bool isGeoSceneDocument() const;
};

/**
 * @short A shared base class for all classes that are mapped to a specific tag (ie. GeoFolder)
 */
class DIGIKAM_EXPORT GeoNode
{
public:

    GeoNode();
    virtual ~GeoNode();

    /// Provides type information for downcasting a GeoNode
    virtual const char* nodeType() const = 0;
};

} // namespace Marble
