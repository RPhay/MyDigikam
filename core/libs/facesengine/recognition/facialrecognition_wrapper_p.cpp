/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The private implementation of recognition wrapper
 *
 * SPDX-FileCopyrightText: 2010      by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019      by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText: 2020      by Nghia Duong <minhnghiaduong997 at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facialrecognition_wrapper_p.h"

// Qt includes

#include <QtConcurrent>

// Local includes

#include "digikam_debug.h"
#include "applicationsettings.h"

namespace Digikam
{
/*
template <class T>
T* getObjectOrCreate(T* &ptr)
{
    if (!ptr)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "create recognizer";
        ptr = new T();
    }

    return ptr;
}
*/

FacialRecognitionWrapper::Private::Private()
    : trainingLock(QReadWriteLock::RecursionMode::Recursive)
{
    DbEngineParameters params = CoreDbAccess::parameters().faceParameters();
    params.setFaceDatabasePath(CoreDbAccess::parameters().faceParameters().getFaceDatabaseNameOrDir());

    recognizeModel            = ApplicationSettings::instance()->getFaceRecognitionModel();

    FaceDbAccess::setParameters(params, recognizeModel);

    dbAvailable               = FaceDbAccess::checkReadyForUse(nullptr);

    if (dbAvailable)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Face database ready for use";

        // Load identity cache.
        // TODO: load cache dynamically.

        const auto ids = FaceDbAccess().db()->identities();

        for (const Identity& identity : ids)
        {
            identityCache[identity.id()] = identity;
        }
    }

    else
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Failed to initialize face database";
    }

    trainingLock.lockForWrite();

    recognizer       = new OpenCVDNNFaceRecognizer(OpenCVDNNFaceRecognizer::Tree, recognizeModel);

    trainingLock.unlock();

    removeThreadPool = new QThreadPool();

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    // Priority should be equal or greater than trainer or recognizer threads.

    removeThreadPool->setThreadPriority(QThread::NormalPriority);

#endif

    // We only need 1 thread for the training remover.

    removeThreadPool->setMaxThreadCount(1);

    // Run the remove queue listener thread.

    // removeThreadResult = QtConcurrent::run(removeThreadPool, &trainingRemoveConcurrent, this);
}

FacialRecognitionWrapper::Private::~Private()
{
    // Send the end signal to the queue.

    // removeQueue.push(removeQueue.endSignal());

    // Clean up dynamic objects.

    delete recognizer;
    delete removeThreadPool;
}

// bool FacialRecognitionWrapper::Private::trainingRemoveConcurrent(FacialRecognitionWrapper::Private* self)
// {
//     QString hash;

//     // while (true)
//     // {
//     //     hash = self->removeQueue.pop_front();

//     //     if (self->removeQueue.endSignal() != hash)
//     //     {
//     //         self->clear(hash);
//     //         hash.clear();
//     //     }
//     //     else
//     //     {
//     //         break;
//     //     }
//     // }

//     qCDebug(DIGIKAM_FACEDB_LOG) << "Remove queue thread terminated";

//     return true;
// }

} // namespace Digikam
