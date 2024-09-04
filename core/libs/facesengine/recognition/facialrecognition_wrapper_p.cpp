/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The private implementation of recognition wrapper
 *
 * SPDX-FileCopyrightText:      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText:      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText:      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * SPDX-FileCopyrightText:      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facialrecognition_wrapper_p.h"

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
{
    DbEngineParameters params = CoreDbAccess::parameters().faceParameters();
    params.setFaceDatabasePath(CoreDbAccess::parameters().faceParameters().getFaceDatabaseNameOrDir());
    FaceDbAccess::setParameters(params);

    dbAvailable               = FaceDbAccess::checkReadyForUse(nullptr);

    if (dbAvailable)
    {
        qCDebug(DIGIKAM_FACESENGINE_LOG) << "Face database ready for use";

        // Load identity cache
        // TODO: load cache dynamically

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

    recognizer = new OpenCVDNNFaceRecognizer(OpenCVDNNFaceRecognizer::Tree);
}

FacialRecognitionWrapper::Private::~Private()
{
    delete recognizer;
}

} // namespace Digikam
