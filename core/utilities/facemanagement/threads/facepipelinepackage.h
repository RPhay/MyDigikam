/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_FACE_PIPELINE_PACKAGE_H
#define DIGIKAM_FACE_PIPELINE_PACKAGE_H

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

    FacePipelineFaceTagsIface();
    explicit FacePipelineFaceTagsIface(const FaceTagsIface& face);
    ~FacePipelineFaceTagsIface();

public:

    Roles     roles;
    int       assignedTagId;
    TagRegion assignedRegion;
};

// ------------------------------------------------------------------------------------

class FacePipelineFaceTagsIfaceList : public QList<FacePipelineFaceTagsIface>
{
public:

    FacePipelineFaceTagsIfaceList();
    explicit FacePipelineFaceTagsIfaceList(const QList<FaceTagsIface>& faces);
    ~FacePipelineFaceTagsIfaceList();

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

    explicit FacePipelinePackage();
    ~FacePipelinePackage();

public:

    ItemInfo                      info;
    DImg                          image;
    QList<QRectF>                 detectedFaces;
    QList<Identity>               recognitionResults;
    FacePipelineFaceTagsIfaceList databaseFaces;

    ProcessFlags                  processFlags;
};

// ------------------------------------------------------------------------------------

class Q_DECL_HIDDEN FacePipelineExtendedPackage : public FacePipelinePackage,
                                                  public QSharedData
{
public:

    explicit FacePipelineExtendedPackage();
    ~FacePipelineExtendedPackage();

    bool operator==(const LoadingDescription& description) const;

public:

    QString                                                           filePath;
    typedef QExplicitlySharedDataPointer<FacePipelineExtendedPackage> Ptr;
};

// ----------------------------------------------------------------------------------------

class Q_DECL_HIDDEN PackageLoadingDescriptionList : public QList<FacePipelineExtendedPackage::Ptr>
{
public:

    explicit PackageLoadingDescriptionList();
    ~PackageLoadingDescriptionList();

    FacePipelineExtendedPackage::Ptr take(const LoadingDescription& description);
};

} // namespace Digikam

Q_DECLARE_METATYPE(Digikam::FacePipelinePackage)
Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::FacePipelineFaceTagsIface::Roles)
Q_DECLARE_OPERATORS_FOR_FLAGS(Digikam::FacePipelinePackage::ProcessFlags)

#endif // DIGIKAM_FACE_PIPELINE_PACKAGE_H
