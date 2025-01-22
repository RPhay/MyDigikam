/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-11-08
 * Description : face detection, recognition, and training controller
 *
 * SPDX-FileCopyrightText: 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facesengine.h"

// Qt includes

#include <QClipboard>
#include <QVBoxLayout>
#include <QTimer>
#include <QIcon>
#include <QPushButton>
#include <QApplication>
#include <QTextEdit>
#include <QHash>
#include <QPixmap>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"
#include "dnotificationwidget.h"
#include "coredb.h"
#include "album.h"
#include "albummanager.h"
#include "albumpointer.h"
#include "facescansettings.h"
#include "iteminfojob.h"
#include "facetags.h"

#include "mlpipelinepackagenotify.h"
#include "facepipelinedetectrecognize.h"
#include "facepipelinerecognize.h"
#include "facepipelineretrain.h"
#include "facepipelinereset.h"

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

class Q_DECL_HIDDEN FacesEngine::Private
{
public:

    Private() = default;

public:

    FacesEngine::InputSource    source          = FacesEngine::Albums;
    bool                        benchmark       = false;

    AlbumPointerList<>          albumTodoList;
    ItemInfoList                infoTodoList;
    QList<qlonglong>            idsTodoList;

    ItemInfoJob                 albumListing;
    FacePipelineBase*           newPipeline     = nullptr;

    int totalFacesFound                         = 0;
};

FacesEngine::FacesEngine(const FaceScanSettings& settings, ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("FacesEngine"), parent),
      d              (new Private)
{
    switch (settings.task)
    {
        case FaceScanSettings::DetectAndRecognize:
        {
            d->newPipeline = new FacePipelineDetectRecognize(settings);
            break;
        }

        case FaceScanSettings::RecognizeMarkedFaces:
        {
            d->newPipeline = new FacePipelineRecognize(settings);
            break;
        }

        case FaceScanSettings::RetrainAll:
        {
            d->newPipeline = new FacePipelineRetrain(settings);
            break;
        }

        case FaceScanSettings::Reset:
        {
            d->newPipeline = new FacePipelineReset(settings);
            break;
        }
    }

    connect(d->newPipeline, SIGNAL(finished()),
            this, SLOT(slotDone()));

    connect(d->newPipeline, SIGNAL(processed(MLPipelinePackageNotify::Ptr)),
            this, SLOT(slotShowOneDetected(MLPipelinePackageNotify::Ptr)));

    connect(d->newPipeline, SIGNAL(skipped(MLPipelinePackageNotify::Ptr)),
            this, SLOT(slotImagesSkipped(MLPipelinePackageNotify::Ptr)));

    connect(this, SIGNAL(progressItemCanceled(ProgressItem*)),
            this, SLOT(slotCancel()));

    connect(d->newPipeline, SIGNAL(signalUpdateItemCount(qlonglong)),
            this, SLOT(slotUpdateItemCount(qlonglong)));

    if      (
             settings.wholeAlbums &&
             (settings.task == FaceScanSettings::RecognizeMarkedFaces)
            )
    {
        d->idsTodoList   = CoreDbAccess().db()->getImagesWithImageTagProperty(FaceTags::unknownPersonTagId(),
                                                                              ImageTagPropertyName::autodetectedFace());

        d->source        = FacesEngine::Ids;
    }
    else if (settings.task == FaceScanSettings::RetrainAll)
    {
        d->idsTodoList   = CoreDbAccess().db()->getImagesWithProperty(ImageTagPropertyName::tagRegion());

        d->source        = FacesEngine::Ids;
    }
    else if (settings.albums.isEmpty() && settings.infos.isEmpty())
    {
        d->albumTodoList = AlbumManager::instance()->allPAlbums();
        d->source        = FacesEngine::Albums;
    }
    else if (!settings.albums.isEmpty())
    {
        d->albumTodoList = settings.albums;
        d->source        = FacesEngine::Albums;
    }
    else
    {
        d->infoTodoList  = settings.infos;
        d->source        = FacesEngine::Infos;
    }
}

FacesEngine::~FacesEngine()
{
    delete d->newPipeline;
    delete d;
}

void FacesEngine::slotStart()
{
    MaintenanceTool::slotStart();

    setThumbnail(QIcon::fromTheme(QLatin1String("edit-image-face-detect")).pixmap(48));

    // Set label depending on settings.

    if      (d->albumTodoList.size() > 0)
    {
        if (d->albumTodoList.size() == 1)
        {
            setLabel(i18n("Scan for faces in album: %1", d->albumTodoList.first()->title()));
        }
        else
        {
            setLabel(i18n("Scan for faces in %1 albums", d->albumTodoList.size()));
        }
    }
    else if (d->infoTodoList.size() > 0)
    {
        if (d->infoTodoList.size() == 1)
        {
            setLabel(i18n("Scan for faces in image: %1", d->infoTodoList.first().name()));
        }
        else
        {
            setLabel(i18n("Scan for faces in %1 images", d->infoTodoList.size()));
        }
    }
    else
    {
        setLabel(i18n("Updating faces database"));
    }

    ProgressManager::addProgressItem(this);

    if      (d->source == FacesEngine::Infos)
    {
        int total = d->infoTodoList.count();
        qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

        setTotalItems(total);

        if (d->infoTodoList.isEmpty())
        {
            slotDone();

            return;
        }

        if (!d->newPipeline->start())
        {
            Q_EMIT signalScanNotification(QString(i18n("Error starting face detection.")),
                                          DNotificationWidget::Error);

            slotDone();

            return;
        }

        return;
    }
    else if (d->source == FacesEngine::Ids)
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

        if (!d->newPipeline->start())
        {
            Q_EMIT signalScanNotification(QString(i18n("Error starting face detection.")),
                                          DNotificationWidget::Error);
        }

        return;
    }

    setUsesBusyIndicator(true);

    // Get total count, cached by AlbumManager.

    QHash<int, int> palbumCounts;
    QHash<int, int> talbumCounts;
    bool hasPAlbums = false;
    bool hasTAlbums = false;

    for (Album* const album : std::as_const(d->albumTodoList))
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

    // First, we use the progressValueMap map to store absolute counts.

    QHash<Album*, int> progressValueMap;

    for (Album* const album : std::as_const(d->albumTodoList))
    {
        if (album->type() == Album::PHYSICAL)
        {
            progressValueMap[album] = palbumCounts.value(album->id());
        }
        else
        {
            // This is possibly broken of course because we do not know if images have multiple tags,
            // but there's no better solution without expensive operation.

            progressValueMap[album] = talbumCounts.value(album->id());
        }
    }

    // Second, calculate (approximate) overall sum.

    int total = 0;

    for (int count : std::as_const(progressValueMap))
    {
        // cppcheck-suppress useStlAlgorithm
        total += count;
    }

    total = qMax(1, total);
    qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

    setUsesBusyIndicator(false);
    setTotalItems(total);

    if (!d->newPipeline->start())
    {
        Q_EMIT signalScanNotification(QString(i18n("Error starting face detection.")), DNotificationWidget::Error);
    }
}

void FacesEngine::slotUpdateItemCount(const qlonglong itemCount)
{
    setTotalItems(itemCount);
}

void FacesEngine::slotDone()
{
/*
    if (d->benchmark)
    {
        new BenchmarkMessageDisplay(d->pipeline.benchmarkResult());
    }
*/
    QString lbl;

    if (totalItems() > 1)
    {
        lbl.append(i18n("Items scanned for faces: %1\n", totalItems()));
    }
    else
    {
        lbl.append(i18n("Item scanned for faces: %1\n", totalItems()));
    }

    if (d->totalFacesFound > 1)
    {
        lbl.append(i18n("Faces found: %1", d->totalFacesFound));
    }
    else
    {
        lbl.append(i18n("Face found: %1", d->totalFacesFound));
    }

    setLabel(lbl);

    // Dispatch scan resume to the icon-view info pop-up.

    Q_EMIT signalScanNotification(lbl, DNotificationWidget::Information);

    // Switch on scanned for faces flag on digiKam config file.

    KSharedConfig::openConfig()->group(QLatin1String("General Settings"))
                                       .writeEntry("Face Scanner First Run", true);

    MaintenanceTool::slotDone();
}

void FacesEngine::slotCancel()
{
    d->newPipeline->cancel();
    MaintenanceTool::slotCancel();
}

void FacesEngine::slotImagesSkipped(const MLPipelinePackageNotify::Ptr& package)
{
    Q_UNUSED(package);

    advance(1);
}

void FacesEngine::slotShowOneDetected(const MLPipelinePackageNotify::Ptr& package)
{
    setThumbnail(package->thumbnail);

    QString lbl = i18n("Face Scanning: %1\n", package->name);
    lbl.append(i18n("Album: %1\n", package->path));
    if (package->processed > 0)
    {
        if (package->processed == 1)
        {
            lbl.append(i18n("Found %1 face.", package->processed));
        }
        else
        {
            lbl.append(i18n("Found %1 faces.", package->processed));
        }

        if (!package->displayData.isEmpty())
        {
            lbl.append(QStringLiteral("  "));
            lbl.append(i18n("Recognized: %1", package->displayData));
        }
    }
    // else
    // {
    //     lbl.append(i18n("No faces found."));
    // }

    setLabel(lbl);
    advance(1);
}

} // namespace Digikam

#include "facesengine.moc"
