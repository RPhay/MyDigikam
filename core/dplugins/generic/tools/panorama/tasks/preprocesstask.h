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

#ifndef DIGIKAM_PRE_PROCESS_TASK_H
#define DIGIKAM_PRE_PROCESS_TASK_H

// Qt includes

#include <QPointer>

// Local includes

#include "panotask.h"
#include "dmetadata.h"

using namespace Digikam;

namespace DigikamGenericPanoramaPlugin
{

class PreProcessTask : public PanoTask
{
public:

    explicit PreProcessTask(const QString& workDirPath,
                            int id,
                            PanoramaPreprocessedUrls& targetUrls,
                            const QUrl& sourceUrl);
    ~PreProcessTask()                       override;

    void requestAbort()                     override;

public:

    const int id;

protected:

    void run(ThreadWeaver::JobPointer self,
             ThreadWeaver::Thread* thread)  override;

private:

    bool computePreview(const QUrl& inUrl);
    bool convertRaw();

private:

    class Private;
    Private* const d = nullptr;

private:

    // Disable
    PreProcessTask() = delete;

    Q_DISABLE_COPY(PreProcessTask)
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_PRE_PROCESS_TASK_H
