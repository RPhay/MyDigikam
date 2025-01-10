/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-11-08
 * Description : Object detection and image auto-tagging engine
 *
 * SPDX-FileCopyrightText: 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2024      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsengine.h"

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

#include "facialrecognition_wrapper.h"
#include "digikam_debug.h"
#include "dnotificationwidget.h"
#include "coredb.h"
#include "album.h"
#include "albummanager.h"
#include "albumpointer.h"
#include "autotagsscansettings.h"
#include "iteminfojob.h"
#include "mlpipelinepackagenotify.h"
#include "autotagspipelineobject.h"

namespace Digikam
{

// class Q_DECL_HIDDEN BenchmarkMessageDisplay : public QWidget
// {
//     Q_OBJECT

// public:

//     explicit BenchmarkMessageDisplay(const QString& richText)
//         : QWidget(nullptr)
//     {
//         setAttribute(Qt::WA_DeleteOnClose);

//         QVBoxLayout* const vbox     = new QVBoxLayout;
//         QTextEdit* const edit       = new QTextEdit;
//         vbox->addWidget(edit, 1);
//         QPushButton* const okButton = new QPushButton(i18n("OK"));
//         vbox->addWidget(okButton, 0, Qt::AlignRight);

//         setLayout(vbox);

//         connect(okButton, SIGNAL(clicked()),
//                 this, SLOT(close()));

//         edit->setHtml(richText);
//         QApplication::clipboard()->setText(edit->toPlainText());

//         resize(500, 400);
//         show();
//         raise();
//     }

// private:

//     // Disable
//     BenchmarkMessageDisplay(QWidget*);
// };

// --------------------------------------------------------------------------

class Q_DECL_HIDDEN AutotagsEngine::Private
{
public:

    Private() = default;

    AutotagsScanSettings            settings;
    bool                            benchmark       = false;

    AlbumPointerList<>              albumTodoList;
    ItemInfoList                    infoTodoList;
    QList<qlonglong>                idsTodoList;

    ItemInfoJob                     albumListing;
    AutotagsPipelineBase*           newPipeline     = nullptr;

    int totalTagsAdded                              = 0;
};

AutotagsEngine::AutotagsEngine(AutotagsScanSettings _settings, ProgressItem* const parent)
    : MaintenanceTool(QLatin1String("AutotagsEngine"), parent),
      settings       (_settings),
      d              (new Private)
{

    d->newPipeline = new AutotagsPipelineObject(_settings);

    connect(d->newPipeline, SIGNAL(finished()),
            this, SLOT(slotDone()));

    connect(d->newPipeline, SIGNAL(processed(const MLPipelinePackageNotify::Ptr&)),
            this, SLOT(slotShowOneDetected(const MLPipelinePackageNotify::Ptr&)));

    connect(d->newPipeline, SIGNAL(skipped(const MLPipelinePackageNotify::Ptr&)),
            this, SLOT(slotImagesSkipped(const MLPipelinePackageNotify::Ptr&)));

    connect(this, SIGNAL(progressItemCanceled(ProgressItem*)),
            this, SLOT(slotCancel()));

    connect(d->newPipeline, SIGNAL(signalUpdateItemCount(const qlonglong)),
            this, SLOT(slotUpdateItemCount(const qlonglong)));

}

AutotagsEngine::~AutotagsEngine()
{
    delete d->newPipeline;
    delete d;
}

void AutotagsEngine::slotStart()
{
    MaintenanceTool::slotStart();

    setLabel(i18n("Autotags Assignment"));
    setThumbnail(QIcon::fromTheme(QLatin1String("tag")).pixmap(48));

    // Set label depending on settings.

    if      (settings.albums.size() > 0)
    {
        if (settings.albums.size() == 1)
        {
            setLabel(i18n("Scan for objects in album: %1", settings.albums.first()->title()));
        }
        else
        {
            setLabel(i18n("Scan for objects in %1 albums", settings.albums.size()));
        }
    }
    // else if (d->infoTodoList.size() > 0)
    // {
    //     if (d->infoTodoList.size() == 1)
    //     {
    //         setLabel(i18n("Scan for objects in image: %1", d->infoTodoList.first().name()));
    //     }
    //     else
    //     {
    //         setLabel(i18n("Scan for objects in %1 images", d->infoTodoList.size()));
    //     }
    // }
    // else
    // {
    //     setLabel(i18n("Updating tags database"));
    // }

    ProgressManager::addProgressItem(this);

    // if      (d->source == AutotagsEngine::Infos)
    // {
    //     int total = d->infoTodoList.count();
    //     qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

    //     setTotalItems(total);

    //     if (d->infoTodoList.isEmpty())
    //     {
    //         slotDone();

    //         return;
    //     }

    //     // slotItemsInfo(d->infoTodoList);

    //     if (!d->newPipeline->start())
    //     {
    //         Q_EMIT signalScanNotification(QString(i18n("Error starting autotag detection.")), DNotificationWidget::Error);

    //         slotDone();

    //         return;       
    //     }
        
    //     return;
    // }
    // else if (d->source == AutotagsEngine::Ids)
    // {
    //     ItemInfoList itemInfos(d->idsTodoList);

    //     int total = itemInfos.count();
    //     qCDebug(DIGIKAM_GENERAL_LOG) << "Total is" << total;

    //     setTotalItems(total);

    //     if (itemInfos.isEmpty())
    //     {
    //         slotDone();

    //         return;
    //     }

    //     // slotItemsInfo(itemInfos);

    //     if (!d->newPipeline->start())
    //     {
    //         Q_EMIT signalScanNotification(QString(i18n("Error starting autotag detection.")), DNotificationWidget::Error);
    //     }
        
    //     return;
    // }

    setUsesBusyIndicator(true);

    // Get total count, cached by AlbumManager.

    QHash<int, int> palbumCounts;
    QHash<int, int> talbumCounts;
    bool hasPAlbums = false;
    bool hasTAlbums = false;

    for (Album* const album : std::as_const(settings.albums))
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

    // // First, we use the progressValueMap map to store absolute counts.

    QHash<Album*, int> progressValueMap;

    for (Album* const album : std::as_const(settings.albums))
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
        Q_EMIT signalScanNotification(QString(i18n("Error starting object detection.")), DNotificationWidget::Error);
    }
}

void AutotagsEngine::slotUpdateItemCount(const qlonglong itemCount)
{
    setTotalItems(itemCount);
}

void AutotagsEngine::slotDone()
{
    // if (d->benchmark)
    // {
    //     new BenchmarkMessageDisplay(d->pipeline.benchmarkResult());
    // }

    QString lbl;

    if (totalItems() > 1)
    {
        lbl.append(i18n("Items scanned for objects: %1\n", totalItems()));
    }
    else
    {
        lbl.append(i18n("Item scanned for objects: %1\n", totalItems()));
    }

    // if (d->totalFacesFound > 1)
    // {
    //     lbl.append(i18n("Faces found: %1", d->totalFacesFound));
    // }
    // else
    // {
    //     lbl.append(i18n("Face found: %1", d->totalFacesFound));
    // }

    setLabel(lbl);

    // Dispatch scan resume to the icon-view info pop-up.

    Q_EMIT signalScanNotification(lbl, DNotificationWidget::Information);

    MaintenanceTool::slotDone();
}

void AutotagsEngine::slotCancel()
{
    d->newPipeline->cancel();
    MaintenanceTool::slotCancel();
}

void AutotagsEngine::slotImagesSkipped(const MLPipelinePackageNotify::Ptr& package)
{
    Q_UNUSED(package);
    advance(1);
    // delete package;
}

void AutotagsEngine::slotShowOneDetected(const MLPipelinePackageNotify::Ptr& package)
{
    setThumbnail(package->thumbnail);

    QString lbl = i18n("Scanned for objects: %1\n", package->name);
    lbl.append(i18n("Path: %1\n", package->path));

    setLabel(lbl);
    advance(1);
}

} // namespace Digikam

#include "moc_autotagsengine.cpp"
