#! /bin/bash

# Script to bundle data using previously-built digiKam installation.
# and create a PKG file with Packages application (http://s.sudre.free.fr/Software/Packages/about.html)
# This script must be run as sudo
#
# Copyright (c) 2015      by Shanti, <listaccount at revenant dot org>
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
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

# Pathes rules
ORIG_PATH="$PATH"
ORIG_WD="`pwd`"

export PATH=$INSTALL_PREFIX/bin:/$INSTALL_PREFIX/sbin:$ORIG_PATH

DKRELEASEID=`cat $ORIG_WD/data/RELEASEID.txt`

#################################################################################################
# Build icons-set ressource

echo -e "\n---------- Build icons-set ressource\n"

cd $ORIG_WD/icon-rcc

rm -f CMakeCache.txt > /dev/null
rm -f *.rcc > /dev/null

cp -f $ORIG_WD/../../../bootstrap.macports .

cmake -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX" \
      -DCMAKE_BUILD_TYPE=debug \
      -DCMAKE_COLOR_MAKEFILE=ON \
      -Wno-dev \
      .

make -j$CPU_CORES

export PATH=$ORIG_PATH

cd $ORIG_WD

#################################################################################################
# Configurations

# Directory where this script is located (default - current directory)
BUILDDIR="$PWD"

# Directory where Packages project files are located
PROJECTDIR="$BUILDDIR/installer"

# Staging area where files to be packaged will be copied
TEMPROOT="$BUILDDIR/$RELOCATE_PREFIX"

# Applications to be launched directly by user (create launch scripts)
KDE_MENU_APPS="\
digikam \
showfoto \
"

# Paths to search for applications above
KDE_APP_PATHS="\
Applications/digiKam.org \
"

# Other apps - non-MacOS binaries & libraries to be included with required dylibs
OTHER_APPS="\
lib/plugins/imageformats/*.so \
lib/plugins/digikam/bqm/*.so \
lib/plugins/digikam/generic/*.so \
lib/plugins/digikam/editor/*.so \
lib/plugins/digikam/dimg/*.so \
lib/plugins/digikam/rawimport/*.so \
lib/mariadb/bin/mysql \
lib/mariadb/bin/mysqld \
lib/mariadb/bin/my_print_defaults \
lib/mariadb/bin/mysqladmin \
lib/mariadb/bin/mysqltest \
lib/mariadb/mysql/*.dylib \
lib/mariadb/plugin/*.so \
lib/ImageMagick*/modules-Q16/coders/*.so \
lib/ImageMagick*/modules-Q16/filters/*.so \
bin/kbuildsycoca5 \
bin/solid-hardware5 \
libexec/qt5/plugins/imageformats/*.dylib \
libexec/qt5/plugins/sqldrivers/*.dylib \
libexec/qt5/plugins/printsupport/*.dylib \
libexec/qt5/plugins/platforms/*.dylib \
libexec/qt5/plugins/platformthemes/*.dylib \
libexec/qt5/plugins/iconengines/*.dylib \
libexec/qt5/plugins/generic/*.dylib \
libexec/qt5/plugins/styles/*.dylib \
libexec/qt5/plugins/bearer/*.dylib \
"

#lib/sane/*.so \

binaries="$OTHER_APPS"

# Additional Files/Directories - to be copied recursively but not checked for dependencies
# Note: dSYM directories aee copied as well and cleaned later if debug symbols must be removed in final bundle.
OTHER_DIRS="\
libexec/qt5/translations \
lib/libdigikam*.dSYM \
lib/plugins \
lib/libgphoto2 \
lib/libgphoto2_port \
lib/mariadb \
lib/ImageMagick* \
share/mariadb \
share/ImageMagick* \
etc/xdg \
etc/ImageMagick* \
etc/mariadb \
"

#etc/sane.d \

# Additional Data Directories - to be copied recursively
OTHER_DATA="\
share/applications \
share/opencv4 \
share/k* \
share/lensfun \
share/mime \
Library/Application/ \
Marble.app/Contents/Resources/ \
Marble.app/Contents/MacOS/resources/ \
"

# Packaging tool paths
PACKAGESBUILD="/usr/local/bin/packagesbuild"
RECURSIVE_LIBRARY_LISTER="$BUILDDIR/rll.py"

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

"$RECURSIVE_LIBRARY_LISTER" $binaries | sort -u | \
while read lib ; do
    lib="`echo $lib | sed "s:$INSTALL_PREFIX/::"`"

    if [ ! -e "$TEMPROOT/$lib" ] ; then
        dir="${lib%/*}"

        if [ ! -d "$TEMPROOT/$dir" ] ; then
            echo "  Creating $TEMPROOT/$dir"
            mkdir -p "$TEMPROOT/$dir"
        fi

        echo "  $lib"
        cp -aH "$INSTALL_PREFIX/$lib" "$TEMPROOT/$dir/"
    fi
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

    echo "  Copying $path"
    cp -a "$INSTALL_PREFIX/$path" "$TEMPROOT/$dir/"
done

echo "---------- Copying directory contents..."

for path in $OTHER_DIRS ; do
    dir="${path%/*}"

    if [ ! -d "$TEMPROOT/$dir" ] ; then
        echo "  Creating $TEMPROOT/$dir"
        mkdir -p "$TEMPROOT/$dir"
    fi

    echo "   Copying $path"
    cp -a "$INSTALL_PREFIX/$path" "$TEMPROOT/$dir/"
done


echo "---------- Copying data files..."

# Special case with data dirs. QStandardPaths::GenericDataLocation was patched everywhere
# in source code by QStandardPaths::AppDataLocation

for path in $OTHER_DATA ; do
    echo "   Copying $path"
    cp -a "$INSTALL_PREFIX/$path" "$TEMPROOT/digikam.app/Contents/Resources/"
done

# Copy digiKam hi-colors PNG icons-set to the bundle

mkdir -p "$TEMPROOT/digikam.app/Contents/Resources/icons/"
cp -a "$INSTALL_PREFIX/share/icons/hicolor" "$TEMPROOT/digikam.app/Contents/Resources/icons/"

echo "---------- Copying Qt Web Backend files..."

# Qt Web framework bin data files.
# NOTE: Since Qt 5.15.0, QtWebEngine runtime process is now located in
#       libexec/qt5/lib/QtWebEngineCore.framework/Versions/5/Helpers/QtWebEngineProcess.app/Contents/MacOS
#       instead of libexec/qt5/libexec/. No needs to make extra rules for this runtime process.

if [[ $DK_QTWEBENGINE = 0 ]] ; then

    # Rules for QtWebKit runtime process

    mkdir -p $TEMPROOT/libexec/qt5/libexec/

    # QtWebKit runtime process

    [[ -e $INSTALL_PREFIX/libexec/qt5/libexec/QtWebNetworkProcess ]] && cp -a "$INSTALL_PREFIX/libexec/qt5/libexec/QtWebNetworkProcess" "$TEMPROOT/libexec/qt5/libexec/"
    [[ -e $INSTALL_PREFIX/libexec/qt5/libexec/QtWebProcess ]]        && cp -a "$INSTALL_PREFIX/libexec/qt5/libexec/QtWebProcess"        "$TEMPROOT/libexec/qt5/libexec/"
    [[ -e $INSTALL_PREFIX/libexec/qt5/libexec/QtWebStorageProcess ]] && cp -a "$INSTALL_PREFIX/libexec/qt5/libexec/QtWebStorageProcess" "$TEMPROOT/libexec/qt5/libexec/"

fi

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
# Move digiKam and KF5 run-time plugins to the right place

cp -a $TEMPROOT/lib/plugins $TEMPROOT/libexec/qt5/
rm -rf $TEMPROOT/lib/plugins

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

cp $ORIG_WD/icon-rcc/breeze.rcc      $TEMPROOT/digikam.app/Contents/Resources/
cp $ORIG_WD/icon-rcc/breeze-dark.rcc $TEMPROOT/digikam.app/Contents/Resources/

#################################################################################################
# Cleanup symbols in binary files to free space.

echo -e "\n---------- Strip symbols in binary files\n"

if [[ $DK_DEBUG = 1 ]] ; then

    find $TEMPROOT -name "*.so"    | grep -Ev '(digikam|showfoto|exiv2)' | xargs strip -SXx
    find $TEMPROOT -name "*.dylib" | grep -Ev '(digikam|showfoto|exiv2)' | xargs strip -SXx

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
# http://thecourtsofchaos.com/2013/09/16/how-to-copy-and-relink-binaries-on-osx/

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

EXECFILES=(`find $TEMPROOT -type f -perm +ugo+x ! -name "*.dylib" ! -name "*.so"`)

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
        install_name_tool -add_rpath @executable_path/../../.. $APP
        install_name_tool -add_rpath @executable_path/../../../.. $APP
        install_name_tool -add_rpath @executable_path/../../../../.. $APP
        install_name_tool -add_rpath @executable_path/../../../../../.. $APP
        install_name_tool -add_rpath @executable_path/../../../../../../.. $APP
        install_name_tool -add_rpath @executable_path/../../../../../../../.. $APP
        install_name_tool -add_rpath @executable_path/../../../../../../../../.. $APP
        install_name_tool -add_rpath @executable_path/../../../../../../../../../.. $APP

        echo "Patch $APP"

    fi

done

#################################################################################################
# Final stage to manage file places in bundle

echo -e "\n---------- Finalize files in bundle"

mv -v $TEMPROOT/bin                           $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/share                         $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/etc                           $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/lib                           $TEMPROOT/digikam.app/Contents/
mv -v $TEMPROOT/libexec                       $TEMPROOT/digikam.app/Contents/

ln -sv "../../digikam.app/Contents/bin"       "$TEMPROOT/showfoto.app/Contents/bin"
ln -sv "../../digikam.app/Contents/etc"       "$TEMPROOT/showfoto.app/Contents/etc"
ln -sv "../../digikam.app/Contents/lib"       "$TEMPROOT/showfoto.app/Contents/lib"
ln -sv "../../digikam.app/Contents/libexec"   "$TEMPROOT/showfoto.app/Contents/libexec"
ln -sv "../../digikam.app/Contents/share"     "$TEMPROOT/showfoto.app/Contents/share"
ln -sv "../../digikam.app/Contents/Resources" "$TEMPROOT/showfoto.app/Contents/Resources"

echo -e "\n---------- Cleanup files in bundle"

# Last cleanup

HEADERFILES=(`find $TEMPROOT -name "*.h" -o -name "*.hpp"`)

for HPP in ${HEADERFILES[@]} ; do

    rm -fv $HPP

done

rm -rfv $TEMPROOT/digikam.app/Contents/share/mariadb/mysql-test
rm -rfv $TEMPROOT/digikam.app/Contents/share/mariadb/sql-bench

echo -e "\n---------- Patch config and script files in bundle"

MARIADBDIRS=(`find $TEMPROOT -type d -name "mariadb"`)

for DIR in ${MARIADBDIRS[@]} ; do

    MARIADBFILES=(`find $DIR -type f ! -name "*.dylib" ! -name "*.so"`)

    for FILE in ${MARIADBFILES[@]} ; do

        # to handle only text files
        ISTEXT=`file "$FILE" | grep "ASCII text" || true`

        if [[ $ISTEXT ]] ; then

            NEEDPATCH=`grep "$INSTALL_PREFIX" "$FILE" || true`

            if [[ $NEEDPATCH ]] ; then

                echo -e "--- Patching $FILE..."
                sed -i '' "s|$INSTALL_PREFIX/var|$RELOCATE_PREFIX/digikam.app/Contents/var|g" $FILE
                sed -i '' "s|$INSTALL_PREFIX/lib|$RELOCATE_PREFIX/digikam.app/Contents/lib|g" $FILE
                sed -i '' "s|$INSTALL_PREFIX/share|$RELOCATE_PREFIX/digikam.app/Contents/share|g" $FILE
                sed -i '' "s|$INSTALL_PREFIX/etc|$RELOCATE_PREFIX/digikam.app/Contents/etc|g" $FILE
                sed -i '' "s|$INSTALL_PREFIX|$RELOCATE_PREFIX/digikam.app/Contents/|g" $FILE

            fi

        fi

    done

done

#################################################################################################
# See bug #436624: move mariadb share files at basedir (this must be done after patch operations)

rsync -a "$TEMPROOT/digikam.app/Contents/share/mariadb" "$TEMPROOT/digikam.app/Contents/lib/mariadb/share/"
rm -fr "$TEMPROOT/digikam.app/Contents/share/mariadb"

#################################################################################################
# Build PKG file

echo "---------- Create MacOS package for digiKam $DKRELEASEID"

mkdir -p $ORIG_WD/bundle
rm -f $ORIG_WD/bundle/*x86-64$DEBUG_SUF* || true

if [[ $DK_VERSION = "master" ]] ; then

    # with master branch, use build time-stamp as sub-version string.
    DK_SUBVER="-`cat $ORIG_WD/data/BUILDDATE.txt`"

fi

TARGET_INSTALLER=digiKam-$DKRELEASEID$DK_SUBVER-MacOS-x86-64$DEBUG_SUF.pkg
TARGET_PKG_FILE=$BUILDDIR/bundle/$TARGET_INSTALLER
echo -e "Target PKG file : $TARGET_PKG_FILE"

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

    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-MacOS-x86-64$DEBUG_SUF.pkg*"

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

TerminateScript
