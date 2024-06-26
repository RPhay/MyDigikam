// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#pragma once

#include "GeoDataFeature.h"
#include "digikam_export.h"

namespace Marble
{

class GeoDataTourPrivate;
class GeoDataPlaylist;
class GeoDataSoundCue;

/**
 */
class DIGIKAM_EXPORT GeoDataTour : public GeoDataFeature
{
public:
    GeoDataTour();
    GeoDataTour(const GeoDataTour& other);
    ~GeoDataTour() override;

    GeoDataTour& operator=(const GeoDataTour& other);

    bool operator==(const GeoDataTour &other) const;
    bool operator!=(const GeoDataTour &other) const;

    GeoDataFeature * clone() const override;

    GeoDataPlaylist* playlist();
    const GeoDataPlaylist* playlist() const;
    void setPlaylist(GeoDataPlaylist* playlist);

    const char* nodeType() const override;

    static const GeoDataTour null;

private:
    Q_DECLARE_PRIVATE(GeoDataTour)

};

} // namespace Marble
