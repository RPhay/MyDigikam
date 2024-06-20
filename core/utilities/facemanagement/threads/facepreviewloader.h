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

#pragma once

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

    void process(const FacePipelineExtendedPackage::Ptr& package);
    void slotImageLoaded(const LoadingDescription& loadingDescription, const DImg& img);

Q_SIGNALS:

    void processed(const FacePipelineExtendedPackage::Ptr& package);

protected:

    PackageLoadingDescriptionList scheduledPackages;

    /// upper limit for memory cost
    int                           maximumSentOutPackages = qMin(QThread::idealThreadCount(), 4);

    FacePipeline::Private* const  d                      = nullptr;

private:

    // Disable
    FacePreviewLoader(const FacePreviewLoader&)            = delete;
    FacePreviewLoader& operator=(const FacePreviewLoader&) = delete;
};

} // namespace Digikam
