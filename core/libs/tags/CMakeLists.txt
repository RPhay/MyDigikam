#
# SPDX-FileCopyrightText: 2010-2023 by Gilles Caulier, <caulier dot gilles at gmail dot com>
# SPDX-FileCopyrightText: 2015      by Veaceslav Munteanu, <veaceslav dot munteanu90 at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

APPLY_COMMON_POLICIES()

kde_enable_exceptions()

include_directories(
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Xml,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Sql,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Gui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Core,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt${QT_VERSION_MAJOR}::Concurrent,INTERFACE_INCLUDE_DIRECTORIES>

    $<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::I18n,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::WidgetsAddons,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::XmlGui,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::Solid,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>

    ${CMAKE_CURRENT_SOURCE_DIR}/autoassignment/model
)

set(libtags_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/addtagscombobox.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/addtagslineedit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tagcheckview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tagfolderview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tagslineeditoverlay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tagspopupmenu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets/tageditdlg.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/engine/tagscompleter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/tagmodificationhelper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/tagsactionmngr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/taggingaction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/engine/taggingactionfactory.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/manager/tagsmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/tagpropwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/taglist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/tagmngrtreeview.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/manager/models/tagmngrlistview.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/models/tagmngrlistmodel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/manager/models/tagmngrlistitem.cpp
)

set(autotagassignment_LIB_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/autoassignment/model/dnnbasedetectormodel.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/autoassignment/model/dnnyolodetector.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/autoassignment/model/dnnresnetdetector.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/autoassignment/autotagsassign.cpp
)

if(KF${QT_VERSION_MAJOR}IconThemes_FOUND)
    include_directories($<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::IconThemes,INTERFACE_INCLUDE_DIRECTORIES>)

    if (Qt6_FOUND)

        include_directories($<TARGET_PROPERTY:KF${QT_VERSION_MAJOR}::IconWidgets,INTERFACE_INCLUDE_DIRECTORIES>)

    endif()

endif()

# Used by digikamgui
add_library(gui_digikamtags_obj OBJECT ${libtags_SRCS})

target_compile_definitions(gui_digikamtags_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

# Used by digikamgui
add_library(gui_digikamautotagassignment_obj
            OBJECT
            ${autotagassignment_LIB_SRCS}
)

target_compile_definitions(gui_digikamautotagassignment_obj
                           PRIVATE
                           digikamgui_EXPORTS
)

# For unit-tests
add_library(digikamautotagassignment STATIC $<TARGET_OBJECTS:gui_digikamautotagassignment_obj>)
