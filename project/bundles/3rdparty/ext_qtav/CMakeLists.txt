# Script to build QtAV for digiKam bundle.
#
# Copyright (c) 2015-2022 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

set(PREFIX_ext_qtav "${EXTPREFIX}")

set(QtAV_Conf -DBUILD_EXAMPLES=OFF
              -DBUILD_PLAYERS=OFF
              -DBUILD_TESTS=OFF
              -DBUILD_QT5OPENGL=ON
              -DBUILD_QML=OFF
)

JoinListAsString("${QtAV_Conf}" " " BASH_OPTIONS)

if(MINGW)

    ExternalProject_Add(ext_qtav

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/wang-bin/QtAV.git

        CONFIGURE_COMMAND git submodule update --init &&
                          cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/create_manifest.sh <SOURCE_DIR>/. &&
                          cd <SOURCE_DIR> &&
                          <SOURCE_DIR>/create_manifest.sh ${EXTERNALS_DOWNLOAD_DIR} qtav &&
                          rm -rfv ${MXE_INSTALL_PREFIX}/qt5/include/QtAV &&
                          rm -rfv ${MXE_INSTALL_PREFIX}/qt5/include/QtAVWidgets &&
                          rm -fv  ${MXE_INSTALL_PREFIX}/qt5/lib/libQtAV1.a &&
                          rm -fv  ${MXE_INSTALL_PREFIX}/qt5/lib/libQtAVWidgets1.a &&
                          rm -fv  ${MXE_INSTALL_PREFIX}/qt5/lib/libQtAV.dll.a &&
                          rm -fv  ${MXE_INSTALL_PREFIX}/qt5/lib/libQtAVWidgets.dll.a &&
                          rm -fv  ${MXE_INSTALL_PREFIX}/qt5/bin/libQtAV1.dll &&
                          rm -fv  ${MXE_INSTALL_PREFIX}/qt5/bin/libQtAVWidgets1.dll &&
                          rm -fv  ${MXE_INSTALL_PREFIX}/qt5/bin/libQtAV.dll &&
                          rm -fv  ${MXE_INSTALL_PREFIX}/qt5/bin/libQtAVWidgets.dll &&
                          cp ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bootstrap.mxe <SOURCE_DIR>/. &&
                          <SOURCE_DIR>/bootstrap.mxe ${MXE_BUILDROOT} RelWithDebInfo ${BASH_OPTIONS}


        BUILD_COMMAND cd <SOURCE_DIR>/build.mxe && $(MAKE)

        INSTALL_COMMAND cp -rfv <SOURCE_DIR>/src/QtAV ${MXE_INSTALL_PREFIX}/qt5/include/ &&
                        cp -rfv <SOURCE_DIR>/widgets/QtAVWidgets ${MXE_INSTALL_PREFIX}/qt5/include/ &&
                        cp -fv  <SOURCE_DIR>/build.mxe/lib/libQtAV.dll.a ${MXE_INSTALL_PREFIX}/qt5/lib/ &&
                        cp -fv  <SOURCE_DIR>/build.mxe/lib/libQtAVWidgets.dll.a ${MXE_INSTALL_PREFIX}/qt5/lib/ &&
                        cp -fv  <SOURCE_DIR>/build.mxe/bin/libQtAV.dll ${MXE_INSTALL_PREFIX}/qt5/bin/ &&
                        cp -fv  <SOURCE_DIR>/build.mxe/bin/libQtAVWidgets.dll ${MXE_INSTALL_PREFIX}/qt5/bin/

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

elseif(APPLE)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qtav-rules-macos.patch.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/qtav-rules-macos.patch)

    ExternalProject_Add(ext_qtav

        DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

        GIT_REPOSITORY https://github.com/wang-bin/QtAV.git

        PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_BINARY_DIR}/qtav-rules-macos.patch &&
                      rm -fv <SOURCE_DIR>/examples/examples.pro &&
                      rm -fv <SOURCE_DIR>/qml/libQmlAV.pri &&
                      rm -fv <SOURCE_DIR>/qml/libQmlAV.pro

        CONFIGURE_COMMAND git submodule update --init &&
                          cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/create_manifest.sh <SOURCE_DIR>/. &&
                          cd <SOURCE_DIR> &&
                          <SOURCE_DIR>/create_manifest.sh ${EXTERNALS_DOWNLOAD_DIR} qtav &&
                          rm -rfv ${INSTALL_ROOT}/libexec/qt5/lib/QtAV.framework &&
                          rm -rfv ${INSTALL_ROOT}/libexec/qt5/lib/QtAVWidgets.framework &&
                          rm -rfv ${INSTALL_ROOT}/libexec/qt5/include/QtAV &&
                          rm -rfv ${INSTALL_ROOT}/libexec/qt5/include/QtAVWidgets &&
                          mkdir -p build && cd build && qmake ..

        BUILD_COMMAND cd build && $(MAKE)

        INSTALL_COMMAND install_name_tool -id ${INSTALL_ROOT}/libexec/qt5/lib/QtAV.framework/Versions/1/QtAV <SOURCE_DIR>/build/lib_osx_x86_64_llvm/QtAV.framework/QtAV &&
                        install_name_tool -id ${INSTALL_ROOT}/libexec/qt5/lib/QtAVWidgets.framework/Versions/1/QtAVWidgets <SOURCE_DIR>/build/lib_osx_x86_64_llvm/QtAVWidgets.framework/QtAVWidgets &&
                        install_name_tool -change QtAV.framework/Versions/1/QtAV ${INSTALL_ROOT}/libexec/qt5/lib/QtAV.framework/Versions/1/QtAV <SOURCE_DIR>/build/lib_osx_x86_64_llvm/QtAVWidgets.framework/QtAVWidgets &&
                        cp -rfv <SOURCE_DIR>/build/lib_osx_x86_64_llvm/QtAV.framework ${INSTALL_ROOT}/libexec/qt5/lib/ &&
                        cp -rfv <SOURCE_DIR>/build/lib_osx_x86_64_llvm/QtAVWidgets.framework ${INSTALL_ROOT}/libexec/qt5/lib/ &&
                        ln -s ${INSTALL_ROOT}/libexec/qt5/lib/QtAV.framework/Headers ${INSTALL_ROOT}/libexec/qt5/include/QtAV &&
                        ln -s ${INSTALL_ROOT}/libexec/qt5/lib/QtAVWidgets.framework/Headers ${INSTALL_ROOT}/libexec/qt5/include/QtAVWidgets

        BUILD_IN_SOURCE 1

        UPDATE_COMMAND ""
        ALWAYS 0
    )

else() #Linux

    if(${ENABLE_QTVERSION} MATCHES "5.*")

        ExternalProject_Add(ext_qtav

            DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

            GIT_REPOSITORY https://github.com/wang-bin/QtAV.git

            CONFIGURE_COMMAND cd <SOURCE_DIR> && git submodule update --init &&
                              cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/create_manifest.sh <SOURCE_DIR>/. &&
                              cd <SOURCE_DIR> &&
                              <SOURCE_DIR>/create_manifest.sh ${EXTERNALS_DOWNLOAD_DIR} qtav &&
                              rm -rfv ${INSTALL_ROOT}/include/QtAV &&
                              rm -rfv ${INSTALL_ROOT}/include/QtAVWidgets &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAV.so &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAV.so.1 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAV.so.1.13 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAV.so.1.13.0 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAVWidgets.so &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAVWidgets.so.1 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAVWidgets.so.1.13 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAVWidgets.so.1.13.0 &&
                              cd <SOURCE_DIR> && mkdir -p build && cd build &&
                              $(CMAKE_COMMAND) .. -DCMAKE_INSTALL_PREFIX=${PREFIX_ext_qtav}
                                                  -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE}
                                                  ${GLOBAL_PROFILE}
                                                  ${QtAV_Conf}

            BUILD_COMMAND cd <SOURCE_DIR>/build && $(MAKE)

            INSTALL_COMMAND cp -rfv <SOURCE_DIR>/src/QtAV ${INSTALL_ROOT}/include/ &&
                            cp -rfv <SOURCE_DIR>/widgets/QtAVWidgets ${INSTALL_ROOT}/include/ &&
                            cp -fv  <SOURCE_DIR>/build/lib/libQtAV.so ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib/libQtAV.so.1 ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib/libQtAV.so.1.13.0 ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib/libQtAVWidgets.so ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib/libQtAVWidgets.so.1 ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib/libQtAVWidgets.so.1.13.0 ${INSTALL_ROOT}/lib/

            UPDATE_COMMAND ""
            ALWAYS 0
        )

    else()

        ExternalProject_Add(ext_qtav

            DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}

            GIT_REPOSITORY https://github.com/wang-bin/QtAV.git

            PATCH_COMMAND git reset --hard &&
                          ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qtav-qt6-pure.patch &&
                          ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/qtav-rules-qt6.patch &&
                          rm -fv <SOURCE_DIR>/examples/examples.pro &&
                          rm -fv <SOURCE_DIR>/qml/libQmlAV.pri &&
                          rm -fv <SOURCE_DIR>/qml/libQmlAV.pro

            CONFIGURE_COMMAND cd <SOURCE_DIR> && git submodule update --init &&
                              cp -f ${CMAKE_CURRENT_SOURCE_DIR}/../../../scripts/create_manifest.sh <SOURCE_DIR>/. &&
                              cd <SOURCE_DIR> &&
                              <SOURCE_DIR>/create_manifest.sh ${EXTERNALS_DOWNLOAD_DIR} qtav &&
                              rm -rfv ${INSTALL_ROOT}/include/QtAV &&
                              rm -rfv ${INSTALL_ROOT}/include/QtAVWidgets &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAV.so &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAV.so.1 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAV.so.1.13 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAV.so.1.13.0 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAVWidgets.so &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAVWidgets.so.1 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAVWidgets.so.1.13 &&
                              rm -fv  ${INSTALL_ROOT}/lib/libQtAVWidgets.so.1.13.0 &&
                              cd <SOURCE_DIR> && mkdir -p build && cd build &&
                              qmake ..

            BUILD_COMMAND cd <SOURCE_DIR>/build && $(MAKE)

            INSTALL_COMMAND cp -rfv <SOURCE_DIR>/src/QtAV ${INSTALL_ROOT}/include/ &&
                            cp -rfv <SOURCE_DIR>/widgets/QtAVWidgets ${INSTALL_ROOT}/include/ &&
                            cp -fv  <SOURCE_DIR>/build/lib_linux_x86_64_llvm/libQtAV.so ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib_linux_x86_64_llvm/libQtAV.so.1 ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib_linux_x86_64_llvm/libQtAV.so.1.13.0 ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib_linux_x86_64_llvm/libQtAVWidgets.so ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib_linux_x86_64_llvm/libQtAVWidgets.so.1 ${INSTALL_ROOT}/lib/ &&
                            cp -fv  <SOURCE_DIR>/build/lib_linux_x86_64_llvm/libQtAVWidgets.so.1.13.0 ${INSTALL_ROOT}/lib/

            BUILD_IN_SOURCE 1

            UPDATE_COMMAND ""
            ALWAYS 0
        )

    endif()

endif()
