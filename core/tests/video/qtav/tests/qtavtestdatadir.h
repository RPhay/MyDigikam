/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-10-10
 * Description : provides convenient access to test data directories
 *               and files for QtAV unit-tests.
 *
 * SPDX-FileCopyrightText: 2022 Steve Robbins <steve at sumost dot ca>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_TEST_DATA_DIR_H
#define QTAV_TEST_DATA_DIR_H

// Local includes

#include "dtestdatadir.h"

/**
 * \brief Class that provides convenient access to audio/video test data directories and files.
 *
 * After instantiation, check that property \ref isValid returns true; \sa TestDataDir.
 *
 * IMPORTANT NOTE: Tests intended to run on the KDE CI test machines will need to ensure that
 * the test machines support the required codecs and media formats.  The test program
 * "qtav-codecslist_cli" is useful to check for this.  It is run on each build and writes to
 * standard output so that it can be examined using https://invent.kde.org/graphics/digikam/-/pipelines.
 */

class QtAVTestDataDir : public DTestDataDir
{
public:

    QtAVTestDataDir()
        : DTestDataDir(QString::fromUtf8("core/tests/video/qtav"))
    {
    }

    QDir videoDir() const
    {
        return dir(QString::fromUtf8("video"));
    }

    /**
     * Return the video files path.
     */
    QString videoPath(const QString& name)
    {
        return videoDir().filePath(name);
    }

    /**
     * Audio codec known to be supported on all three KDE CI machines as of 2022-10-10.
     */
    QString basemediav1_mp4()
    {
        return videoPath(QString::fromUtf8("basemediav1.mp4"));
    }

    /**
     * Video and audio codec known to be supported on all three KDE CI machines as of 2022-10-10.
     */
    QString mpeg2_mp4()
    {
        return videoPath(QString::fromUtf8("mpeg2.mp4"));
    }

    QString mpeg4_mp4()
    {
        return videoPath(QString::fromUtf8("mpeg4.mp4"));
    }

    QString riffMpeg_avi()
    {
        return videoPath(QString::fromUtf8("riffMpeg.avi"));
    }
};

#endif // QTAV_TEST_DATA_DIR_H
