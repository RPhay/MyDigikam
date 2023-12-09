#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set( navigation_SRCS NavigationFloatItem.cpp ArrowDiscWidget.cpp NavigationButton.cpp NavigationSlider.cpp )
set (navigation_UI navigation.ui)

qt_add_resources(navigation_SRCS navigation.qrc)
qt_wrap_ui(navigation_SRCS ${navigation_UI})

marble_add_plugin( NavigationFloatItem ${navigation_SRCS} )
