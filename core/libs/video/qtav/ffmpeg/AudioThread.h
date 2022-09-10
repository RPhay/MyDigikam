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

#ifndef QAV_AUDIO_THREAD_H
#define QAV_AUDIO_THREAD_H

// Local includes

#include "AVThread.h"

namespace QtAV
{

class AudioDecoder;
class AudioFrame;
class AudioThreadPrivate;

class AudioThread : public AVThread
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(AudioThread)

public:

    explicit AudioThread(QObject* const parent = nullptr);

protected:

    void applyFilters(AudioFrame& frame);
    virtual void run();
};

} // namespace QtAV

#endif // QAV_AUDIO_THREAD_H
