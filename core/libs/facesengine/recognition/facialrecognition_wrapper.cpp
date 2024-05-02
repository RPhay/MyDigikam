/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The recognition wrapper
 *
 * SPDX-FileCopyrightText:      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText:      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText:      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
        d = new Private;
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
