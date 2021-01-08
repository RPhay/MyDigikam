/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The private implementation of recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C)      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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
    : ref       (1),
      mutex     (QMutex::Recursive),
      recognizer(nullptr)
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

        foreach (const Identity& identity, FaceDbAccess().db()->identities())
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
