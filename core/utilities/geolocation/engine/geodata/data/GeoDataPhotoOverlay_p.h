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

#include "GeoDataOverlay_p.h"
#include "GeoDataTypes.h"
#include "GeoDataPoint.h"
#include "GeoDataImagePyramid.h"
#include "GeoDataViewVolume.h"

namespace Marble
{

struct Q_DECL_HIDDEN GeoDataPhotoOverlayPrivate : public GeoDataOverlayPrivate
{
public:

    GeoDataPhotoOverlayPrivate();
    GeoDataPhotoOverlayPrivate(const GeoDataPhotoOverlayPrivate& other);

public:

    qreal                       m_rotation;
    GeoDataViewVolume           m_viewVolume;
    GeoDataImagePyramid         m_imagePyramid;
    GeoDataPoint                m_point;
    GeoDataPhotoOverlay::Shape  m_shape;
};

GeoDataPhotoOverlayPrivate::GeoDataPhotoOverlayPrivate()
    : m_rotation( 0.0 ),
      m_viewVolume(),
      m_imagePyramid(),
      m_point(),
      m_shape( GeoDataPhotoOverlay::Rectangle )
{
    // nothing to do
}

GeoDataPhotoOverlayPrivate::GeoDataPhotoOverlayPrivate(const GeoDataPhotoOverlayPrivate& other)
    : GeoDataOverlayPrivate(other),
      m_rotation(other.m_rotation),
      m_viewVolume(other.m_viewVolume),
      m_imagePyramid(other.m_imagePyramid),
      m_point(other.m_point),
      m_shape(other.m_shape)
{
}

} // namespace Marble
