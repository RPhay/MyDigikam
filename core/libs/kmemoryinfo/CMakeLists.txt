#
# Copyright (c) 2010-2021 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# Copyright (c) 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

APPLY_COMMON_POLICIES()

include_directories($<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>)

set(libkmemoryinfo_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/kmemoryinfo.cpp)

# Used by digikamcore
add_library(core_kmemoryinfo_obj OBJECT ${libkmemoryinfo_SRCS})

target_compile_definitions(core_kmemoryinfo_obj
                           PRIVATE
                           digikamcore_EXPORTS
)
