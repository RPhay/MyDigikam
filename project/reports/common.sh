#!/bin/bash

# SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

########################################################################
# checks if branch has something pending
function parseGitDirty()
{
    git diff --quiet --ignore-submodules HEAD 2>/dev/null; [ $? -eq 1 ] && echo "M"
}

########################################################################
# gets the current git branch
function parseGitBranch()
{
    git branch --no-color 2> /dev/null | sed -e '/^[^*]/d' -e "s/* \(.*\)/\1/"
}

########################################################################
# get last commit hash prepended with @ (i.e. @8a323d0)
function parseGitHash()
{
    git rev-parse --short HEAD 2> /dev/null | sed "s/\(.*\)/-rev-\1/"
}

########################################################################
# Update online static analyze report.
# arg1: static analyzer name (clang, cppcheck, krazy, ...).
# arg2: static analyzer report directory with html contents.
# arg3: static analyzer report title.
# arg4: git branch name.
#
function updateOnlineReport()
{
    DK_UPLOADURL="digikam@tinami.kde.org"
    DK_UPLOADDIR="/srv/archives/files/digikam/reports/"

    echo -e "---------- Create archive of static analyzer report\n"

    XZ_OPT=-9 tar cJf $1_$4.tar.xv $2

    echo -e "---------- Cleanup older static analyzer report from remote web site\n"

    sftp -q $DK_UPLOADURL:$DK_UPLOADDIR <<< "rm $1_$4.tar.xv"

    echo -e "---------- Upload new static analyzer report\n"

    rsync -r -v --progress -e ssh $1_$4.tar.xv $DK_UPLOADURL:$DK_UPLOADDIR

    rm -fr $1_$4.tar.xv

    echo -e "---------- Report is available online at https://files.kde.org/digikam/reports/\n"
}

########################################################################
# Check CPU core available (Linux or MacOS)
function checksCPUCores()
{
    CPU_CORES=$(grep -c ^processor /proc/cpuinfo 2>/dev/null || sysctl -n hw.ncpu)

    if [[ $CPU_CORES -gt 1 ]]; then
        CPU_CORES=$((CPU_CORES-1))
    fi

    echo "CPU Cores to use : $CPU_CORES"
}
