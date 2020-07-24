/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The private implementation of recognition wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef FACESENGINE_INTERFACE_P_H
#define FACESENGINE_INTERFACE_P_H

#include "facesengine_interface.h"

// Qt includes

#include <QMutex>
#include <QMutexLocker>
#include <QUuid>
#include <QDir>
#include <QStandardPaths>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"

/*
NOTE: experimental and deprecated
#include "opencvfisherfacerecognizer.h"
#include "opencveigenfacerecognizer.h"
*/

#ifdef USE_DNN_RECOGNITION_BACKEND
#   include "facerecognizer.h"
using namespace RecognitionTest;
#else
#   include "opencvlbphfacerecognizer.h"
#endif

#include "recognitiontrainingprovider.h"
#include "coredbaccess.h"
#include "dbengineparameters.h"
#include "facedbaccess.h"
#include "facedboperationgroup.h"
#include "facedb.h"
#include "funnelreal.h"
#include "dataproviders.h"
#include "identity.h"

namespace Digikam
{

class Q_DECL_HIDDEN FacesEngineInterface::Private
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

    bool     identityContains(const Identity& identity,
                              const QString& attribute,
                              const QString& value)                   const;

    Identity findByAttribute(const QString& attribute,
                             const QString& value)                    const;

    Identity findByAttributes(const QString& attribute,
                              const QMap<QString, QString>& valueMap) const;

public:

    bool                 dbAvailable;
    mutable QMutex       mutex;
    QVariantMap          parameters;
    QHash<int, Identity> identityCache;

    FunnelReal* funnel;

#ifdef USE_DNN_RECOGNITION_BACKEND

    FaceRecognizer*             recognizer;

#else

    OpenCVLBPHFaceRecognizer*   recognizer;

/*
    NOTE: experimental and deprecated

    OpenCVFISHERFaceRecognizer* recognizer;
    OpenCVEIGENFaceRecognizer*  recognizer;
*/

#endif
};

} // namespace Digikam

#endif // FACESENGINE_INTERFACE_P_H
