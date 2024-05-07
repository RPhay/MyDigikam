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

#ifndef DIGIKAM_CREATE_MK_TASK_H
#define DIGIKAM_CREATE_MK_TASK_H

// Local includes

#include "commandtask.h"

namespace DigikamGenericPanoramaPlugin
{

class CreateMKTask : public CommandTask
{
public:

    explicit CreateMKTask(const QString& workDirPath,
                          const QUrl& input,
                          QUrl& mkUrl,
                          QUrl& panoUrl,
                          PanoramaFileType fileType,
                          const QString& pto2mkPath,
                          bool preview);
    ~CreateMKTask() override = default;

protected:

    void run(ThreadWeaver::JobPointer self, ThreadWeaver::Thread* thread) override;

private:

    const QUrl&            ptoUrl;
    QUrl&                  mkUrl;
    QUrl&                  panoUrl;
    const PanoramaFileType fileType;

private:

    // Disable
    CreateMKTask() = delete;

    Q_DISABLE_COPY(CreateMKTask)
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_CREATE_MK_TASK_H
