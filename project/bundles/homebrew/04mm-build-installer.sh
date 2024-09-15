#! /bin/bash

# Script to bundle data using previously-built digiKam installation.
# and create a PKG file with Packages application (http://s.sudre.free.fr/Software/Packages/about.html)
# This script must be run as sudo
#
# SPDX-FileCopyrightText: 2015      by Shanti, <listaccount at revenant dot org>
# SPDX-FileCopyrightText: 2015-2024 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

# Ask to run as root
(( EUID != 0 )) && exec sudo -- "$0" "$@"

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

#################################################################################################
# Manage script traces to log file

mkdir -p ./logs
exec > >(tee ./logs/build-installer.full.log) 2>&1

#################################################################################################

echo "04-build-installer.sh : build digiKam bundle PKG."
echo "-------------------------------------------------"

#################################################################################################
# Pre-processing checks

. ./config.sh
. ./common.sh
StartScript
ChecksRunAsRoot
ChecksXCodeCLI
ChecksCPUCores
OsxCodeName
#RegisterRemoteServers

#################################################################################################

# Paths rules
ORIG_PATH="$PATH"
ORIG_WD="`pwd`"

export PATH=$INSTALL_PREFIX/bin:/$INSTALL_PREFIX/sbin:$ORIG_PATH

DKRELEASEID=`cat $ORIG_WD/data/RELEASEID.txt`

#################################################################################################
# Configurations

# Directory where this script is located (default - current directory)
BUILDDIR="$PWD"

# Directory where Packages project files are located
PROJECTDIR="$BUILDDIR/installer"

# Staging area where files to be packaged will be copied
TEMPROOT="$BUILDDIR/$RELOCATE_PREFIX"

DK_APP_CONTENTS="digikam.app/Contents"

# Applications to be launched directly by user (create launch scripts)
KDE_MENU_APPS="\
digikam \
showfoto \
"

# Paths to search for applications above
KDE_APP_PATHS="\
bin \
"

# Other apps - non-MacOS binaries & libraries to be included with required dylibs
OTHER_APPS="\
share/qt/plugins/imageformats/*.dylib \
share/qt/plugins/styles/* \
share/qt/plugins/digikam/bqm/*.so \
share/qt/plugins/digikam/generic/*.so \
share/qt/plugins/digikam/editor/*.so \
share/qt/plugins/digikam/dimg/*.so \
share/qt/plugins/digikam/rawimport/*.so \
opt/mariadb/bin/mysql \
opt/mariadb/bin/mysqld \
opt/mariadb/bin/my_print_defaults \
opt/mariadb/bin/mysqladmin \
opt/mariadb/bin/mysqltest \
opt/mariadb/lib/*.dylib \
opt/mariadb/lib/plugin/*.so \
bin/kbuildsycoca6 \
bin/solid-hardware6 \
bin/ffmpeg \
bin/hunspell \
opt/qt-mariadb/share/qt/plugins/sqldrivers/*.dylib \
opt/qt6/share/qt/plugins/sqldrivers/*.dylib \
opt/qt6/share/qt/plugins/imageformats/*.dylib \
opt/qt6/share/qt/plugins/platforms/*.dylib \
opt/qt6/share/qt/plugins/iconengines/*.dylib \
opt/qt6/share/qt/plugins/generic/*.dylib \
opt/qt6/share/qt/plugins/styles/*.dylib \
lib/libhunspell*.dylib \
"
#FIXME: recompile HomeBrew Qt6 with missing plugins
#opt/qt6/share/qt/plugins/bearer/*.dylib \
#opt/qt6/share/qt/plugins/printsupport/*.dylib \
#opt/qt6/share/qt/plugins/platformthemes/*.dylib \

#lib/sane/*.so \

binaries="$OTHER_APPS"

# Additional Files/Directories - to be copied recursively but not checked for dependencies
# Note: dSYM directories are copied as well and cleaned later if debug symbols must be removed in final bundle.
OTHER_DIRS="\
Cellar/qt/6.7.0_2/lib/QtWebEngineCore.framework/Versions/A/Helpers/QtWebEngineProcess.app \
Cellar/qt/6.7.0_2/lib/QtWebEngineCore.framework/Versions/A/Resources \
opt/qt6/share/qt/translations \
lib/libdigikam*.dSYM \
lib/libgphoto2 \
lib/libgphoto2_port \
opt/mariadb \
share/QtCurve/Breeze.qtcurve \
lib/ImageMagick* \
share/ImageMagick* \
etc/ImageMagick* \
etc/my.cnf
etc/my.cnf.default
etc/my.cnf.d
"

#etc/xdg \
#etc/mariadb$MARIADB_SUFFIX \

#etc/sane.d \

# Additional Data Directories - to be copied recursively
OTHER_DATA="\
share/applications \
share/opencv4 \
share/k* \
share/lensfun \
share/mime \
share/locale \
Library/Application/ \
"

# Packaging tool paths
PACKAGESBUILD="/usr/local/bin/packagesbuild"

echo "digiKam version: $DKRELEASEID"

# ./installer sub-dir must be writable by root
chmod 777 ${PROJECTDIR}

#################################################################################################
# Check if Packages CLI tools are installed

if [[ (! -f "$PACKAGESBUILD") ]] ; then
    echo "Packages CLI tool is not installed"
    echo "See http://s.sudre.free.fr/Software/Packages/about.html for details."
    exit 1
else
    echo "Check Packages CLI tool passed..."
fi

#################################################################################################
# clean up old cache files
if [ -f "$ORIG_WD/found_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/found_dylibs_cache.pkl"
fi

if [ -f "$ORIG_WD/processed_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/processed_dylibs_cache.pkl"
fi
if [ -f "$ORIG_WD/signed_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/signed_dylibs_cache.pkl"
fi

#################################################################################################
# Create temporary dir to build package contents

if [ -d "$TEMPROOT" ] ; then
    echo "---------- Removing temporary packaging directory $TEMPROOT"
    rm -rf "$TEMPROOT"
fi

echo "Creating $TEMPROOT"
mkdir -p "$TEMPROOT"

#################################################################################################
# Prepare applications for MacOS

echo "---------- Preparing Applications for MacOS"

for app in $KDE_MENU_APPS ; do
    echo "  $app"

    # Look for application

    for searchpath in $KDE_APP_PATHS ; do

        # Copy the application if it is found (create directory if necessary)

        if [ -d "$INSTALL_PREFIX/$searchpath/$app.app" ] ; then

            echo "    Found $app in $INSTALL_PREFIX/$searchpath"

            # Copy application directory

            echo "    Copying $app"
            cp -pr "$INSTALL_PREFIX/$searchpath/$app.app" "$TEMPROOT"

            # Add executable to list of binaries for which we need to collect dependencies for

            binaries="$binaries $searchpath/$app.app/Contents/MacOS/$app"

            chmod 755 "$TEMPROOT/$app.app"

        fi

    done

done

#################################################################################################
# Collect dylib dependencies for all binaries,
# then copy them to the staging area (creating directories as required)

echo "---------- Collecting dependencies for applications, binaries, and libraries..."

cd "$INSTALL_PREFIX"

binary_paths=$(ls $binaries | sort -u)

for bin in $binary_paths ; do

    echo "Scan dependencies for $bin"

    # Copy original with parsing

    lib="${bin/$INSTALL_PREFIX\//}"

    if [ ! -e "$TEMPROOT/$lib" ] ; then
        dir="${lib%/*}"

        if [ ! -d "$TEMPROOT/$dir" ] ; then
            mkdir -p "$TEMPROOT/$dir"
        fi

        cp -aH "$INSTALL_PREFIX/$lib" "$TEMPROOT/$dir/"
    fi

    # CopyReccursiveDependencies "$bin" "$TEMPROOT"

done

#################################################################################################
# Copy non-binary files and directories, creating parent directories if needed

echo "---------- Copying binary files..."

for path in $OTHER_APPS ; do
    dir="${path%/*}"

    if [ ! -d "$TEMPROOT/$dir" ] ; then
        echo "  Creating $TEMPROOT/$dir"
        mkdir -p "$TEMPROOT/$dir"
    fi

    echo "  Copying $INSTALL_PREFIX/$path to $TEMPROOT/$dir/"
    cp -aH "$INSTALL_PREFIX/$path" "$TEMPROOT/$dir/"
done

echo "---------- Copying directory contents..."

for path in $OTHER_DIRS ; do
    dir="${path%/*}"

    if [ ! -d "$TEMPROOT/$dir" ] ; then
        echo "  Creating $TEMPROOT/$dir"
        mkdir -p "$TEMPROOT/$dir"
    fi

    echo "   Copying $INSTALL_PREFIX/$path to $TEMPROOT/$dir/"
    cp -aHL "$INSTALL_PREFIX/$path" "$TEMPROOT/$dir/"
done

# See bug #476290
rm -fr $TEMPROOT/libexec/qt6/plugins/imageformats/libqjp2.dylib

echo "---------- Copying data files..."

# Special case with data dirs. QStandardPaths::GenericDataLocation was patched everywhere
# in source code by QStandardPaths::AppDataLocation

for path in $OTHER_DATA ; do
    echo "   Copying $path"
    cp -aL "$INSTALL_PREFIX/$path" "$TEMPROOT/digikam.app/Contents/Resources/"
done

# Copy digiKam hi-colors PNG icons-set to the bundle

mkdir -p "$TEMPROOT/digikam.app/Contents/Resources/icons/"
cp -a "$INSTALL_PREFIX/share/icons/hicolor" "$TEMPROOT/digikam.app/Contents/Resources/icons/"

echo "---------- Copying Qt Web Backend files..."

# Qt Web framework bin data files.
# NOTE: QtWebEngine runtime process is now located in
#       opt/qt6/lib/QtWebEngineCore.framework/Versions/current/Helpers/QtWebEngineProcess.app/Contents/MacOS
#       instead of opt/qt6/libexec/. No needs to make extra rules for this runtime process.

echo "---------- Copying i18n..."

i18nprefix=$INSTALL_PREFIX/share/
cd $i18nprefix

FILES=$(cat $ORIG_WD/logs/build-extralibs.full.log | grep "$INSTALL_PREFIX/share/locale/" | cut -d' ' -f3  | awk '{sub("'"$i18nprefix"'","")}1')

for FILE in $FILES ; do
    rsync -R "./$FILE" "$TEMPROOT/digikam.app/Contents/Resources/"
done

FILES=$(cat $ORIG_WD/logs/build-digikam.full.log | grep "$INSTALL_PREFIX/share/locale/" | cut -d' ' -f3  | awk '{sub("'"$i18nprefix"'","")}1')

for FILE in $FILES ; do
    rsync -R "./$FILE" "$TEMPROOT/digikam.app/Contents/Resources/"
done

# Move Qt translation data files at the right place in the bundle. See Bug #438701.

mv -v $TEMPROOT/opt/qt6/share/qt/translations $TEMPROOT/digikam.app/Contents/Resources/

# To support localized system menu entries from MacOS. See bug #432650.

FILES=$(find "$TEMPROOT/digikam.app/Contents/Resources/locale" -type d -depth 1)

for FILE in $FILES ; do
    BASE=$(basename $FILE)
    echo "Create localized system menu entry for $BASE"
    mkdir "$TEMPROOT/digikam.app/Contents/Resources/$BASE.lproj"
done

# Showfoto resources dir must be merged with digiKam.

cp -a "$TEMPROOT/showfoto.app/Contents/Resources/" "$TEMPROOT/digikam.app/Contents/Resources/"
rm -rf "$TEMPROOT/showfoto.app/Contents/Resources"

cd "$ORIG_WD"

#################################################################################################
# Move digiKam and KF6 run-time plugins to the right place

rm -fr $TEMPROOT/bin/digikam.app
rm -fr $TEMPROOT/bin/showfoto.app

mkdir -p $TEMPROOT/libexec/qt6/
cp -a  $TEMPROOT/share/qt/plugins         $TEMPROOT/libexec/qt6/
rm -rf $TEMPROOT/share/qt/plugins
cp -a  $TEMPROOT/opt/qt6/share/qt/plugins $TEMPROOT/libexec/qt6/
rm -rf $TEMPROOT/opt/qt6/

#################################################################################################
# Merge Manifest files

echo "---------- Copy Git Revisions Manifest\n"

touch $TEMPROOT/digikam.app/Contents/Resources/MANIFEST.txt

FILES=$(ls $ORIG_WD/data/*_manifest.txt)

for FILE in $FILES ; do
    echo $FILE
    cat $FILE >> $TEMPROOT/digikam.app/Contents/Resources/MANIFEST.txt
done

#################################################################################################
# Create package pre-install script

echo "---------- Create package pre-install script"

# Delete /Applications entries, delete existing installation

cat << EOF > "$PROJECTDIR/preinstall"
#!/bin/bash

if [ -d /Applications/digiKam ] ; then
    echo "Removing digiKam directory from Applications folder"
    rm -r /Applications/digiKam
fi

if [ -d /Applications/digikam.app ] ; then
    echo "Removing legacy digikam.app from Applications folder"
    rm -r /Applications/digikam.app
fi

if [ -d /Applications/showfoto.app ] ; then
    echo "Removing legacy showfoto.app from Applications folder"
    rm -r /Applications/showfoto.app
fi

if [ -d "/opt/digikam" ] ; then
    echo "Removing legacy /opt/digikam"
    rm -rf "/opt/digikam"
fi
EOF

# Pre-install script need to be executable

chmod 755 "$PROJECTDIR/preinstall"

#################################################################################################
# Create package post-install script

echo "---------- Create package post-install script"

# Creates Applications menu icons

cat << EOF > "$PROJECTDIR/postinstall"
#!/bin/bash

# NOTE: Disabled with relocate bundle
#for app in $INSTALL_PREFIX/Applications/digiKam/*.app ; do
#    ln -s "\$app" /Applications/\${app##*/}
#done
EOF

# Post-install script need to be executable

chmod 755 "$PROJECTDIR/postinstall"

#################################################################################################
# Copy icons-set resource files.

cp $INSTALL_PREFIX/share/icons/breeze/breeze-icons.rcc           $TEMPROOT/digikam.app/Contents/Resources/breeze.rcc
#cp $INSTALL_PREFIX/share/icons/breeze-dark/breeze-icons-dark.rcc $TEMPROOT/digikam.app/Contents/Resources/breeze-dark.rcc
cp $INSTALL_PREFIX/share/icons/breeze/breeze-icons.rcc $TEMPROOT/digikam.app/Contents/Resources/breeze-dark.rcc

#################################################################################################
# Cleanup symbols in binary files to free space.

echo -e "\n---------- Strip symbols in binary files\n"

if [[ $DK_DEBUG = 1 ]] ; then

    find $TEMPROOT -name "*.so"    | grep -Ev '(digikam|showfoto)' | xargs strip -SXx
    find $TEMPROOT -name "*.dylib" | grep -Ev '(digikam|showfoto)' | xargs strip -SXx

else

    # no debug symbols at all.

    find $TEMPROOT -perm +111 -type f | xargs strip -SXx 2>/dev/null

    # Under MacOS, all debug symbols are stored to separated dSYM sub directories near binary files.

    DSYMDIRS=`find $TEMPROOT -type d -name "*.dSYM"`

    for dsym in $DSYMDIRS ; do

        rm -fr $dsym
        echo "Remove debug symbols from $dsym"

    done

fi

if [[ $DK_DEBUG = 1 ]] ; then

    DEBUG_SUF="-debug"

fi

#################################################################################################
# Relocatable binary files. For details, see these urls:
#
# https://stackoverflow.com/questions/9263256/can-you-please-help-me-understand-how-mach-o-libraries-work-in-mac-os-x
# https://matthew-brett.github.io/docosx/mac_runtime_link.html
# https://thecourtsofchaos.com/2013/09/16/how-to-copy-and-relink-binaries-on-osx/

echo -e "\n---------- Finalize files in bundle"

mv -v $TEMPROOT/bin                           $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/share                         $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/etc                           $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/lib                           $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/libexec                       $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/opt                           $TEMPROOT/digikam.app/Contents/
if [ ! -d $TEMPROOT/Cellar ] ; then
    mkdir $TEMPROOT/Cellar
fi
mv -v $TEMPROOT/Cellar                        $TEMPROOT/digikam.app/Contents/

ln -sv "../../digikam.app/Contents/bin"       "$TEMPROOT/showfoto.app/Contents/bin"
ln -sv "../../digikam.app/Contents/etc"       "$TEMPROOT/showfoto.app/Contents/etc"
ln -sv "../../digikam.app/Contents/lib"       "$TEMPROOT/showfoto.app/Contents/lib"
ln -sv "../../digikam.app/Contents/libexec"   "$TEMPROOT/showfoto.app/Contents/libexec"
ln -sv "../../digikam.app/Contents/share"     "$TEMPROOT/showfoto.app/Contents/share"
ln -sv "../../digikam.app/Contents/Resources" "$TEMPROOT/showfoto.app/Contents/Resources"
ln -sv "../../digikam.app/Contents/opt"       "$TEMPROOT/showfoto.app/Contents/opt"
ln -sv "../../digikam.app/Contents/Cellar"    "$TEMPROOT/showfoto.app/Contents/Cellar"

ln -sv "../lib" $TEMPROOT/digikam.app/Contents/opt/lib

echo -e "\n---------- Relocatable binary files"

# Relocatable dynamic libraries with rpath

echo -e "\n--- Relocatable dynamic library files"

DYLIBFILES=(`find $TEMPROOT -name "*.dylib"`)

RelocatableBinaries DYLIBFILES[@]

# Relocatable system objects with rpath

echo -e "\n--- Relocatable system object files"

SOFILES=(`find $TEMPROOT -name "*.so"`)

RelocatableBinaries SOFILES[@]

# Relocatable executables with rpath and patch relative search path.

echo -e "\n--- Relocatable executable files"

EXECFILES=(`find $TEMPROOT -type f ! -name "*.dylib" ! -name "*.so" -print0 | xargs -0 -n 10 file | grep "Mach-O" | cut -d ':' -f 1`)

RelocatableBinaries EXECFILES[@]

echo -e "\n--- Patch RPATH in executable files"

for APP in ${EXECFILES[@]} ; do

    ISBINARY=`file "$APP" | grep "Mach-O" || true`

    # Do not touch debug extension
    ISDSYM=`file "$APP" | grep "dSYM" || true`

    # Do not patch applet files which are pure Apple API binaries
    BASENAME=`basename "$APP"`

    if [[ $ISBINARY ]] && [[ ! $ISDSYM ]] && [[ $BASENAME != "applet" ]] ; then

        install_name_tool -add_rpath @executable_path/.. $APP
        install_name_tool -add_rpath @executable_path/../.. $APP
#        install_name_tool -add_rpath @executable_path/../../.. $APP
#        install_name_tool -add_rpath @executable_path/../../../.. $APP
#        install_name_tool -add_rpath @executable_path/../../../../.. $APP
#        install_name_tool -add_rpath @executable_path/../../../../../.. $APP
#        install_name_tool -add_rpath @executable_path/../../../../../../.. $APP
#        install_name_tool -add_rpath @executable_path/../../../../../../../.. $APP
#        install_name_tool -add_rpath @executable_path/../../../../../../../../.. $APP
#        install_name_tool -add_rpath @executable_path/../../../../../../../../../.. $APP
        codesign --force -s - $APP

        echo "Patch $APP"

    fi

done

#################################################################################################
# Final stage to manage file places in bundle

# echo -e "\n---------- Finalize files in bundle"

# mv -v $TEMPROOT/bin                           $TEMPROOT/digikam.app/Contents/
# mv -v $TEMPROOT/share                         $TEMPROOT/digikam.app/Contents/
# mv -v $TEMPROOT/etc                           $TEMPROOT/digikam.app/Contents/
# mv -v $TEMPROOT/lib                           $TEMPROOT/digikam.app/Contents/
# mv -v $TEMPROOT/libexec                       $TEMPROOT/digikam.app/Contents/
# mv -v $TEMPROOT/opt                           $TEMPROOT/digikam.app/Contents/
# mv -v $TEMPROOT/Cellar                        $TEMPROOT/digikam.app/Contents/

# ln -sv "../../digikam.app/Contents/bin"       "$TEMPROOT/showfoto.app/Contents/bin"
# ln -sv "../../digikam.app/Contents/etc"       "$TEMPROOT/showfoto.app/Contents/etc"
# ln -sv "../../digikam.app/Contents/lib"       "$TEMPROOT/showfoto.app/Contents/lib"
# ln -sv "../../digikam.app/Contents/libexec"   "$TEMPROOT/showfoto.app/Contents/libexec"
# ln -sv "../../digikam.app/Contents/share"     "$TEMPROOT/showfoto.app/Contents/share"
# ln -sv "../../digikam.app/Contents/Resources" "$TEMPROOT/showfoto.app/Contents/Resources"
# ln -sv "../../digikam.app/Contents/opt"       "$TEMPROOT/showfoto.app/Contents/opt"
# ln -sv "../../digikam.app/Contents/Cellar"    "$TEMPROOT/showfoto.app/Contents/Cellar"

# ln -sv "../lib" $TEMPROOT/digikam.app/Contents/opt/lib

# echo -e "\n---------- Cleanup files in bundle"

# Last cleanup

echo -e "\n---------- Cleanup files in bundle"

HEADERFILES=(`find $TEMPROOT -name "*.h" -o -name "*.hpp"`)

for HPP in ${HEADERFILES[@]} ; do

    rm -fv $HPP

done


#################################################################################################
# fix ImageMagick config
sed -i '' "s|$INSTALL_PREFIX|..|g" $TEMPROOT/$DK_APP_CONTENTS/lib/ImageMagick/config-Q16HDRI/configure.xml

#################################################################################################
# fix the Qt directories and copy the qt.conf file

# Copy qt.conf
cp "$ORIG_WD/data/qt.conf" "$TEMPROOT/$DK_APP_CONTENTS/Resources/qt.conf"
QT_FULL_VERSION=`ls "$TEMPROOT/$DK_APP_CONTENTS/Cellar/qt"`

# symlink Qt dirs
ln -s "./Resources/QtCurve" "$TEMPROOT/$DK_APP_CONTENTS/share/QtCurve"
ln -s "../Cellar/qt/$QT_FULL_VERSION/lib/QtWebEngineCore.framework/Versions/A/Helpers/QtWebEngineProcess.app/Contents/MacOS/QtWebEngineProcess" "$TEMPROOT/$DK_APP_CONTENTS/MacOS/QtWebEngineProcess"
ln -s "./libexec/qt6/plugins" "$TEMPROOT/$DK_APP_CONTENTS/PlugIns"
ln -s "../../Cellar/qt/$QT_FULL_VERSION/lib/QtWebEngineCore.framework/Versions/A/Resources/qtwebengine_locales" "$TEMPROOT/$DK_APP_CONTENTS/Resources/translations/qtwebengine_locales"

# move the QtWebEngine resources
echo "     ----- Moving WebEngine Resources"
WEBENGINE_RESOURCES=`find "$TEMPROOT/$DK_APP_CONTENTS/Cellar/qt/$QT_FULL_VERSION/lib/QtWebEngineCore.framework/Versions/A/Resources" -name "*.*" -maxdepth 1 ! -name '*.plist'`
for WER in $WEBENGINE_RESOURCES ; do
    echo "Moving WER: $WER"
    mv "$WER" "$TEMPROOT/$DK_APP_CONTENTS/Resources"
done

#################################################################################################
# configure MySQL/MariaDB 

# clean up old cache files
if [ -f "$ORIG_WD/found_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/found_dylibs_cache.pkl"
fi

if [ -f "$ORIG_WD/processed_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/processed_dylibs_cache.pkl"
fi
if [ -f "$ORIG_WD/signed_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/signed_dylibs_cache.pkl"
fi

rm -rfv $TEMPROOT/digikam.app/Contents/opt/mariadb/bin/mysql-test

mv $TEMPROOT/$DK_APP_CONTENTS/opt/mariadb $TEMPROOT/$DK_APP_CONTENTS/lib/
cp $INSTALL_PREFIX/Library/Application/digikam/database/mysql-global.conf "$TEMPROOT/$DK_APP_CONTENTS/Resources/digikam/database/mysql-global.conf"

# find the binaries
MARIADB_BINARIES=`find $TEMPROOT/$DK_APP_CONTENTS/lib/mariadb -type f -perm +111`
for FILE in $MARIADB_BINARIES ; do
        # install_name_tool -add_rpath @executable_path/.. $FILE || true
        # install_name_tool -add_rpath @executable_path/../.. $FILE || true
        # install_name_tool -add_rpath @executable_path/../../.. $FILE || true
        # install_name_tool -add_rpath @executable_path/../../../.. $FILE || true
        # codesign --force -s - $APP

        copy_lib="$INSTALL_PREFIX/bin/python3 $ORIG_WD/package_lib.py --file=$FILE --bundle-root=$TEMPROOT/$DK_APP_CONTENTS/lib/mariadb --homebrew=$INSTALL_PREFIX --processed-cache=update  --found-cache=update --signed-cache=update --update-binary=1 --copy=1 --preserve_rpath=0"
        #copy_lib="$INSTALL_PREFIX/bin/python3 $ORIG_WD/package_lib.py --file=$FILE --bundle-root=$TEMPROOT/$DK_APP_CONTENTS --processed-cache=update  --found-cache=update --signed-cache=update --update-binary=true --copy=false"
        eval "$copy_lib"
done

echo -e "\n---------- Patch config and script files in bundle"

MARIADB_VERSION=`ls "$INSTALL_PREFIX/Cellar/mariadb"`

echo "MariaDB Version is: - $MARIADB_VERSION -"

MARIADBFILES=`find $TEMPROOT/$DK_APP_CONTENTS/lib/mariadb -type f ! -name "*.dylib" ! -name "*.so"`

for FILE in $MARIADBFILES ; do

    echo "MariaDB processing: $FILE"
    # to handle only text files
    ISTEXT=`file "$FILE" | grep -e "ASCII text" -e "Perl script text" || true`

    if [[ $ISTEXT ]] ; then

        echo "MariaDB text file: $FILE"
        NEEDPATCH=`grep "$INSTALL_PREFIX" "$FILE" || true`

        if [[ $NEEDPATCH ]] ; then

            echo -e "--- Patching $FILE..."
            sed -i '' "s|$INSTALL_PREFIX/Cellar/mariadb/$MARIADB_VERSION/bin|$RELOCATE_PREFIX/digikam.app/Contents/lib/mariadb/bin|g" $FILE
            sed -i '' "s|$INSTALL_PREFIX/Cellar/mariadb/$MARIADB_VERSION/var|$RELOCATE_PREFIX/digikam.app/Contents/lib/mariadb/var|g" $FILE
            sed -i '' "s|$INSTALL_PREFIX/Cellar/mariadb/$MARIADB_VERSION/lib|$RELOCATE_PREFIX/digikam.app/Contents/lib/mariadb/lib|g" $FILE
            sed -i '' "s|$INSTALL_PREFIX/Cellar/mariadb/$MARIADB_VERSION/libexec|$RELOCATE_PREFIX/digikam.app/Contents/lib/mariadb/libexec|g" $FILE
            sed -i '' "s|$INSTALL_PREFIX/Cellar/mariadb/$MARIADB_VERSION/share|$RELOCATE_PREFIX/digikam.app/Contents/lib/mariadb/share|g" $FILE
            sed -i '' "s|$INSTALL_PREFIX/Cellar/mariadb/$MARIADB_VERSION/etc|$RELOCATE_PREFIX/digikam.app/Contents/lib/mariadb/etc|g" $FILE
            sed -i '' "s|$INSTALL_PREFIX|$RELOCATE_PREFIX/digikam.app/Contents/lib/mariadb|g" $FILE

        fi

    fi

done

# #################################################################################################
# # See bug #436624: move mariadb share files at basedir (this must be done after patch operations)



#################################################################################################
# Not sure why this library isn't being moved into the right place
# quick-fix, but should be looked at more
#sudo mv $TEMPROOT/digikam.app/Contents/lib/gcc/current/libgcc_s.1.1.dylib $TEMPROOT/digikam.app/Contents/Frameworks/

#################################################################################################
# Install ExifTool binary.

cd $DOWNLOAD_DIR

#if [ ! -f "$DOWNLOAD_DIR/Image-ExifTool.tar.gz" ] ; then
    wget https://files.kde.org/digikam/exiftool/Image-ExifTool.tar.gz -O Image-ExifTool.tar.gz
#fi

tar -xvf "$DOWNLOAD_DIR/Image-ExifTool.tar.gz" -C "$TEMPROOT/digikam.app/Contents/bin"
cd $TEMPROOT/digikam.app/Contents/bin
EXIFTOOL_DIR=$(ls -d Image-ExifTool*)
ln -sv "./$EXIFTOOL_DIR" "./Image-ExifTool"
ln -sv "./Image-ExifTool/exiftool" "exiftool"

#################################################################################################
# Build PKG file

echo "---------- Create MacOS package for digiKam $DKRELEASEID"

mkdir -p $ORIG_WD/bundle
rm -f $ORIG_WD/bundle/*arm64$DEBUG_SUF* || true

if [[ $DK_VERSION != v* ]] ; then

    # with non-official release version, use build time-stamp as sub-version string.
    DK_SUBVER="-`cat $ORIG_WD/data/BUILDDATE.txt`"

else

    # with official release version, disable upload to KDE server, as this break check for new version function.
    echo -e "Official release version detected, upload is disabled.\n"
    DK_UPLOAD=0

fi

if [[ $DK_QTVERSION == 5 ]] ; then

    QT_SUF="-Qt5"

else

    QT_SUF="-Qt6"

fi

TARGET_INSTALLER=digiKam-$DKRELEASEID$DK_SUBVER$QT_SUF-MacOS-$ARCH_TARGET$DEBUG_SUF.pkg
TARGET_PKG_FILE=$BUILDDIR/bundle/$TARGET_INSTALLER
echo -e "Target PKG file : $TARGET_PKG_FILE"

chmod 666 $PROJECTDIR/digikam.pkgproj

$PACKAGESBUILD -v "$PROJECTDIR/digikam.pkgproj" --package-version "$DKRELEASEID"

mv "$PROJECTDIR/build/digikam.pkg" "$TARGET_PKG_FILE"

#################################################################################################
# Show resume information and future instructions to host PKG file to remote server

echo -e "\n---------- Compute package checksums for digiKam $DKRELEASEID\n" >  $TARGET_PKG_FILE.sum
echo    "File       : $TARGET_PKG_FILE"                                     >> $TARGET_PKG_FILE.sum
echo -n "Size       : "                                                     >> $TARGET_PKG_FILE.sum
du -h "$TARGET_PKG_FILE"        | { read first rest ; echo $first ; }       >> $TARGET_PKG_FILE.sum
echo -n "SHA256 sum : "                                                     >> $TARGET_PKG_FILE.sum
shasum -a256 "$TARGET_PKG_FILE" | { read first rest ; echo $first ; }       >> $TARGET_PKG_FILE.sum

# Checksums to post on Phabricator at release time.
shasum -a256 "$TARGET_PKG_FILE" > $BUILDDIR/bundle/sha256_release.sum

if [[ $DK_SIGN = 1 ]] ; then

    cat ~/.gnupg/dkorg-gpg-pwd.txt | gpg --batch --yes --passphrase-fd 0 -sabv "$TARGET_PKG_FILE"
    mv -f $TARGET_PKG_FILE.asc $TARGET_PKG_FILE.sig

    echo    "File       : $TARGET_PKG_FILE.sig"                                     >> $TARGET_PKG_FILE.sum
    echo -n "Size       : "                                                         >> $TARGET_PKG_FILE.sum
    du -h "$TARGET_PKG_FILE.sig"        | { read first rest ; echo $first ; }       >> $TARGET_PKG_FILE.sum
    echo -n "SHA256 sum : "                                                         >> $TARGET_PKG_FILE.sum
    shasum -a256 "$TARGET_PKG_FILE.sig" | { read first rest ; echo $first ; }       >> $TARGET_PKG_FILE.sum

    # Checksums to post on Phabricator at release time.
    shasum -a256 "$TARGET_PKG_FILE.sig" >> $BUILDDIR/bundle/sha256_release.sum

fi

cat $TARGET_PKG_FILE.sum

if [[ $DK_UPLOAD = 1 ]] ; then

    echo -e "---------- Cleanup older bundle Package files from files.kde.org repository \n"

    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-MacOS-arm64$DEBUG_SUF.pkg*"

    echo -e "---------- Upload new bundle Package files to files.kde.org repository \n"

    rsync -r -v --progress -e ssh $BUILDDIR/bundle/$TARGET_INSTALLER     $DK_UPLOADURL:$DK_UPLOADDIR

    if [[ $DK_SIGN = 1 ]] ; then
        scp $BUILDDIR/bundle/$TARGET_INSTALLER.sig $DK_UPLOADURL:$DK_UPLOADDIR
    fi

    # update remote files list

    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "ls digi*" > $ORIG_WD/bundle/ls.txt
    tail -n +2 $ORIG_WD/bundle/ls.txt > $ORIG_WD/bundle/ls.tmp
    cat $ORIG_WD/bundle/ls.tmp | grep -E '(.pkg |.appimage |.exe )' | grep -Ev '(debug)' > $ORIG_WD/bundle/FILES
    rm $ORIG_WD/bundle/ls.tmp
    rm $ORIG_WD/bundle/ls.txt
    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm FILES"
    rsync -r -v --progress -e ssh $BUILDDIR/bundle/FILES $DK_UPLOADURL:$DK_UPLOADDIR

else
    echo -e "\n------------------------------------------------------------------"
    curl https://download.kde.org/README_UPLOAD
    echo -e "------------------------------------------------------------------\n"
fi

#################################################################################################
# clean up old cache files
if [ -f "$ORIG_WD/found_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/found_dylibs_cache.pkl"
fi

if [ -f "$ORIG_WD/processed_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/processed_dylibs_cache.pkl"
fi
if [ -f "$ORIG_WD/signed_dylibs_cache.pkl" ] ; then
    rm "$ORIG_WD/signed_dylibs_cache.pkl"
fi

#################################################################################################

TerminateScript
