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

#ifndef DIGIKAM_PARALLEL_PIPES_H
#define DIGIKAM_PARALLEL_PIPES_H

// Local includes

#include "facepipeline_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN ParallelPipes : public QObject
{
    Q_OBJECT

public:

    explicit ParallelPipes();
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
    int                m_currentIndex;

private:

    // Disable
    ParallelPipes(QObject*)                        = delete;
    ParallelPipes(const ParallelPipes&)            = delete;
    ParallelPipes& operator=(const ParallelPipes&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_PARALLEL_PIPES_H
