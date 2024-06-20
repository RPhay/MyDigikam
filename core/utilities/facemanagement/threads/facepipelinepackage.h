/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QFlags>
#include <QThread>

// Local includes

#include "identity.h"
#include "digikam_debug.h"
#include "facetagsiface.h"
#include "dimg.h"
#include "loadingdescription.h"
#include "iteminfo.h"
//#include "recognitiondatabase.h"

namespace Digikam
{

class DetectionBenchmarker;
class RecognitionBenchmarker;
class DetectionWorker;
class RecognitionWorker;
class TrainerWorker;
class DatabaseWriter;
class FacePreviewLoader;
class FaceItemRetriever;
class ParallelPipes;
class ScanStateFilter;

class FacePipelineFaceTagsIface : public FaceTagsIface
{
public:

    enum Role
    {
        NoRole             = 0,

        /// Source
        GivenAsArgument    = 1 << 0,
        ReadFromDatabase   = 1 << 1,
        DetectedFromImage  = 1 << 2,

        /// Task
        ForRecognition     = 1 << 10,
        ForConfirmation    = 1 << 11,
        ForTraining        = 1 << 12,
        ForEditing         = 1 << 13, ///< Add, Change or remove

        /// Executed action (task is cleared)
        Confirmed          = 1 << 20,
        Trained            = 1 << 21,
        Edited             = 1 << 22
    };
    Q_DECLARE_FLAGS(Roles, Role)

public:

    FacePipelineFaceTagsIface()  = default;
    explicit FacePipelineFaceTagsIface(const FaceTagsIface& face);
    ~FacePipelineFaceTagsIface() = default;

    FacePipelineFaceTagsIface& operator=(const FacePipelineFaceTagsIface& other);

public:

    Roles     roles             = NoRole;
    int       assignedTagId     = 0;
    TagRegion assignedRegion;
};

// ------------------------------------------------------------------------------------

class FacePipelineFaceTagsIfaceList : public QList<FacePipelineFaceTagsIface>
{
public:

    FacePipelineFaceTagsIfaceList()  = default;
    explicit FacePipelineFaceTagsIfaceList(const QList<FaceTagsIface>& faces);
    ~FacePipelineFaceTagsIfaceList() = default;

    FacePipelineFaceTagsIfaceList& operator=(const QList<FaceTagsIface>& faces);

public:

    void setRole(FacePipelineFaceTagsIface::Roles role);
    void clearRole(FacePipelineFaceTagsIface::Roles role);
    void replaceRole(FacePipelineFaceTagsIface::Roles remove,
                     FacePipelineFaceTagsIface::Roles add);

    QList<FaceTagsIface> toFaceTagsIfaceList()                                        const;

    FacePipelineFaceTagsIfaceList facesForRole(FacePipelineFaceTagsIface::Roles role) const;
};

// ------------------------------------------------------------------------------------

class FacePipelinePackage
{
public:

    enum ProcessFlag
    {
        NotProcessed            = 0,
        PreviewImageLoaded      = 1 << 0,
        ProcessedByDetector     = 1 << 1,
        ProcessedByRecognizer   = 1 << 2,
        WrittenToDatabase       = 1 << 3,
        ProcessedByTrainer      = 1 << 4
    };
    Q_DECLARE_FLAGS(ProcessFlags, ProcessFlag)

public:

    FacePipelinePackage()  = default;
    ~FacePipelinePackage() = default;

public:

    ItemInfo                      info;
    DImg                          image;
    QList<QRectF>                 detectedFaces;
    QList<Identity>               recognitionResults;
    FacePipelineFaceTagsIfaceList databaseFaces;

    ProcessFlags                  processFlags        = NotProcessed;
};

// ------------------------------------------------------------------------------------

class Q_DECL_HIDDEN FacePipelineExtendedPackage : public FacePipelinePackage,
                                                  public QSharedData
{
public:

    FacePipelineExtendedPackage()  = default;
    ~FacePipelineExtendedPackage() = default;

    bool operator==(const LoadingDescription& description) const;

public:

    QString                                                           filePath;
    typedef QExplicitlySharedDataPointer<FacePipelineExtendedPackage> Ptr;
};

// ----------------------------------------------------------------------------------------

class Q_DECL_HIDDEN PackageLoadingDescriptionList : public QList<FacePipelineExtendedPackage::Ptr>
{
public:

    PackageLoadingDescriptionList()  = default;
    ~PackageLoadingDescriptionList() = default;

    FacePipelineExtendedPackage::Ptr take(const LoadingDescription& description);
};

} // namespace Digikam

Q_DECLARE_METATYPE(Digikam::FacePipelinePackage)
Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::FacePipelineFaceTagsIface::Roles)
Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::FacePipelinePackage::ProcessFlags)
