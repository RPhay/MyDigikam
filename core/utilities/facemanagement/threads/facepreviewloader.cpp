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

#include "facepreviewloader.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

FacePreviewLoader::FacePreviewLoader(FacePipeline::Private* const dd)
    : d(dd)
{
    // upper limit for memory cost

    maximumSentOutPackages = qMin(QThread::idealThreadCount(), 4);

    // this is crucial! Per default, only the last added image will be loaded

    setLoadingPolicy(PreviewLoadThread::LoadingPolicySimpleAppend);

    connect(this, SIGNAL(signalImageLoaded(LoadingDescription,DImg)),
            this, SLOT(slotImageLoaded(LoadingDescription,DImg)));
}

FacePreviewLoader::~FacePreviewLoader()
{
}

void FacePreviewLoader::cancel()
{
    stopAllTasks();
    scheduledPackages.clear();
}

void FacePreviewLoader::process(FacePipelineExtendedPackage::Ptr package)
{
    if (!package->image.isNull())
    {
        Q_EMIT processed(package);
        return;
    }

    scheduledPackages << package;
    loadHighQuality(package->filePath, PreviewSettings::RawPreviewFromRawHalfSize);
/*
    load(package->filePath, 800, MetaEngineSettings::instance()->settings().exifRotate);
    loadHighQuality(package->filePath, MetaEngineSettings::instance()->settings().exifRotate);
*/
    checkRestart();
}

void FacePreviewLoader::slotImageLoaded(const LoadingDescription& loadingDescription, const DImg& img)
{
    FacePipelineExtendedPackage::Ptr package = scheduledPackages.take(loadingDescription);

    if (!package)
    {
        return;
    }

    // Avoid congestion before detection or recognition by pausing the thread.
    // We are throwing around serious amounts of memory!
/*
    //qCDebug(DIGIKAM_GENERAL_LOG) << "sent out packages:"
                                   << d->packagesOnTheRoad - scheduledPackages.size()
                                   << "scheduled:" << scheduledPackages.size();
*/
    if (sentOutLimitReached() && !scheduledPackages.isEmpty())
    {
        stop();
        wait();
    }

    if (qMax(img.width(), img.height()) > 2000)
    {
        package->image     = img.smoothScale(2000,
                                             2000,
                                             Qt::KeepAspectRatio);
    }
    else
    {
        package->image     = img;
    }

    package->processFlags |= FacePipelinePackage::PreviewImageLoaded;

    Q_EMIT processed(package);
}

bool FacePreviewLoader::sentOutLimitReached() const
{
    int packagesInTheFollowingPipeline = d->packagesOnTheRoad - scheduledPackages.size();

    return (packagesInTheFollowingPipeline > maximumSentOutPackages);
}

void FacePreviewLoader::checkRestart()
{
    if (!sentOutLimitReached() && !scheduledPackages.isEmpty())
    {
        start();
    }
}

} // namespace Digikam
