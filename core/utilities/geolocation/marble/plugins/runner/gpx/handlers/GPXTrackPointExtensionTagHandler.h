// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Niko Sams <niko.sams@gmail.com>
//

#ifndef MARBLE_GPX_TRACK_POINT_EXTENSION_TAG_HANDLER_H
#define MARBLE_GPX_TRACK_POINT_EXTENSION_TAG_HANDLER_H

#include "GeoTagHandler.h"

namespace Marble
{
namespace gpx
{

class GPXTrackPointExtensionTagHandler : public GeoTagHandler
{
public:
    GeoNode* parse(GeoParser&) const override;
};

}
}

#endif
