/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACE_PREVIEW_LOADER_H
#define DIGIKAM_FACE_PREVIEW_LOADER_H

// Local includes

#include "facepipeline_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN FacePreviewLoader : public PreviewLoadThread
{
    Q_OBJECT

public:

    explicit FacePreviewLoader(FacePipeline::Private* const dd);
    ~FacePreviewLoader() override;

    void cancel();
    bool sentOutLimitReached() const;
    void checkRestart();

public Q_SLOTS:

    void process(FacePipelineExtendedPackage::Ptr package);
    void slotImageLoaded(const LoadingDescription& loadingDescription, const DImg& img);

Q_SIGNALS:

    void processed(FacePipelineExtendedPackage::Ptr package);

protected:

    PackageLoadingDescriptionList scheduledPackages;
    int                           maximumSentOutPackages;
    FacePipeline::Private* const  d;

private:

    // Disable
    FacePreviewLoader(const FacePreviewLoader&)            = delete;
    FacePreviewLoader& operator=(const FacePreviewLoader&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_PREVIEW_LOADER_H
