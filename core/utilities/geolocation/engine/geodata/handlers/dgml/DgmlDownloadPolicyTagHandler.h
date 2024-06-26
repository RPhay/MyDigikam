// SPDX-FileCopyrightText: 2009 Jens-Michael Hoffmann <jmho@c-xx.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include "GeoTagHandler.h"

namespace Marble
{

namespace dgml
{

class DgmlDownloadPolicyTagHandler: public GeoTagHandler
{
 public:
    GeoNode* parse( GeoParser & ) const override;
};

} // namespace dgml

} // namespace Marble
