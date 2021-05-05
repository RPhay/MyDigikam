/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-15
 * Description : Manager for creating and starting IO jobs threads
 *
 * Copyright (C) 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2018 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_IO_JOBS_MANAGER_H
#define DIGIKAM_IO_JOBS_MANAGER_H

// Qt includes

#include <QObject>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "dtrashiteminfo.h"
#include "iojobsthread.h"

namespace Digikam
{

class IOJobData;

class DIGIKAM_GUI_EXPORT IOJobsManager : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief instance: returns the singleton of IO Jobs Manager
     * @return IOJobsManager global instance
     */
    static IOJobsManager* instance();

    /**
     * @brief startIOJobs: Starts a thread to copy, move, delete or rename items
     * @param data: IOJobData container with source and destination url
     * @return IOJobsThread pointer for signal/slot connection
     */
    IOJobsThread* startIOJobs(IOJobData* const data);

    /**
     * @brief Starts a thread for listing items inside trash for specific collection
     * @param collectionPath: the path for collection to list items for it's trash
     * @return IOJobsThread pointer for signal/slot connection
     */
    IOJobsThread* startDTrashItemsListingForCollection(const QString& collectionPath);

private:

    // Disable
    IOJobsManager();
    explicit IOJobsManager(QObject*);

    friend class IOJobsManagerCreator;
};

} // namespace Digikam

#endif // DIGIKAM_IO_JOBS_MANAGER_H
