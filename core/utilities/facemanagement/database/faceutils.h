/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-08-08
 * Description : FacesEngine database interface allowing easy manipulation of face tags
 *
 * SPDX-FileCopyrightText: 2010-2011 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QStringList>

// Local includes

#include "identity.h"
#include "facialrecognition_wrapper.h"
#include "iteminfo.h"
#include "facetagseditor.h"
#include "digikam_export.h"

class QImage;

namespace Digikam
{

class DImg;
class ThumbnailLoadThread;
class ThumbnailImageCatcher;

class FaceUtils : public QObject,
                  public FaceTagsEditor
{
    Q_OBJECT

public:

    enum FaceRecognitionSteps
    {
        DetectFaceRegions,
        DetectAndRecognize
    };

public:

    explicit FaceUtils(QObject* const parent = nullptr);
    ~FaceUtils()                                                                              override;

    // --- Face detection and recognition ---

    /**
     * The given face list is a result of automatic detection and possibly recognition.
     * The results are written to the database and merged with existing entries.
     * The returned list contains the faces written to the database and has the same size as the given list.
     * If a face was skipped (because of an existing entry), a null FaceTagsIface will be at this place.
     */
    QList<FaceTagsIface> writeUnconfirmedResults(qlonglong imageid,
                                                 const QList<QRectF>& detectedFaces,
                                                 const QList<Identity>& recognitionResults,
                                                 const QSize& fullSize);

    // --- Status flags ---

    /**
     * Tells if the image has been scanned for faces or not
     */
    bool                hasBeenScanned(const ItemInfo& info)                            const;
    bool                hasBeenScanned(qlonglong imageid)                               const;

    /**
     * Marks the image as scanned for faces.
     */
    void                markAsScanned(qlonglong imageid, bool hasBeenScanned = true)    const;
    void                markAsScanned(const ItemInfo& info, bool hasBeenScanned = true) const;

    // --- Utilities ---

    /**
     * This uses a thumbnail load thread to load the image detail.
     * If requested, the faces will be scaled to the given (fixed) size.
     */
/*
    void                 fillImageInFaces(ThumbnailImageCatcher* const catcher,
                                          const QString& filePath,
                                          QList<Face>& faceList,
                                          const QSize& scaleSize = QSize())             const;
*/

    /**
     * Store the needed thumbnails for the given faces. This can be a huge optimization
     * when the has already been loaded anyway.
     */
    void                 storeThumbnails(ThumbnailLoadThread* const thread,
                                         const QString& filePath,
                                         const QList<FaceTagsIface>& databaseFaces,
                                         const DImg& image);

    /**
     * Conversion
     */
    QList<FaceTagsIface> toFaceTagsIfaces(qlonglong imageid,
                                         const QList<QRectF>& detectedFaces,
                                         const QList<Identity>& recognitionResults,
                                         const QSize& fullSize)                         const;

    /**
     * For display, it may be desirable to display a slightly larger region than the strict
     * face rectangle. This returns a pixel margin commonly used to increase the rectangle size
     * in all four directions.
     */
    static QRect         faceRectToDisplayRect(const QRect& rect);

    // TODO: investigate this method
    Identity identityForTag(int tagId, FacialRecognitionWrapper& recognizer)            const;
    int      tagForIdentity(const Identity& identity)                                   const;

protected:

    // Reimplemented
    void addNormalTag(qlonglong imageId, int tagId)                                           override;
    void removeNormalTag(qlonglong imageId, int tagId)                                        override;
    void removeNormalTags(qlonglong imageId, const QList<int>& tagIds)                        override;

private:

    // Disable
    FaceUtils(const FaceUtils&)            = delete;
    FaceUtils& operator=(const FaceUtils&) = delete;
};

} // Namespace Digikam
