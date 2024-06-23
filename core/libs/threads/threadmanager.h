/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-13
 * Description : Thread object scheduling
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>
#include <QRunnable>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DynamicThread;
class WorkerObject;

class DIGIKAM_EXPORT ThreadManager : public QObject
{
    Q_OBJECT

public:

    static ThreadManager* instance();

    void initialize(WorkerObject* const object);
    void initialize(DynamicThread* const dynamicThread);

public Q_SLOTS:

    void schedule(WorkerObject* object);
    void schedule(QRunnable* runnable);

protected Q_SLOTS:

    void slotDestroyed(QObject* object);

protected:

    explicit ThreadManager();
    ~ThreadManager() override;

private:

    // Disable
    explicit ThreadManager(QObject*)               = delete;
    ThreadManager(const ThreadManager&)            = delete;
    ThreadManager& operator=(const ThreadManager&) = delete;

private:

    friend class ThreadManagerCreator;

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
