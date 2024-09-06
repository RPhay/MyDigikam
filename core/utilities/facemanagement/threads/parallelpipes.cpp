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

#include "parallelpipes.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

ParallelPipes::~ParallelPipes()
{
    for (WorkerObject* const object : std::as_const(m_workers))
    {
        delete object;
    }
}

void ParallelPipes::schedule()
{
    for (WorkerObject* const object : std::as_const(m_workers))
    {
        object->schedule();
    }
}

void ParallelPipes::deactivate(WorkerObject::DeactivatingMode mode)
{
    for (WorkerObject* const object : std::as_const(m_workers))
    {
        object->deactivate(mode);
    }
}

void ParallelPipes::wait()
{
    for (WorkerObject* const object : std::as_const(m_workers))
    {
        object->wait();
    }
}

void ParallelPipes::setPriority(QThread::Priority priority)
{
    for (WorkerObject* const object : std::as_const(m_workers))
    {
        object->setPriority(priority);
    }
}

void ParallelPipes::add(WorkerObject* const worker)
{
    QByteArray normalizedSignature = QMetaObject::normalizedSignature("process(FacePipelineExtendedPackage::Ptr)");
    int methodIndex                = worker->metaObject()->indexOfMethod(normalizedSignature.constData());

    if (methodIndex == -1)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Object" << worker << "does not have a slot"
                                     << normalizedSignature << " - cannot use for processing.";
        return;
    }

    m_workers << worker;
    m_methods << worker->metaObject()->method(methodIndex);

    // collect the worker's signals and bundle them to our single signal, which is further connected

    connect(worker, SIGNAL(processed(FacePipelineExtendedPackage::Ptr)),
            this, SIGNAL(processed(FacePipelineExtendedPackage::Ptr)));
}

void ParallelPipes::process(const FacePipelineExtendedPackage::Ptr& package)
{
    // Here, we send the package to one of the workers, in turn

    m_methods.at(m_currentIndex).invoke(m_workers.at(m_currentIndex), Qt::QueuedConnection,
                                        Q_ARG(FacePipelineExtendedPackage::Ptr, package));

    if (++m_currentIndex == m_workers.size())
    {
        m_currentIndex = 0;
    }
}

} // namespace Digikam

#include "moc_parallelpipes.cpp"
