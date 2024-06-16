/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-03-15
 * Description : a tool to create panorama by fusion of several images.
 *
 * SPDX-FileCopyrightText: 2012-2015 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "commandtask.h"

namespace DigikamGenericPanoramaPlugin
{

class CompileMKTask : public CommandTask
{
public:

    explicit CompileMKTask(const QString& workDirPath,
                           const QUrl& mkUrl,
                           const QUrl& panoUrl,
                           const QString& nonaPath,
                           const QString& enblendPath,
                           const QString& makePath,
                           bool preview);
    ~CompileMKTask() = default;

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread* thread) override;

private:

/*
    const QUrl* const panoUrl;
*/
    const QUrl&       mkUrl;
    const QString     nonaPath;
    const QString     enblendPath;

private:

    // Disable
    CompileMKTask() = delete;

    Q_DISABLE_COPY(CompileMKTask)
};

} // namespace DigikamGenericPanoramaPlugin
