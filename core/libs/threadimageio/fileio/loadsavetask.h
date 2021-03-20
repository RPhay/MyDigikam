/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-20
 * Description : image file IO threaded interface.
 *
 * Copyright (C) 2005-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_LOAD_SAVE_TASK_H
#define DIGIKAM_LOAD_SAVE_TASK_H

// Qt includes

#include <QList>
#include <QEvent>

// Local includes

#include "dimg.h"
#include "dimgloaderobserver.h"
#include "loadingdescription.h"
#include "loadingcache.h"

namespace Digikam
{

class LoadSaveThread;

class LoadSaveTask
{
public:

    enum TaskType
    {
        TaskTypeLoading,
        TaskTypeSaving
    };

public:

    explicit LoadSaveTask(LoadSaveThread* const thread);
    virtual ~LoadSaveTask();

public:

    virtual void execute()                    = 0;
    virtual TaskType type()                   = 0;

    virtual void progressInfo(float progress) = 0;
    virtual bool continueQuery()              = 0;

protected:

    LoadSaveThread* m_thread;

private:

    // Disable
    LoadSaveTask(const LoadSaveTask&)            = delete;
    LoadSaveTask& operator=(const LoadSaveTask&) = delete;
};

//---------------------------------------------------------------------------------------------------

class LoadingTask : public LoadSaveTask,
                    public DImgLoaderObserver
{
public:

    enum LoadingTaskStatus
    {
        LoadingTaskStatusLoading,
        LoadingTaskStatusPreloading,
        LoadingTaskStatusStopping
    };

public:

    explicit LoadingTask(LoadSaveThread* const thread,
                         const LoadingDescription& description,
                         LoadingTaskStatus loadingTaskStatus = LoadingTaskStatusLoading);
    ~LoadingTask() override;

    LoadingTaskStatus status()                     const;
    QString filePath()                             const;

    const LoadingDescription& loadingDescription() const;

    // LoadSaveTask

    void execute()                    override;
    TaskType type()                   override;

    // DImgLoaderObserver

    void progressInfo(float progress) override;
    bool continueQuery()              override;

    void setStatus(LoadingTaskStatus status);

protected:

    LoadingDescription         m_loadingDescription;
    volatile LoadingTaskStatus m_loadingTaskStatus;

private:

    // Disable
    LoadingTask(const LoadingTask&)            = delete;
    LoadingTask& operator=(const LoadingTask&) = delete;
};

//---------------------------------------------------------------------------------------------------

class SharedLoadingTask : public LoadingTask,
                          public LoadingProcess,
                          public LoadingProcessListener
{
public:

    explicit SharedLoadingTask(LoadSaveThread* const thread,
                               const LoadingDescription& description,
                               LoadSaveThread::AccessMode mode = LoadSaveThread::AccessModeReadWrite,
                               LoadingTaskStatus loadingTaskStatus = LoadingTaskStatusLoading);

    void execute()                                                      override;
    void progressInfo(float progress)                                   override;

    bool needsPostProcessing()              const;
    virtual void postProcess();

    // LoadingProcess

    bool completed()                        const                       override;
    QString cacheKey()                      const                       override;
    void addListener(LoadingProcessListener* const listener)            override;
    void removeListener(LoadingProcessListener* const listener)         override;
    void notifyNewLoadingProcess(LoadingProcess* const process,
                                 const LoadingDescription& description) override;

    // LoadingProcessListener

    bool querySendNotifyEvent()             const                       override;
    void setResult(const LoadingDescription& loadingDescription,
                   const DImg& img)                                     override;
    LoadSaveNotifier* loadSaveNotifier()    const                       override;
    LoadSaveThread::AccessMode accessMode() const                       override;

    DImg img()                              const;

protected:

    volatile bool                  m_completed;
    LoadSaveThread::AccessMode     m_accessMode;
    QList<LoadingProcessListener*> m_listeners;
    DImg                           m_img;

private:

    // Disable
    SharedLoadingTask(const SharedLoadingTask&)            = delete;
    SharedLoadingTask& operator=(const SharedLoadingTask&) = delete;
};

//---------------------------------------------------------------------------------------------------

class SavingTask : public LoadSaveTask,
                   public DImgLoaderObserver
{
public:

    enum SavingTaskStatus
    {
        SavingTaskStatusSaving,
        SavingTaskStatusStopping
    };

public:

    explicit SavingTask(LoadSaveThread* const thread,
                        const DImg& img,
                        const QString& filePath,
                        const QString& format);

    SavingTaskStatus status() const;
    QString filePath()        const;

public:

    void execute()                    override;
    TaskType type()                   override;

    void progressInfo(float progress) override;
    bool continueQuery()              override;

    void setStatus(SavingTaskStatus status);

private:

    QString                   m_filePath;
    QString                   m_format;
    DImg                      m_img;
    volatile SavingTaskStatus m_savingTaskStatus;

private:

    // Disable
    SavingTask(const SavingTask&)            = delete;
    SavingTask& operator=(const SavingTask&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_LOAD_SAVE_TASK_H
