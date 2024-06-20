/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "facepipeline_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN ParallelPipes : public QObject
{
    Q_OBJECT

public:

    ParallelPipes() = default;
    ~ParallelPipes() override;

    void schedule();
    void deactivate(WorkerObject::DeactivatingMode mode = WorkerObject::FlushSignals);
    void wait();

    void add(WorkerObject* const worker);
    void setPriority(QThread::Priority priority);

public:

    QList<WorkerObject*> m_workers;

public Q_SLOTS:

    void process(const FacePipelineExtendedPackage::Ptr& package);

Q_SIGNALS:

    void processed(const FacePipelineExtendedPackage::Ptr& package);

protected:

    QList<QMetaMethod> m_methods;
    int                m_currentIndex = 0;

private:

    // Disable
    ParallelPipes(QObject*)                        = delete;
    ParallelPipes(const ParallelPipes&)            = delete;
    ParallelPipes& operator=(const ParallelPipes&) = delete;
};

} // namespace Digikam
