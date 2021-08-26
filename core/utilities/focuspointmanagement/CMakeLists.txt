APPLY_COMMON_POLICIES()

set(libfocuspointmanagement_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/focuspointgroup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/focuspointgroup_p.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/focuspointitem.cpp
)

include_directories($<TARGET_PROPERTY:Qt5::Sql,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Gui,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>

                    $<TARGET_PROPERTY:KF5::I18n,INTERFACE_INCLUDE_DIRECTORIES>
                    $<TARGET_PROPERTY:KF5::ConfigCore,INTERFACE_INCLUDE_DIRECTORIES>
)

if(ENABLE_DBUS)
    include_directories($<TARGET_PROPERTY:Qt5::DBus,INTERFACE_INCLUDE_DIRECTORIES>)
endif()

# Used by digikamgui
add_library(gui_focuspointmanagement_obj OBJECT ${libfocuspointmanagement_SRCS})

target_compile_definitions(gui_focuspointmanagement_obj
                           PRIVATE
                           digikamgui_EXPORTS
)
