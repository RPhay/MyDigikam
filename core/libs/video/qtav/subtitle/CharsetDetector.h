/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_CHAR_SET_DETECTOR_H
#define QTAV_CHAR_SET_DETECTOR_H

// Qt includes

#include <QByteArray>

namespace QtAV
{

class CharsetDetector
{
public:

    CharsetDetector();
    ~CharsetDetector();

    bool isAvailable() const;

    /*!
     * \brief detect
     * \param data text to parse
     * \return charset name
     */
    QByteArray detect(const QByteArray& data);

private:

    class Private;
    Private* d;

private:

    // Disable
    CharsetDetector(const CharsetDetector&)            = delete;
    CharsetDetector& operator=(const CharsetDetector&) = delete;
};

} // namespace QtAV

#endif // QTAV_CHAR_SET_DETECTOR_H
