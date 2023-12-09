#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set( nominatim_SRCS OsmNominatimReverseGeocodingRunner.cpp NominatimReverseGeocodingPlugin.cpp )

marble_add_plugin( NominatimReverseGeocodingPlugin ${nominatim_SRCS} )
