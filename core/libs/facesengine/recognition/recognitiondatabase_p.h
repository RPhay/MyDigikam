/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : The recognition database wrapper
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include "opencvfisherfacerecognizer.h"
#include "opencveigenfacerecognizer.h"
#include "opencvlbphfacerecognizer.h"
#include "opencvdnnfacerecognizer.h"
#include "recognitiontrainingprovider.h"
#include "coredbaccess.h"
#include "dbengineparameters.h"
#include "facedbaccess.h"
#include "facedboperationgroup.h"
#include "facedb.h"
#include "funnelreal.h"
#include "dataproviders.h"

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

    OpenCVLBPHFaceRecognizer*   lbph()               { return getObjectOrCreate(opencvlbph);          }
    OpenCVLBPHFaceRecognizer*   lbphConst() const    { return opencvlbph;                             }

    OpenCVEIGENFaceRecognizer*  eigen()              { return getObjectOrCreate(opencveigen);         }
    OpenCVEIGENFaceRecognizer*  eigenConst() const   { return opencveigen;                            }

    OpenCVFISHERFaceRecognizer* fisher()             { return getObjectOrCreate(opencvfisher);        }
    OpenCVFISHERFaceRecognizer* fisherConst() const  { return opencvfisher;                           }

    OpenCVDNNFaceRecognizer*    dnn()                { return getObjectOrCreate(opencvdnn);           }
    OpenCVDNNFaceRecognizer*    dnnConst() const     { return opencvdnn;                              }
    void                        createDNNDebug()     { opencvdnn = new OpenCVDNNFaceRecognizer(true); }

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

    void train(OpenCVLBPHFaceRecognizer* const r,
               const QList<Identity>& identitiesToBeTrained,
               TrainingDataProvider* const data,
               const QString& trainingContext);

    void train(OpenCVEIGENFaceRecognizer* const r,
               const QList<Identity>& identitiesToBeTrained,
               TrainingDataProvider* const data,
               const QString& trainingContext);

    void train(OpenCVFISHERFaceRecognizer* const r,
               const QList<Identity>& identitiesToBeTrained,
               TrainingDataProvider* const data,
               const QString& trainingContext);

    void train(OpenCVDNNFaceRecognizer* const r,
               const QList<Identity>& identitiesToBeTrained,
               TrainingDataProvider* const data,
               const QString& trainingContext);

public:

    // --- Recognition management (recognitiondatabase_recognize.cpp) ------------------------------------

    void clear(OpenCVLBPHFaceRecognizer* const,
               const QList<int>& idsToClear,
               const QString& trainingContext);

    void clear(OpenCVEIGENFaceRecognizer* const,
               const QList<int>& idsToClear,
               const QString& trainingContext);

    void clear(OpenCVFISHERFaceRecognizer* const,
               const QList<int>& idsToClear,
               const QString& trainingContext);

    void clear(OpenCVDNNFaceRecognizer* const,
               const QList<int>& idsToClear,
               const QString& trainingContext);

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

    OpenCVFISHERFaceRecognizer* opencvfisher;
    OpenCVEIGENFaceRecognizer*  opencveigen;
    OpenCVLBPHFaceRecognizer*   opencvlbph;
    OpenCVDNNFaceRecognizer*    opencvdnn;

    FunnelReal*                 funnel;
};

// --- Static Training methods (recognitiondatabase_training.cpp) ----------------------------------------

void trainIdentityBatchLBPH(OpenCVLBPHFaceRecognizer* const r,
                            const QList<Identity>& identitiesToBeTrained,
                            TrainingDataProvider* const data,
                            const QString& trainingContext,
                            RecognitionDatabase::Private* const d);

void trainIdentityBatchEIGEN(OpenCVEIGENFaceRecognizer* const r,
                             const QList<Identity>& identitiesToBeTrained,
                             TrainingDataProvider* const data,
                             const QString& trainingContext,
                             RecognitionDatabase::Private* const d);

void trainIdentityBatchFISHER(OpenCVFISHERFaceRecognizer* const r,
                              const QList<Identity>& identitiesToBeTrained,
                              TrainingDataProvider* const data,
                              const QString& trainingContext,
                              RecognitionDatabase::Private* const d);

void trainIdentityBatchDNN(OpenCVDNNFaceRecognizer* const r,
                           const QList<Identity>& identitiesToBeTrained,
                           TrainingDataProvider* const data,
                           const QString& trainingContext,
                           RecognitionDatabase::Private* const d);


} // namespace Digikam

#endif // DIGIKAM_FACESENGINE_RECOGNITION_DATABASE_P_H
