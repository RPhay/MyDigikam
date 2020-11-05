/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-15
 * Description : IO Jobs for file systems jobs
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

#ifndef DIGIKAM_IO_JOB_H
#define DIGIKAM_IO_JOB_H

// Qt includes

#include <QUrl>

// Local includes

#include "actionthreadbase.h"
#include "digikam_export.h"
#include "iojobdata.h"

namespace Digikam
{

class ItemInfo;

class DIGIKAM_GUI_EXPORT IOJob : public ActionJob
{
    Q_OBJECT

protected:

    IOJob();

Q_SIGNALS:

    void signalError(const QString& errMsg);
    void signalOneProccessed(const QUrl& url);

private:

    // Disable
    explicit IOJob(QObject*);
};

// ---------------------------------------

class DIGIKAM_GUI_EXPORT CopyOrMoveJob : public IOJob
{
    Q_OBJECT

public:

    explicit CopyOrMoveJob(IOJobData* const data);

protected:

    void run() override;

private:

    // Disable
    CopyOrMoveJob(QObject*);

private:

    IOJobData* m_data;
};

// ---------------------------------------

class DIGIKAM_GUI_EXPORT DeleteJob : public IOJob
{
    Q_OBJECT

public:

    explicit DeleteJob(IOJobData* const data);

protected:

    void run() override;

private:

    // Disable
    DeleteJob(QObject*);

private:

    IOJobData* m_data;
};

// ---------------------------------------

class DIGIKAM_GUI_EXPORT RenameFileJob : public IOJob
{
    Q_OBJECT

public:

    explicit RenameFileJob(IOJobData* const data);

Q_SIGNALS:

    void signalRenameFailed(const QUrl& url);

protected:

    void run() override;

private:

    // Disable
    RenameFileJob(QObject*);

private:

    IOJobData* m_data;
};

// ----------------------------------------------

class DIGIKAM_GUI_EXPORT DTrashItemsListingJob : public IOJob
{
    Q_OBJECT

public:

    explicit DTrashItemsListingJob(const QString& collectionPath);

Q_SIGNALS:

    void trashItemInfo(const DTrashItemInfo& info);

protected:

    void run() override;

private:

    // Disable
    DTrashItemsListingJob(QObject*);

private:

    QString m_collectionPath;
};

// ----------------------------------------------

class DIGIKAM_GUI_EXPORT RestoreDTrashItemsJob : public IOJob
{
    Q_OBJECT

public:

    explicit RestoreDTrashItemsJob(IOJobData* const data);

protected:

    void run() override;

private:

    // Disable
    RestoreDTrashItemsJob(QObject*);

private:

    IOJobData* m_data;
};

// ----------------------------------------------

class DIGIKAM_GUI_EXPORT EmptyDTrashItemsJob : public IOJob
{
    Q_OBJECT

public:

    explicit EmptyDTrashItemsJob(IOJobData* const data);

protected:

    void run() override;

private:

    // Disable
    EmptyDTrashItemsJob(QObject*);

private:

    IOJobData* m_data;
};

} // namespace Digikam

#endif // DIGIKAM_IO_JOB_H
