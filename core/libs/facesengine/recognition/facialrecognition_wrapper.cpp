/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Local include

#include "facedbaccess.h"
#include "facedb.h"

namespace Digikam
{
FacialRecognitionWrapper::Private* FacialRecognitionWrapper::d = nullptr;

FacialRecognitionWrapper::FacialRecognitionWrapper()
{
    if (!d)
    {
        d = new Private();
    }
    else
    {
        ++(d->ref);
    }
}

FacialRecognitionWrapper::FacialRecognitionWrapper(const FacialRecognitionWrapper& other)
{
    Q_UNUSED(other)
    ++(d->ref);
}

FacialRecognitionWrapper::~FacialRecognitionWrapper()
{
    --(d->ref);

    if (d->ref == 0)
    {
        delete d;
    }
}

bool FacialRecognitionWrapper::integrityCheck()
{
    if (!d || !d->dbAvailable)
    {
        return false;
    }

    QMutexLocker lock(&d->mutex);

    return FaceDbAccess().db()->integrityCheck();
}

void FacialRecognitionWrapper::vacuum()
{
    if (!d || !d->dbAvailable)
    {
        return;
    }

    QMutexLocker lock(&d->mutex);

    FaceDbAccess().db()->vacuum();
}

} // namespace Digikam
