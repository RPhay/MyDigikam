/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-01
 * Description : Base-class for backends for geolocation interface
 *
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Michael G. Hansen <mike at mghansen dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "mapbackend.h"

namespace Digikam
{

MapBackend::MapBackend(const QExplicitlySharedDataPointer<GeoIfaceSharedData>& sharedData,
                       QObject* const parent)
    : QObject(parent),
      s      (sharedData)
{
}

MapBackend::~MapBackend()
{
}

void MapBackend::slotThumbnailAvailableForIndex(const QVariant& index, const QPixmap& pixmap)
{
    Q_UNUSED(index)
    Q_UNUSED(pixmap)
}

void MapBackend::slotTrackManagerChanged()
{
}

} // namespace Digikam
