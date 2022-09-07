/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 02-02-2012
 * Description : Face database interface to train identities.
 *
 * Copyright (C) 2012-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 * Copyright (C)      2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACE_DB_P_H
#define DIGIKAM_FACE_DB_P_H

#include "facedb.h"

// C++ includes

#include <iterator>

// Local includes

#include "digikam_debug.h"
#include "kd_tree.h"

namespace Digikam
{

class Q_DECL_HIDDEN FaceDb::Private
{

public:
    explicit Private()
        : db(nullptr)
    {
    }

    FaceDbBackend* db;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_DB_P_H
