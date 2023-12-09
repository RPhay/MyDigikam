#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

set( compass_SRCS CompassFloatItem.cpp )
set( compass_UI CompassConfigWidget.ui )

qt_wrap_ui( compass_SRCS ${compass_UI} )

qt_add_resources( compass_SRCS compass.qrc )

marble_add_plugin( CompassFloatItem ${compass_SRCS} )
