#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set(gpsinfo_SRCS GpsInfo.cpp)
qt_wrap_ui(gpsinfo_SRCS GpsInfoPlugin.ui)

marble_add_plugin(GpsInfo ${gpsinfo_SRCS})
