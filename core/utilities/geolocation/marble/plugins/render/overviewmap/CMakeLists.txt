#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set( overviewmap_SRCS OverviewMap.cpp )
set( overviewmap_UI OverviewMapConfigWidget.ui )

qt_wrap_ui(overviewmap_SRCS ${overviewmap_UI})

marble_add_plugin( OverviewMap ${overviewmap_SRCS} )
