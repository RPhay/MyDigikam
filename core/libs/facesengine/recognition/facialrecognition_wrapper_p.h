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

#ifndef FACIAL_RECOGNITION_WRAPPER_P_H
#define FACIAL_RECOGNITION_WRAPPER_P_H

#include "facialrecognition_wrapper.h"

// Qt includes

#include <QMutexLocker>
#include <QUuid>
#include <QDir>
#include <QStandardPaths>
#include <QRecursiveMutex>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "opencvdnnfacerecognizer.h"
#include "recognitiontrainingprovider.h"
#include "coredbaccess.h"
#include "dbengineparameters.h"
#include "facedbaccess.h"
#include "facedboperationgroup.h"
#include "facedb.h"
#include "identity.h"

namespace Digikam
{

class Q_DECL_HIDDEN FacialRecognitionWrapper::Private
{
public:

    Private();
    ~Private();

public:

    // --- Backend parameters (facesengine_interface_setup.cpp) --------------------------

    void applyParameters();

public:

    // --- Faces Training management (facesengine_interface_training.cpp) ----------------------------------

    void trainIdentityBatch(const QList<Identity>& identitiesToBeTrained,
                            TrainingDataProvider* const data,
                            const QString& trainingContext);

    void clear(const QList<int>& idsToClear, const QString& trainingContext);

    // --- Identity management (facesengine_interface_identity.cpp) -----------------------------------------

    static bool identityContains(const Identity& identity,
                                 const QString& attribute,
                                 const QString& value);

    Identity findByAttribute(const QString& attribute,
                             const QString& value)                         const;

    Identity findByAttributes(const QString& attribute,
                              const QMultiMap<QString, QString>& valueMap) const;

public:

    bool                        dbAvailable = false;
    int                         ref         = 1;
    mutable QRecursiveMutex     mutex;
    QVariantMap                 parameters;
    QHash<int, Identity>        identityCache;
    OpenCVDNNFaceRecognizer*    recognizer  = nullptr;
};

} // namespace Digikam

#endif // FACIAL_RECOGNITION_WRAPPER_P_H
