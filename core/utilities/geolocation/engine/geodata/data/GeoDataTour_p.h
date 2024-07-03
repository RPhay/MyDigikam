/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

#include "GeoDataFeature_p.h"
#include "GeoDataTypes.h"
#include "GeoDataPlaylist.h"

namespace Marble
{

class GeoDataTourPrivate : public GeoDataFeaturePrivate
{
public:

    GeoDataTourPrivate()
        : m_playlist( nullptr )
    {
        // nothing to do
    }

    GeoDataTourPrivate(const GeoDataTourPrivate &other)
      : GeoDataFeaturePrivate(other),
        m_playlist(nullptr)
    {
        if (other.m_playlist) {
            m_playlist = new GeoDataPlaylist(*other.m_playlist);
        }
    }

    ~GeoDataTourPrivate() override
    {
        delete m_playlist;
    }

    GeoDataTourPrivate& operator=( const GeoDataTourPrivate &other )
    {
        if ( this == &other ) {
            return *this;
        }

        GeoDataFeaturePrivate::operator=( other );

        GeoDataPlaylist *newPlaylist = nullptr;
        if ( other.m_playlist ) {
            newPlaylist = new GeoDataPlaylist( *other.m_playlist );
        }
        delete m_playlist;
        m_playlist = newPlaylist;

        return *this;
    }

    GeoDataPlaylist* m_playlist = nullptr;
};

} // namespace Marble
