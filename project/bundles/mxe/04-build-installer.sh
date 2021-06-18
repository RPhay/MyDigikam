#! /bin/bash

# Script to bundle data using previously-built KDE and digiKam installation
# and create a Windows installer file with NSIS application
# Dependency : NSIS makensis program for Linux.
#
# Copyright (c) 2015-2021 by Gilles Caulier  <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Halt and catch errors
set -eE
trap 'PREVIOUS_COMMAND=$THIS_COMMAND; THIS_COMMAND=$BASH_COMMAND' DEBUG
trap 'echo "FAILED COMMAND: $PREVIOUS_COMMAND"' ERR

export LANG=C

#################################################################################################
# Manage script traces to log file

mkdir -p ./logs
exec > >(tee ./logs/build-installer.full.log) 2>&1

#################################################################################################

echo "04-build-installer.sh : build digiKam Windows installer."
echo "--------------------------------------------------------"

#################################################################################################
# Pre-processing checks

. ./config.sh
. ./common.sh
StartScript
ChecksCPUCores
RegisterRemoteServers

#################################################################################################
# Check if NSIS CLI tools is installed

if ! which makensis ; then
    echo "NSIS CLI tool is not installed"
    echo "See http://nsis.sourceforge.net/ for details."
    exit 1
else
    echo "Check NSIS CLI tools passed..."
fi

#################################################################################################
# Configurations

# Directory where this script is located (default - current directory)
BUILDDIR="$PWD"

# Directory where bundle files are located
BUNDLEDIR="$BUILDDIR/temp"

ORIG_WD="`pwd`"

DK_RELEASEID=`cat $ORIG_WD/data/RELEASEID.txt`

#################################################################################################
# Build icons-set ressource

echo -e "\n---------- Build icons-set ressource\n"

cd $ORIG_WD/icon-rcc

rm -f CMakeCache.txt > /dev/null
rm -f *.rcc > /dev/null

cmake -DCMAKE_INSTALL_PREFIX="$MXE_INSTALL_PREFIX" \
      -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_COLOR_MAKEFILE=ON \
      -Wno-dev \
      .

make -j$CPU_CORES

#################################################################################################
# Copy files

echo -e "\n---------- Copy files in bundle directory\n"

# Directories creation -----------------------------------------------------------------------

cd $ORIG_WD

if [ -d "$BUNDLEDIR" ]; then
    rm -fr $BUNDLEDIR
    mkdir $BUNDLEDIR
fi

mkdir -p $BUNDLEDIR/data
mkdir -p $BUNDLEDIR/etc
mkdir -p $BUNDLEDIR/share
mkdir -p $BUNDLEDIR/translations

# Data files ---------------------------------------------------------------------------------

echo -e "\n---------- Marble data"
cp -r $MXE_INSTALL_PREFIX/data/*                                        $BUNDLEDIR/data         2>/dev/null

echo -e "\n---------- Generics data"
cp -r $MXE_INSTALL_PREFIX/share/lensfun                                 $BUNDLEDIR/data         2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/data/digikam                              $BUNDLEDIR/data         2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/data/showfoto                             $BUNDLEDIR/data         2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/data/solid                                $BUNDLEDIR/data         2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/data/k*                                   $BUNDLEDIR/data         2>/dev/null

# Copy digiKam hi-colors PNG icons-set to the bundle

cp -r $MXE_INSTALL_PREFIX/bin/data/icons                                $BUNDLEDIR/data         2>/dev/null
rm -fr $BUNDLEDIR/data/icons/breeze*                                                            2>/dev/null
rm -fr $BUNDLEDIR/data/icons/*.qrc                                                              2>/dev/null

echo -e "\n---------- Qt config"
cp    $BUILDDIR/data/qt.conf                                            $BUNDLEDIR/             2>/dev/null

echo -e "\n---------- icons-set"
cp    $BUILDDIR/icon-rcc/breeze.rcc                                     $BUNDLEDIR/             2>/dev/null
cp    $BUILDDIR/icon-rcc/breeze-dark.rcc                                $BUNDLEDIR/             2>/dev/null

echo -e "\n---------- i18n"
cp -r $MXE_INSTALL_PREFIX/qt5/translations/qt_*                         $BUNDLEDIR/translations 2>/dev/null
cp -r $MXE_INSTALL_PREFIX/qt5/translations/qtbase*                      $BUNDLEDIR/translations 2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/data/locale                               $BUNDLEDIR/data         2>/dev/null

echo -e "\n---------- Xdg"
cp -r $MXE_INSTALL_PREFIX/etc/xdg                                       $BUNDLEDIR/etc          2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/data/xdg                                  $BUNDLEDIR/share        2>/dev/null

# Plugins Shared libraries -------------------------------------------------------------------

echo -e "\n---------- Qt5 plugins"
cp -r $MXE_INSTALL_PREFIX/qt5/plugins                                   $BUNDLEDIR/             2>/dev/null

echo -e "\n---------- Marble plugins"
cp -r $MXE_INSTALL_PREFIX/plugins/*.dll                                 $BUNDLEDIR/plugins      2>/dev/null

echo -e "\n---------- digiKam and KF5 plugins"
cp -r $MXE_INSTALL_PREFIX/lib/plugins                                   $BUNDLEDIR/             2>/dev/null

echo -e "\n---------- OpenAL for QtAV"
cp -r $MXE_INSTALL_PREFIX/bin/OpenAL32.dll                              $BUNDLEDIR/             2>/dev/null

echo -e "\n---------- DrMinGw run-time"
cp -r $MXE_INSTALL_PREFIX/bin/exchndl.dll                               $BUNDLEDIR/             2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/mgwhelp.dll                               $BUNDLEDIR/             2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/dbghelp.dll                               $BUNDLEDIR/             2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/symsrv.dll                                $BUNDLEDIR/             2>/dev/null
cp -r $MXE_INSTALL_PREFIX/bin/symsrv.yes                                $BUNDLEDIR/             2>/dev/null

echo -e "\n---------- Copy executables with recursive dependencies in bundle directory\n"

# Executables and plugins shared libraries dependencies scan ---------------------------------

EXE_FILES="\
$MXE_INSTALL_PREFIX/bin/digikam.exe \
$MXE_INSTALL_PREFIX/bin/showfoto.exe \
$MXE_INSTALL_PREFIX/bin/kbuildsycoca5.exe \
$MXE_INSTALL_PREFIX/qt5/bin/QtWebNetworkProcess.exe \
$MXE_INSTALL_PREFIX/qt5/bin/QtWebProcess.exe \
$MXE_INSTALL_PREFIX/qt5/bin/QtWebStorageProcess.exe \
"
for app in $EXE_FILES ; do

    cp $app $BUNDLEDIR/
    $ORIG_WD/rll.py --copy --installprefix $MXE_INSTALL_PREFIX --odir $BUNDLEDIR --efile $app

done

DLL_FILES="\
`find  $MXE_INSTALL_PREFIX/lib/plugins -name "*.dll" -type f | sed 's|$MXE_INSTALL_PREFIX/libs/plugins||'` \
`find  $MXE_INSTALL_PREFIX/qt5/plugins -name "*.dll" -type f | sed 's|$MXE_INSTALL_PREFIX/qt5/plugins||'`  \
`find  $MXE_INSTALL_PREFIX/plugins     -name "*.dll" -type f | sed 's|$MXE_INSTALL_PREFIX/plugins/||'`     \
$MXE_INSTALL_PREFIX/bin/OpenAL32.dll \
$MXE_INSTALL_PREFIX/bin/exchndl.dll  \
$MXE_INSTALL_PREFIX/bin/mgwhelp.dll  \
"

for app in $DLL_FILES ; do

    $ORIG_WD/rll.py --copy --installprefix $MXE_INSTALL_PREFIX --odir $BUNDLEDIR --efile $app

done

#################################################################################################
# Cleanup symbols in binary files to free space.

echo -e "\n---------- Strip symbols in binary files\n"

if [[ $DK_DEBUG = 1 ]] ; then

    find $BUNDLEDIR -name \*exe | grep -Ev '(digikam|showfoto|exiv2)' | xargs ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip
    find $BUNDLEDIR -name \*dll | grep -Ev '(digikam|showfoto|exiv2)' | xargs ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip

else

    find $BUNDLEDIR -name \*exe | xargs ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip
    find $BUNDLEDIR -name \*dll | xargs ${MXE_BUILDROOT}/usr/bin/${MXE_BUILD_TARGETS}-strip

fi

#################################################################################################
# Install ExifTool binary.

EXIFTOOL_VERSION="12.27"
wget https://exiftool.org/exiftool-$EXIFTOOL_VERSION.zip -P $BUNDLEDIR
unzip $BUNDLEDIR/exiftool-$EXIFTOOL_VERSION.zip -o -d $BUNDLEDIR
mv "$BUNDLEDIR/exiftool(-k).exe" "$BUNDLEDIR/exiftool.exe"
rm -f $BUNDLEDIR/exiftool-$EXIFTOOL_VERSION.zip

#################################################################################################

if [[ $DK_DEBUG = 1 ]] ; then

    DEBUG_SUF="-debug"

fi

if [[ $DK_VERSION = "master" ]] ; then

    # with master branch, use build time-stamp as sub-version string.
    DK_SUBVER="-`cat $ORIG_WD/data/BUILDDATE.txt`"

fi

#################################################################################################
# Build NSIS installer and Portable archive.

echo -e "\n---------- Build NSIS installer and Portable archive\n"

mkdir -p $ORIG_WD/bundle

if [ $MXE_BUILD_TARGETS == "i686-w64-mingw32.shared" ]; then

    TARGET_INSTALLER=digiKam-$DK_RELEASEID$DK_SUBVER-Win32$DEBUG_SUF.exe
    PORTABLE_FILE=digiKam-$DK_RELEASEID$DK_SUBVER-Win32$DEBUG_SUF.tar.xz
    CHECKSUM_FILE=digiKam-$DK_RELEASEID$DK_SUBVER-Win32$DEBUG_SUF.sum
    rm -f $ORIG_WD/bundle/*Win32$DEBUG_SUF* || true

else

    TARGET_INSTALLER=digiKam-$DK_RELEASEID$DK_SUBVER-Win64$DEBUG_SUF.exe
    PORTABLE_FILE=digiKam-$DK_RELEASEID$DK_SUBVER-Win64$DEBUG_SUF.tar.xz
    CHECKSUM_FILE=digiKam-$DK_RELEASEID$DK_SUBVER-Win64$DEBUG_SUF.sum
    rm -f $ORIG_WD/bundle/*Win64$DEBUG_SUF* || true

fi

cd $ORIG_WD/installer

makensis -DVERSION=$DK_RELEASEID -DBUNDLEPATH=$BUNDLEDIR -DTARGETARCH=$MXE_ARCHBITS -DOUTPUT=$ORIG_WD/bundle/$TARGET_INSTALLER ./digikam.nsi

cd $ORIG_WD
tar cf - `basename $BUNDLEDIR` --transform s/temp/digiKam/ | xz -4e > $ORIG_WD/bundle/$PORTABLE_FILE

#################################################################################################
# Show resume information and future instructions to host target files on remote server

echo -e "\n---------- Compute installer checksums for digiKam $DK_RELEASEID\n"          >  $ORIG_WD/bundle/$TARGET_INSTALLER.sum
echo    "File       : $TARGET_INSTALLER"                                                >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
echo -n "Size       : "                                                                 >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
du -h "$ORIG_WD/bundle/$TARGET_INSTALLER"        | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
echo -n "SHA256 sum : "                                                                 >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
shasum -a256 "$ORIG_WD/bundle/$TARGET_INSTALLER" | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum

# Checksums to post on Phabricator at release time.
shasum -a256 "$ORIG_WD/bundle/$TARGET_INSTALLER" > $ORIG_WD/bundle/$CHECKSUM_FILE

echo -e "\n---------- Compute Portable archive checksums for digiKam $DK_RELEASEID\n"   >  $ORIG_WD/bundle/$PORTABLE_FILE.sum
echo    "File       : $PORTABLE_FILE"                                                   >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
echo -n "Size       : "                                                                 >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
du -h "$ORIG_WD/bundle/$PORTABLE_FILE"        | { read first rest ; echo $first ; }     >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
echo -n "SHA256 sum : "                                                                 >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
shasum -a256 "$ORIG_WD/bundle/$PORTABLE_FILE" | { read first rest ; echo $first ; }     >> $ORIG_WD/bundle/$PORTABLE_FILE.sum

# Checksums to post on Phabricator at release time.
shasum -a256 "$ORIG_WD/bundle/$PORTABLE_FILE" >> $ORIG_WD/bundle/$CHECKSUM_FILE

if [[ $DK_SIGN = 1 ]] ; then

    echo -e "\n---------- Compute Signature checksums for digiKam installer $DK_RELEASEID\n"    >  $ORIG_WD/bundle/$TARGET_INSTALLER.sum

    cat ~/.gnupg/dkorg-gpg-pwd.txt | gpg --batch --yes --passphrase-fd 0 -sabv "$ORIG_WD/bundle/$TARGET_INSTALLER"
    mv -f $ORIG_WD/bundle/$TARGET_INSTALLER.asc $ORIG_WD/bundle/$TARGET_INSTALLER.sig

    echo    "File       : $TARGET_INSTALLER.sig"                                                >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
    echo -n "Size       : "                                                                     >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
    du -h "$ORIG_WD/bundle/$TARGET_INSTALLER.sig"        | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
    echo -n "SHA256 sum : "                                                                     >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum
    shasum -a256 "$ORIG_WD/bundle/$TARGET_INSTALLER.sig" | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$TARGET_INSTALLER.sum

    # Checksums to post on Phabricator at release time.
    shasum -a256 "$ORIG_WD/bundle/$TARGET_INSTALLER.sig" >> $ORIG_WD/bundle/$CHECKSUM_FILE

    echo -e "\n---------- Compute Signature checksums for digiKam Portable $DK_RELEASEID\n"     >  $ORIG_WD/bundle/$PORTABLE_FILE.sum

    cat ~/.gnupg/dkorg-gpg-pwd.txt | gpg --batch --yes --passphrase-fd 0 -sabv "$ORIG_WD/bundle/$PORTABLE_FILE"
    mv -f $ORIG_WD/bundle/$PORTABLE_FILE.asc $ORIG_WD/bundle/$PORTABLE_FILE.sig

    echo    "File       : $PORTABLE_FILE.sig"                                                >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
    echo -n "Size       : "                                                                  >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
    du -h "$ORIG_WD/bundle/$PORTABLE_FILE.sig"        | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
    echo -n "SHA256 sum : "                                                                  >> $ORIG_WD/bundle/$PORTABLE_FILE.sum
    shasum -a256 "$ORIG_WD/bundle/$PORTABLE_FILE.sig" | { read first rest ; echo $first ; }  >> $ORIG_WD/bundle/$PORTABLE_FILE.sum

    # Checksums to post on Phabricator at release time.
    shasum -a256 "$ORIG_WD/bundle/$PORTABLE_FILE.sig" >> $ORIG_WD/bundle/$CHECKSUM_FILE

fi

cat $ORIG_WD/bundle/$TARGET_INSTALLER.sum
cat $ORIG_WD/bundle/$PORTABLE_FILE.sum

if [[ $DK_UPLOAD = 1 ]] ; then

    echo -e "---------- Cleanup older Windows bundle files from files.kde.org repository \n"

    if [ $MXE_BUILD_TARGETS == "i686-w64-mingw32.shared" ]; then
        sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win32$DEBUG_SUF.exe*"
        sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win32$DEBUG_SUF.tar.xz*"
    else
        sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win64$DEBUG_SUF.exe*"
        sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm *-Win64$DEBUG_SUF.tar.xz*"
    fi

    echo -e "---------- Upload new Windows bundle files to files.kde.org repository \n"

    rsync -r -v --progress -e ssh $ORIG_WD/bundle/$TARGET_INSTALLER $DK_UPLOADURL:$DK_UPLOADDIR
    rsync -r -v --progress -e ssh $ORIG_WD/bundle/$PORTABLE_FILE $DK_UPLOADURL:$DK_UPLOADDIR

    if [[ $DK_SIGN = 1 ]] ; then
        scp $ORIG_WD/bundle/$TARGET_INSTALLER.sig $DK_UPLOADURL:$DK_UPLOADDIR
        scp $ORIG_WD/bundle/$PORTABLE_FILE.sig $DK_UPLOADURL:$DK_UPLOADDIR
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
