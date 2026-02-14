#!/bin/bash

# ===========================================================
#
# This file is a part of digiKam project
# https://www.digikam.org
#
# Date:        2026-02-14
# Description: Script to generate a video of the git activity for one contributor.
#              Dependencies: gource and ffmpeg.
#              Arguments: the git user name (if null, whole team is displayed).
#              Exemple: ./gitactivity.sh Maik Qualmann
#
# Copyright (C) 2011-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
#
# SPDX-License-Identifier: BSD-3-Clause
#
# ============================================================

USER_NAME=$1

if [[ $USER_NAME == "" ]] ; then

    gource --highlight-colour FF0000 \
           --seconds-per-day 0.01 \
           --git-branch master \
           --hide filenames \
           --file-extensions \
           --highlight-users \
           --max-file-lag 0.5 \
           --auto-skip-seconds 1 \
           --title "digiKam Activity" \
           --output-framerate 25 \
           -o - | \
           ffmpeg -y -r 25 -f image2pipe \
                  -vcodec ppm -i - -vcodec libx264 \
                  -preset ultrafast -pix_fmt yuv420p \
                  -crf 20 -threads 0 -bf 0 \
                  "digiKam_activity.mp4"

else

    gource --user-show-filter $USER_NAME \
           --highlight-colour FF0000 \
           --seconds-per-day 0.01 \
           --git-branch master \
           --hide filenames \
           --file-extensions \
           --highlight-users \
           --max-file-lag 0.5 \
           --auto-skip-seconds 1 \
           --title "digiKam Activity for $USER_NAME" \
           --output-framerate 25 \
           -o - | \
           ffmpeg -y -r 25 -f image2pipe \
                  -vcodec ppm -i - -vcodec libx264 \
                  -preset ultrafast -pix_fmt yuv420p \
                  -crf 20 -threads 0 -bf 0 \
                  "digiKam_$USER_NAME_activity.mp4"

fi
