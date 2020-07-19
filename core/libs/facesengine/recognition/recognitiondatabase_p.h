/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The recognition database wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2019 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
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

#ifndef DIGIKAM_FACESENGINE_RECOGNITION_DATABASE_P_H
#define DIGIKAM_FACESENGINE_RECOGNITION_DATABASE_P_H

#include "recognitiondatabase.h"

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
//#   include "opencvdnnfacerecognizer.h"
#   include "facerecognizer.h"
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

using namespace RecognitionTest;

namespace Digikam
{

class Q_DECL_HIDDEN RecognitionDatabase::Private
{
public:

    bool                                    dbAvailable;
    mutable QMutex                          mutex;
    QVariantMap                             parameters;
    QHash<int, Identity>                    identityCache;
    RecognitionDatabase::RecognizeAlgorithm recognizeAlgorithm;

public:

    explicit Private();
    ~Private();

public:

    template <class T>
    T* getObjectOrCreate(T* &ptr) const
    {
        QMutexLocker lock(&mutex);

        if (!ptr)
        {
            qCDebug(DIGIKAM_FACESENGINE_LOG) << "create recognizer";
            ptr = new T();
        }

        return ptr;
    }

public:

/*
    NOTE: experimental and deprecated

    OpenCVEIGENFaceRecognizer*  eigen()              { return getObjectOrCreate(opencveigen);         }
    OpenCVEIGENFaceRecognizer*  eigenConst() const   { return opencveigen;                            }

    OpenCVFISHERFaceRecognizer* fisher()             { return getObjectOrCreate(opencvfisher);        }
    OpenCVFISHERFaceRecognizer* fisherConst() const  { return opencvfisher;                           }
*/

#ifdef USE_DNN_RECOGNITION_BACKEND

    FaceRecognizer*             dnn()                { return getObjectOrCreate(opencvdnn);           }
    //FaceRecognizer*             dnnConst() const     { return opencvdnn;                              }

#else

    OpenCVLBPHFaceRecognizer*   lbph()               { return getObjectOrCreate(opencvlbph);          }
    OpenCVLBPHFaceRecognizer*   lbphConst() const    { return opencvlbph;                             }

#endif

    FunnelReal*                 aligner()            { return getObjectOrCreate(funnel);              }
    FunnelReal*                 alignerConst() const { return funnel;                                 }

public:

    cv::Mat preprocessingChain(const QImage& image);
    cv::Mat preprocessingChainRGB(const QImage& image);

public:

    // --- Backend parameters (recognitiondatabase_backend.cpp) --------------------------

    void applyParameters();

public:

    // --- Faces Training management (recognitiondatabase_training.cpp) ----------------------------------

    void train(const QList<Identity>& identitiesToBeTrained,
               TrainingDataProvider* const data,
               const QString& trainingContext);

#ifdef USE_DNN_RECOGNITION_BACKEND
    void trainIdentityBatchDNN(const QList<Identity>& identitiesToBeTrained,
                               TrainingDataProvider* const data,
                               const QString& trainingContext);
#else
    void trainIdentityBatchLBPH(const QList<Identity>& identitiesToBeTrained,
                                TrainingDataProvider* const data,
                                const QString& trainingContext);

#endif
/*
    NOTE: experimental and deprecated

    void trainIdentityBatchEIGEN(const QList<Identity>& identitiesToBeTrained,
                                 TrainingDataProvider* const data,
                                 const QString& trainingContext);

    void trainIdentityBatchFISHER(const QList<Identity>& identitiesToBeTrained,
                                  TrainingDataProvider* const data,
                                  const QString& trainingContext);
*/

public:

    // --- Recognition management (recognitiondatabase_recognize.cpp) ------------------------------------

    void clear(const QList<int>& idsToClear,
               const QString& trainingContext);

#ifdef USE_DNN_RECOGNITION_BACKEND

    void clearDNN(const QList<int>& idsToClear,
                  const QString& trainingContext);

#else

    void clearLBPH(const QList<int>& idsToClear,
                   const QString& trainingContext);

#endif

/*
    NOTE: experimental and deprecated

    void clearEIGEN(const QList<int>& idsToClear,
                    const QString& trainingContext);

    void clearFISHER();
*/

public:

    // --- Identity management (recognitiondatabase_identity.cpp) -----------------------------------------

    bool     identityContains(const Identity& identity,
                              const QString& attribute,
                              const QString& value)                   const;

    Identity findByAttribute(const QString& attribute,
                             const QString& value)                    const;

    Identity findByAttributes(const QString& attribute,
                              const QMap<QString, QString>& valueMap) const;

private:

#ifdef USE_DNN_RECOGNITION_BACKEND

    FaceRecognizer*   opencvdnn;

#else

    OpenCVLBPHFaceRecognizer*   opencvlbph;

#endif

/*
    NOTE: experimental and deprecated

    OpenCVFISHERFaceRecognizer* opencvfisher;
    OpenCVEIGENFaceRecognizer*  opencveigen;
*/

    FunnelReal*                 funnel;
};

} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_RECOGNITION_DATABASE_P_H
