/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-18
 * Description : batch face detection
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Andi Clemens <andi dot clemens at gmail dot com>
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

#include "facesdetector.h"

// Qt includes

#include <QClipboard>
#include <QVBoxLayout>
#include <QTimer>
#include <QIcon>
#include <QPushButton>
#include <QApplication>
#include <QTextEdit>
#include <QHash>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <ksharedconfig.h>

// Local includes

#include "facialrecognition_wrapper.h"
#include "digikam_debug.h"
#include "coredb.h"
#include "album.h"
#include "albummanager.h"
#include "albumpointer.h"
#include "facepipeline.h"
#include "facescansettings.h"
#include "iteminfo.h"
#include "iteminfojob.h"
#include "facetags.h"

namespace Digikam
{

class Q_DECL_HIDDEN BenchmarkMessageDisplay : public QWidget
{
    Q_OBJECT

public:

    explicit BenchmarkMessageDisplay(const QString& richText)
        : QWidget(nullptr)
    {
        setAttribute(Qt::WA_DeleteOnClose);

        QVBoxLayout* const vbox     = new QVBoxLayout;
        QTextEdit* const edit       = new QTextEdit;
        vbox->addWidget(edit, 1);
        QPushButton* const okButton = new QPushButton(i18n("OK"));
        vbox->addWidget(okButton, 0, Qt::AlignRight);

        setLayout(vbox);

        connect(okButton, SIGNAL(clicked()),
                this, SLOT(close()));

        edit->setHtml(richText);
        QApplication::clipboard()->setText(edit->toPlainText());

        resize(500, 400);
        show();
        raise();
    }

private:

    // Disable
    BenchmarkMessageDisplay(QWidget*);
};

// --------------------------------------------------------------------------

class Q_DECL_HIDDEN FacesDetector::Private
{
public:

    explicit Private()
      : source   (FacesDetector::Albums),
        benchmark(false)
    {
    }

    FacesDetector::InputSource source;
    bool                       benchmark;

    AlbumPointerList<>         albumTodoList;
    ItemInfoList               infoTodoList;
    QList<qlonglong>           idsTodoList;

    ItemInfoJob                albumListing;
    FacePipeline               pipeline;
};

FacesDetector::FacesDetector(const FaceScanSettings& settings, ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("FacesDetector"), parent),
      d              (new Private)
{
    setLabel(i18n("Updating faces database."));
    ProgressManager::addProgressItem(this);

    if      (settings.task == FaceScanSettings::RetrainAll)
    {
        // clear all training data in the database
        FacialRecognitionWrapper().clearAllTraining(QLatin1String("digikam"));
        d->pipeline.plugRetrainingDatabaseFilter();
        d->pipeline.plugTrainer();
        d->pipeline.construct();
    }
    else if (settings.task == FaceScanSettings::BenchmarkDetection)
    {
        d->benchmark = true;
        d->pipeline.plugDatabaseFilter(FacePipeline::ScanAll);
        d->pipeline.plugFacePreviewLoader();

        if (settings.useFullCpu)
        {
            d->pipeline.plugParallelFaceDetectors();
        }
        else
        {
            d->pipeline.plugFaceDetector();
        }

        d->pipeline.plugDetectionBenchmarker();
        d->pipeline.construct();
    }
    else if (settings.task == FaceScanSettings::BenchmarkRecognition)
    {
        d->benchmark = true;
        d->pipeline.plugRetrainingDatabaseFilter();
        d->pipeline.plugFaceRecognizer();
        d->pipeline.plugRecognitionBenchmarker();
        d->pipeline.construct();
    }
    else if ((settings.task == FaceScanSettings::DetectAndRecognize) ||
             (settings.task == FaceScanSettings::Detect))
    {
        FacePipeline::FilterMode filterMode;
        FacePipeline::WriteMode  writeMode;

        if      (settings.alreadyScannedHandling == FaceScanSettings::Skip)
        {
            filterMode = FacePipeline::SkipAlreadyScanned;
            writeMode  = FacePipeline::NormalWrite;
        }
        else if (settings.alreadyScannedHandling == FaceScanSettings::Rescan)
        {
            filterMode = FacePipeline::ScanAll;
            writeMode  = FacePipeline::OverwriteUnconfirmed;
        }
        else // FaceScanSettings::Merge
        {
            filterMode = FacePipeline::ScanAll;
            writeMode  = FacePipeline::NormalWrite;
        }

        d->pipeline.plugDatabaseFilter(filterMode);
        d->pipeline.plugFacePreviewLoader();

        if (settings.useFullCpu)
        {
            d->pipeline.plugParallelFaceDetectors();
        }
        else
        {
            d->pipeline.plugFaceDetector();
        }

        if (settings.task == FaceScanSettings::DetectAndRecognize)
        {
            //d->pipeline.plugRerecognizingDatabaseFilter();
            d->pipeline.plugFaceRecognizer();
        }

        d->pipeline.plugDatabaseWriter(writeMode);
        d->pipeline.setAccuracyAndModel(settings.accuracy,
                                        settings.useYoloV3);
        d->pipeline.construct();
    }
    else // FaceScanSettings::RecognizeMarkedFaces
    {
        d->pipeline.plugRerecognizingDatabaseFilter();
        d->pipeline.plugFaceRecognizer();
        d->pipeline.plugDatabaseWriter(FacePipeline::NormalWrite);
        d->pipeline.setAccuracyAndModel(settings.accuracy,
                                        settings.useYoloV3);
        d->pipeline.construct();
    }

    connect(&d->albumListing, SIGNAL(signalItemsInfo(ItemInfoList)),
            this, SLOT(slotItemsInfo(ItemInfoList)));

    connect(&d->albumListing, SIGNAL(signalCompleted()),
            this, SLOT(slotContinueAlbumListing()));

    connect(&d->pipeline, SIGNAL(finished()),
            this, SLOT(slotContinueAlbumListing()));

    connect(&d->pipeline, SIGNAL(processed(FacePipelinePackage)),
            this, SLOT(slotShowOneDetected(FacePipelinePackage)));

    connect(&d->pipeline, SIGNAL(skipped(QList<ItemInfo>)),
            this, SLOT(slotImagesSkipped(QList<ItemInfo>)));

    connect(this, SIGNAL(progressItemCanceled(ProgressItem*)),
            this, SLOT(slotCancel()));

    if      (settings.task == FaceScanSettings::RecognizeMarkedFaces)
    {
        d->idsTodoList = CoreDbAccess().db()->
            getImagesWithImageTagProperty(FaceTags::unknownPersonTagId(),
                                          ImageTagPropertyName::autodetectedFace());

        d->source = FacesDetector::Ids;
    }
    else if (settings.task == FaceScanSettings::RetrainAll)
    {
        d->idsTodoList = CoreDbAccess().db()->
            getImagesWithProperty(ImageTagPropertyName::tagRegion());

        d->source = FacesDetector::Ids;
    }
    else if (settings.albums.isEmpty() && settings.infos.isEmpty())
    {
        d->albumTodoList = AlbumManager::instance()->allPAlbums();
        d->source = FacesDetector::Albums;
    }
    else if (!settings.albums.isEmpty())
    {
        d->albumTodoList = settings.albums;
        d->source = FacesDetector::Albums;
    }
    else
    {
        d->infoTodoList = settings.infos;
        d->source = FacesDetector::Infos;
    }
}

FacesDetector::~FacesDetector()
{
    delete d;
}

void FacesDetector::slotStart()
{
    MaintenanceTool::slotStart();

    setThumbnail(QIcon::fromTheme(QLatin1String("edit-image-face-show")).pixmap(22));

    if      (d->source == FacesDetector::Infos)
    {
        int total = d->infoTodoList.count();
        qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

        setTotalItems(total);

        if (d->infoTodoList.isEmpty())
        {
            slotDone();
            return;
        }

        slotItemsInfo(d->infoTodoList);
        return;
    }
    else if (d->source == FacesDetector::Ids)
    {
        ItemInfoList itemInfos(d->idsTodoList);

        int total = itemInfos.count();
        qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

        setTotalItems(total);

        if (itemInfos.isEmpty())
        {
            slotDone();
            return;
        }

        slotItemsInfo(itemInfos);
        return;
    }

    setUsesBusyIndicator(true);

    // get total count, cached by AlbumManager

    QMap<int, int> palbumCounts;
    QMap<int, int> talbumCounts;
    bool hasPAlbums = false;
    bool hasTAlbums = false;

    foreach (Album* const album, d->albumTodoList)
    {
        if (album->type() == Album::PHYSICAL)
        {
            hasPAlbums = true;
        }
        else
        {
            hasTAlbums = true;
        }
    }

    palbumCounts = AlbumManager::instance()->getPAlbumsCount();
    talbumCounts = AlbumManager::instance()->getTAlbumsCount();

    if (palbumCounts.isEmpty() && hasPAlbums)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        palbumCounts = CoreDbAccess().db()->getNumberOfImagesInAlbums();
        QApplication::restoreOverrideCursor();
    }

    if (talbumCounts.isEmpty() && hasTAlbums)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        talbumCounts = CoreDbAccess().db()->getNumberOfImagesInTags();
        QApplication::restoreOverrideCursor();
    }

    // first, we use the progressValueMap map to store absolute counts

    QHash<Album*, int> progressValueMap;

    foreach (Album* const album, d->albumTodoList)
    {
        if (album->type() == Album::PHYSICAL)
        {
            progressValueMap[album] = palbumCounts.value(album->id());
        }
        else
        {
            // this is possibly broken of course because we do not know if images have multiple tags,
            // but there's no better solution without expensive operation

            progressValueMap[album] = talbumCounts.value(album->id());
        }
    }

    // second, calculate (approximate) overall sum

    int total = 0;

    foreach (int count, progressValueMap)
    {
        total += count;
    }

    total = qMax(1, total);
    qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

    setUsesBusyIndicator(false);
    setTotalItems(total);

    slotContinueAlbumListing();
}

void FacesDetector::slotContinueAlbumListing()
{
    if (d->source != FacesDetector::Albums)
    {
        slotDone();
        return;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << d->albumListing.isRunning() << !d->pipeline.hasFinished();

    // we get here by the finished signal from both, and want both to have finished to continue

    if (d->albumListing.isRunning() || !d->pipeline.hasFinished())
    {
        return;
    }

    // list can have null pointer if album was deleted recently

    Album* album = nullptr;

    do
    {
        if (d->albumTodoList.isEmpty())
        {
            slotDone();
            return;
        }

        album = d->albumTodoList.takeFirst();
    }
    while (!album);

    d->albumListing.allItemsFromAlbum(album);
}

void FacesDetector::slotItemsInfo(const ItemInfoList& items)
{
    d->pipeline.process(items);
}

void FacesDetector::slotDone()
{
    if (d->benchmark)
    {
        new BenchmarkMessageDisplay(d->pipeline.benchmarkResult());
    }

    // Switch on scanned for faces flag on digiKam config file.

    KSharedConfig::openConfig()->group("General Settings").writeEntry("Face Scanner First Run", true);

    MaintenanceTool::slotDone();
}

void FacesDetector::slotCancel()
{
    d->pipeline.shutDown();
    MaintenanceTool::slotCancel();
}

void FacesDetector::slotImagesSkipped(const QList<ItemInfo>& infos)
{
    advance(infos.size());
}

void FacesDetector::slotShowOneDetected(const FacePipelinePackage& /*package*/)
{
    advance(1);
}

} // namespace Digikam

#include "facesdetector.moc"
