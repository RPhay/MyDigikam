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

class CompileMKStepTask : public CommandTask
{
public:

    explicit CompileMKStepTask(const QString& workDirPath,
                               int id,
                               const QUrl& mkUrl,
                               const QString& nonaPath,
                               const QString& enblendPath,
                               const QString& makePath,
                               bool preview);
    ~CompileMKStepTask() = default;

public:

    const int     id;

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread* thread) override;

private:

    const QUrl&   mkUrl;
    const QString nonaPath;
    const QString enblendPath;

private:

    // Disable
    CompileMKStepTask() = delete;

    Q_DISABLE_COPY(CompileMKStepTask)
};

} // namespace DigikamGenericPanoramaPlugin
