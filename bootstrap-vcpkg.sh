#!/bin/bash

export PATH=$PATH:/c/bison:/c/icoutils/bin:$INSTALL_DIR/$VCPKG_TRIPLET/tools/gperf
echo "PATH=$PATH"

mkdir build.vcpkg
cd build.vcpkg

cmake .. -DCMAKE_TOOLCHAIN_FILE=/c/vcpkg/scripts/buildsystems/vcpkg.cmake \
                           -DVCPKG_TARGET_TRIPLET=x64-windows \
                           -DCMAKE_COLOR_MAKEFILE=ON \
						   -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                           -DCMAKE_INSTALL_PREFIX=/e/dk/x64-windows \
                           -DINSTALL_ROOT=/e/dk/x64-windows \
                           -DBUILD_TESTING=ON \
						   -DBUILD_WITH_QT6=ON \
						   -DDIGIKAMSC_COMPILE_PO=OFF \
						   -DDIGIKAMSC_COMPILE_DIGIKAM=ON \
						   -DENABLE_KFILEMETADATASUPPORT=OFF \
						   -DENABLE_AKONADICONTACTSUPPORT=OFF \
						   -DENABLE_MYSQLSUPPORT=ON \
						   -DENABLE_INTERNALMYSQL=ON \
						   -DENABLE_MEDIAPLAYER=ON \
						   -DENABLE_DBUS=OFF \
						   -DENABLE_APPSTYLES=ON \
						   -DENABLE_QWEBENGINE=ON \
						   -DENABLE_QTMULTIMEDIA=ON \
						   -Wno-dev
							
#                           -DCMAKE_BUILD_TYPE=Debug \
