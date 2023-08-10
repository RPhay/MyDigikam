/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Thread actions task for image quality sorter.
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsassignmentask.h"

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "previewloadthread.h"
#include "iteminfo.h"
#include "maintenancedata.h"
#include "autotagsassign.h"

namespace Digikam
{

class Q_DECL_HIDDEN AutotagsAssignmentTask::Private
{
public:

    explicit Private()
        : data    (nullptr)
    {
    }

    MaintenanceData*      data;
    autoTagsAssign*       autotagsEngine;
};

// -------------------------------------------------------

AutotagsAssignmentTask::AutotagsAssignmentTask()
    : ActionJob(),
      d        (new Private)
{
}

AutotagsAssignmentTask::~AutotagsAssignmentTask()
{
    cancel();

    delete d;
}

void AutotagsAssignmentTask::setMaintenanceData(MaintenanceData* const data)
{
    d->data = data;
}

void AutotagsAssignmentTask::run()
{
    // While we have data (using this as check for non-null)
    
    while (d->data)
    {
        if (m_cancel)
        {
            return;
        }

        QString path = d->data->getImagePath();

        if (path.isEmpty())
        {
            break;
        }

        if (!m_cancel)
        {
            // Run Autotags backend here
            ItemInfo info = ItemInfo::fromLocalFile(path);
            qDebug() << info.filePath();

            d->autotagsEngine = new autoTagsAssign();

            // show tags to debug
            QList<QString> tagsLists = d->autotagsEngine->generateTagsList(path);
            qDebug() << "Tags :" << tagsLists << "\n";    

            // TODO Assign Tags in database using API from itemInfo

            d->autotagsEngine = nullptr;
            delete d->autotagsEngine;
        }


        Q_EMIT signalFinished();
    }

    Q_EMIT signalDone();
}

} // namespace Digikam
