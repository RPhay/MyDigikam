/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-03-27
 * Description : file copy actions using threads.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2006-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019      by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_FC_TASK_H
#define DIGIKAM_FC_TASK_H

// Qt includes

#include <QUrl>

// Local includes

#include "actionthreadbase.h"

using namespace Digikam;

namespace DigikamGenericFileCopyPlugin
{

using ChangeImagePropertiesPtr = std::shared_ptr<struct ChangeImageProperties>;

class FCTask : public ActionJob
{
    Q_OBJECT

public:

    enum FileCopyType
    {
        CopyFile = 0,
        FullSymLink,
        RelativeSymLink
    };

public:

    explicit FCTask(const QUrl& srcUrl,
                    const QUrl& dstUrl,
                    int behavior,
                    bool overwrite,
                    const ChangeImagePropertiesPtr& imageProp);
    ~FCTask();

Q_SIGNALS:

    void signalUrlProcessed(const QUrl& from, const QUrl& to);

protected:

    void run();

private:
    bool imageResize(const QString& orgUrl, const QString& destName, QString& err);

    class Private;
    Private* const d;
};

}  // namespace DigikamGenericFileCopyPlugin

#endif // DIGIKAM_FC_TASK
