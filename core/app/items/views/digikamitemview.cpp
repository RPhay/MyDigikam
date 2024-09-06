/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-24
 * Description : Qt model-view for items
 *
 * SPDX-FileCopyrightText: 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2011      by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2013      by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "digikamitemview_p.h"

// Qt includes

#include <QApplication>
#include <QPointer>
#include <QAction>
#include <QMenu>
#include <QIcon>
#include <QUrl>

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "coredb.h"
#include "coredboperationgroup.h"
#include "advancedrenamedialog.h"
#include "advancedrenameprocessdialog.h"
#include "assignnameoverlay.h"
#include "contextmenuhelper.h"
#include "coredbaccess.h"
#include "ddragobjects.h"
#include "digikamapp.h"
#include "digikamitemdelegate.h"
#include "itemfacedelegate.h"
#include "dio.h"
#include "groupindicatoroverlay.h"
#include "itemalbumfiltermodel.h"
#include "itemalbummodel.h"
#include "itemdragdrop.h"
#include "itemratingoverlay.h"
#include "itemfullscreenoverlay.h"
#include "itemcoordinatesoverlay.h"
#include "tagslineeditoverlay.h"
#include "imagewindow.h"
#include "fileactionmngr.h"
#include "fileactionprogress.h"
#include "thumbnailloadthread.h"
#include "tagregion.h"
#include "addtagslineedit.h"
#include "facerejectionoverlay.h"
#include "facetagsiface.h"

namespace Digikam
{

DigikamItemView::DigikamItemView(QWidget* const parent)
    : ItemCategorizedView(parent),
      d                  (new Private(this))
{
    installDefaultModels();

    d->editPipeline.plugDatabaseEditor();
    d->editPipeline.plugTrainer();
    d->editPipeline.construct();

    connect(&d->editPipeline, SIGNAL(scheduled()),
            this, SLOT(slotInitProgressIndicator()));

    d->normalDelegate = new DigikamItemDelegate(this);
    d->faceDelegate   = new ItemFaceDelegate(this);

    setItemDelegate(d->normalDelegate);
    setSpacing(10);

    ApplicationSettings* const settings = ApplicationSettings::instance();

    imageFilterModel()->setCategorizationMode(ItemSortSettings::CategoryByAlbum);

    imageAlbumModel()->setThumbnailLoadThread(ThumbnailLoadThread::defaultIconViewThread());

    // Virtual method: use Dynamic binding.

    this->setThumbnailSize(ThumbnailSize(settings->getDefaultIconSize()));

    imageAlbumModel()->setPreloadThumbnails(true);

    imageModel()->setDragDropHandler(new ItemDragDropHandler(imageModel()));
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(false);

    setToolTipEnabled(settings->showToolTipsIsValid());
    imageFilterModel()->setStringTypeNatural(settings->isStringTypeNatural());
    imageFilterModel()->setSortRole((ItemSortSettings::SortRole)settings->getImageSortOrder());
    imageFilterModel()->setSortOrder((ItemSortSettings::SortOrder)settings->getImageSorting());
    imageFilterModel()->setCategorizationMode((ItemSortSettings::CategorizationMode)settings->getImageSeparationMode());
    imageFilterModel()->setCategorizationSortOrder((ItemSortSettings::SortOrder) settings->getImageSeparationSortOrder());

    // selection overlay

    addSelectionOverlay(d->normalDelegate);
    addSelectionOverlay(d->faceDelegate);

    // rotation overlays

    d->rotateLeftOverlay  = ItemRotateOverlay::left(this);
    d->rotateRightOverlay = ItemRotateOverlay::right(this);
    d->fullscreenOverlay  = ItemFullScreenOverlay::instance(this);
    d->updateOverlays();

    // rating overlay

    ItemRatingOverlay* const ratingOverlay = new ItemRatingOverlay(this);
    addOverlay(ratingOverlay);

    // face overlays
    // NOTE: order to plug this overlay is important, else rejection cant be suitable (see bug #324759).

    addAssignNameOverlay(d->faceDelegate);
    addRejectionOverlay(d->faceDelegate);

    GroupIndicatorOverlay* const groupOverlay = new GroupIndicatorOverlay(this);
    addOverlay(groupOverlay);

    addOverlay(new ItemCoordinatesOverlay(this));

    connect(ratingOverlay, SIGNAL(ratingEdited(QList<QModelIndex>,int)),
            this, SLOT(assignRating(QList<QModelIndex>,int)));

    connect(groupOverlay, SIGNAL(toggleGroupOpen(QModelIndex)),
            this, SLOT(groupIndicatorClicked(QModelIndex)));

    connect(groupOverlay, SIGNAL(showButtonContextMenu(QModelIndex,QContextMenuEvent*)),
            this, SLOT(showGroupContextMenu(QModelIndex,QContextMenuEvent*)));

    d->utilities = new ItemViewUtilities(this);

    connect(imageModel()->dragDropHandler(), SIGNAL(assignTags(QList<ItemInfo>,QList<int>)),
            FileActionMngr::instance(), SLOT(assignTags(QList<ItemInfo>,QList<int>)));

    connect(imageModel()->dragDropHandler(), SIGNAL(addToGroup(ItemInfo,QList<ItemInfo>)),
            FileActionMngr::instance(), SLOT(addToGroup(ItemInfo,QList<ItemInfo>)));

    connect(imageModel()->dragDropHandler(), SIGNAL(dragDropSort(ItemInfo,QList<ItemInfo>)),
            this, SLOT(dragDropSort(ItemInfo,QList<ItemInfo>)));

    connect(d->utilities, SIGNAL(editorCurrentUrlChanged(QUrl)),
            this, SLOT(setCurrentUrlWhenAvailable(QUrl)));

    // --- NOTE: use dynamic binding as slotSetupChanged() is a virtual method which can be re-implemented in derived classes.

    connect(settings, &ApplicationSettings::setupChanged,
            this, &DigikamItemView::slotSetupChanged);

    this->slotSetupChanged();
}

DigikamItemView::~DigikamItemView()
{
    delete d;
}

ItemViewUtilities* DigikamItemView::utilities() const
{
    return d->utilities;
}

void DigikamItemView::setThumbnailSize(const ThumbnailSize& size)
{
    imageThumbnailModel()->setPreloadThumbnailSize(size);
    ItemCategorizedView::setThumbnailSize(size);
}

ItemInfoList DigikamItemView::allItemInfos(bool grouping) const
{
    if (grouping)
    {
        return resolveGrouping(ItemCategorizedView::allItemInfos());
    }

    return ItemCategorizedView::allItemInfos();
}

ItemInfoList DigikamItemView::selectedItemInfos(bool grouping) const
{
    if (grouping)
    {
        return resolveGrouping(ItemCategorizedView::selectedItemInfos());
    }

    return ItemCategorizedView::selectedItemInfos();
}

ItemInfoList DigikamItemView::selectedItemInfosCurrentFirst(bool grouping) const
{
    if (grouping)
    {
        return resolveGrouping(ItemCategorizedView::selectedItemInfosCurrentFirst());
    }

    return ItemCategorizedView::selectedItemInfosCurrentFirst();
}

void DigikamItemView::dragDropSort(const ItemInfo& pick, const QList<ItemInfo>& infos)
{
    if (pick.isNull() || infos.isEmpty())
    {
        return;
    }

    ItemInfoList infoList = allItemInfos(false);
    qlonglong counter     = pick.manualOrder();
    bool order            = (ApplicationSettings::instance()->
                               getImageSorting() == Qt::AscendingOrder);
    bool found            = false;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    CoreDbOperationGroup group;
    group.setMaximumTime(200);

    for (ItemInfo info : std::as_const(infoList))
    {
        if      (!found && info.id() == pick.id())
        {
            for (ItemInfo dropInfo : std::as_const(infos))
            {
                dropInfo.setManualOrder(counter);
                counter += (order ? 1 : -1);
            }

            info.setManualOrder(counter);
            found = true;
        }
        else if (found && !infos.contains(info))
        {
            if (
                ( order && info.manualOrder() > counter) ||
                (!order && info.manualOrder() < counter)
               )
            {
                break;
            }

            counter += (order ? 100 : -100);
            info.setManualOrder(counter);
        }

        group.allowLift();
    }

    QApplication::restoreOverrideCursor();

    imageFilterModel()->invalidate();
 }

bool DigikamItemView::allNeedGroupResolving(const OperationType type) const
{
    return needGroupResolving(type, allItemInfos());
}

bool DigikamItemView::selectedNeedGroupResolving(const OperationType type) const
{
    return needGroupResolving(type, selectedItemInfos());
}

int DigikamItemView::fitToWidthIcons()
{
    return delegate()->calculatethumbSizeToFit(viewport()->size().width());
}

void DigikamItemView::slotSetupChanged()
{
    imageFilterModel()->setStringTypeNatural(ApplicationSettings::instance()->isStringTypeNatural());
    setInitialSelectedItem(ApplicationSettings::instance()->getSelectFirstAlbumItem());
    setToolTipEnabled(ApplicationSettings::instance()->showToolTipsIsValid());
    setFont(ApplicationSettings::instance()->getIconViewFont());

    d->updateOverlays();

    ItemCategorizedView::slotSetupChanged();
}

bool DigikamItemView::hasHiddenGroupedImages(const ItemInfo& info) const
{
    return (info.hasGroupedImages()                &&
            !imageFilterModel()->isAllGroupsOpen() &&
            !imageFilterModel()->isGroupOpen(info.id()));
}

ItemInfoList DigikamItemView::imageInfos(const QList<QModelIndex>& indexes,
                                         OperationType type) const
{
    ItemInfoList infos = ItemCategorizedView::imageInfos(indexes);

    if (needGroupResolving(type, infos))
    {
        return resolveGrouping(infos);
    }

    return infos;
}

bool DigikamItemView::getFaceMode() const
{
    return d->faceMode;
}

void DigikamItemView::setFaceMode(bool on)
{
    d->faceMode = on;

    if (on)
    {
        // See ItemLister, which creates a search the implements listing tag in the ioslave

        imageAlbumModel()->setSpecialTagListing(QLatin1String("faces"));
        setItemDelegate(d->faceDelegate);

        // grouping is not very much compatible with faces

        imageFilterModel()->setAllGroupsOpen(true);

        // by default, Face View is categorized by Faces.

        imageFilterModel()->setCategorizationMode(ItemSortSettings::CategoryByFaces);

        Q_EMIT signalSeparationModeChanged((int)ItemSortSettings::CategoryByFaces);
    }
    else
    {
        imageAlbumModel()->setSpecialTagListing(QString());
        setItemDelegate(d->normalDelegate);

        bool open = ApplicationSettings::instance()->getAllGroupsOpen();
        int separationMode = ApplicationSettings::instance()->getImageSeparationMode();

        imageFilterModel()->setAllGroupsOpen(open);
        imageFilterModel()->setCategorizationMode((ItemSortSettings::CategorizationMode)separationMode);

        Q_EMIT signalSeparationModeChanged((int)separationMode);
    }
}

void DigikamItemView::addRejectionOverlay(ItemDelegate* delegate)
{
    FaceRejectionOverlay* const rejectionOverlay = new FaceRejectionOverlay(this);

    connect(rejectionOverlay, SIGNAL(rejectFaces(QList<QModelIndex>)),
            this, SLOT(removeFaces(QList<QModelIndex>)));

    addOverlay(rejectionOverlay, delegate);
}

/*
void DigikamItemView::addTagEditOverlay(ItemDelegate* delegate)
{
    TagsLineEditOverlay* tagOverlay = new TagsLineEditOverlay(this);

    connect(tagOverlay, SIGNAL(tagEdited(QModelIndex,QString)),
            this, SLOT(assignTag(QModelIndex,QString)));

    addOverlay(tagOverlay, delegate);
}
*/

void DigikamItemView::addAssignNameOverlay(ItemDelegate* delegate)
{
    AssignNameOverlay* const nameOverlay = new AssignNameOverlay(this);
    addOverlay(nameOverlay, delegate);

    connect(nameOverlay, SIGNAL(confirmFaces(QList<QModelIndex>,int)),
            this, SLOT(confirmFaces(QList<QModelIndex>,int)));

    connect(nameOverlay, SIGNAL(removeFaces(QList<QModelIndex>)),
            this, SLOT(rejectFaces(QList<QModelIndex>)));

    connect(nameOverlay, SIGNAL(unknownFaces(QList<QModelIndex>)),
            this, SLOT(unknownFaces(QList<QModelIndex>)));

    connect(nameOverlay, SIGNAL(ignoreFaces(QList<QModelIndex>)),
            this, SLOT(ignoreFaces(QList<QModelIndex>)));
}

void DigikamItemView::confirmFaces(const QList<QModelIndex>& indexes, int tagId)
{
    /**
     * You aren't allowed to "confirm" a person as
     * Ignored. Marking as Ignored is treated as a
     * changeTag() operation.
     */
    if (FaceTags::isTheIgnoredPerson(tagId))
    {
        rejectFaces(indexes);
        return;
    }

    QList<ItemInfo>      infos;
    QList<FaceTagsIface> faces;
    QList<QModelIndex>   sourceIndexes;

    // fast-remove in the "unknown person" view

    Album* const album  = currentAlbum();
    bool needFastRemove = false;

    if (album)
    {
        needFastRemove = (d->faceMode && (tagId != album->id()));
    }

    for (const QModelIndex& index : std::as_const(indexes))
    {
        if (index.isValid())
        {
            faces << d->faceDelegate->face(index);
            infos << ItemModel::retrieveItemInfo(index);

            if (needFastRemove)
            {
                sourceIndexes << imageSortFilterModel()->mapToSourceItemModel(index);
            }
        }
    }

    for (int i = 0 ; i < infos.size() ; ++i)
    {
        d->editPipeline.confirm(infos[i], faces[i], tagId);
    }

    imageAlbumModel()->removeIndexes(sourceIndexes);

    clearSelection();
}

void DigikamItemView::removeFaces(const QList<QModelIndex>& indexes)
{
    QList<ItemInfo> infos;
    QList<FaceTagsIface> faces;
    QList<QModelIndex> sourceIndexes;

    for (const QModelIndex& index : std::as_const(indexes))
    {
        if (index.isValid())
        {
            faces         << d->faceDelegate->face(index);
            infos         << ItemModel::retrieveItemInfo(index);
            sourceIndexes << imageSortFilterModel()->mapToSourceItemModel(index);
        }
    }

    for (int i = 0 ; i < infos.size() ; ++i)
    {
        d->editPipeline.remove(infos[i], faces[i]);
    }

    imageAlbumModel()->removeIndexes(sourceIndexes);

    clearSelection();
}

void DigikamItemView::unknownFaces(const QList<QModelIndex>& indexes)
{
    QList<ItemInfo> infos;
    QList<FaceTagsIface> faces;
    QList<QModelIndex> sourceIndexes;

    for (const QModelIndex& index : std::as_const(indexes))
    {
        if (index.isValid())
        {
            faces         << d->faceDelegate->face(index);
            infos         << ItemModel::retrieveItemInfo(index);
            sourceIndexes << imageSortFilterModel()->mapToSourceItemModel(index);
        }
    }

    for (int i = 0 ; i < infos.size() ; ++i)
    {
        d->editPipeline.editTag(infos[i], faces[i],
                                FaceTags::unknownPersonTagId());
    }

    imageAlbumModel()->removeIndexes(sourceIndexes);

    clearSelection();
}

void DigikamItemView::rejectFaces(const QList<QModelIndex>& indexes)
{
    QList<ItemInfo> infos;
    QList<FaceTagsIface> faces;
    QList<QModelIndex> sourceIndexes;

    for (const QModelIndex& index : std::as_const(indexes))
    {
        if (index.isValid())
        {
            faces         << d->faceDelegate->face(index);
            infos         << ItemModel::retrieveItemInfo(index);
            sourceIndexes << imageSortFilterModel()->mapToSourceItemModel(index);
        }
    }

    for (int i = 0 ; i < infos.size() ; ++i)
    {
        if      (FaceTags::isTheUnknownPerson(faces[i].tagId()))
        {
            // Reject signal was sent from an Unknown Face. Mark as Ignored.

            d->editPipeline.editTag(infos[i], faces[i], FaceTags::ignoredPersonTagId());
        }
        else if (FaceTags::isTheIgnoredPerson(faces[i].tagId()))
        {
            // Reject signal was sent from an Ignored Face. Remove face.

            d->editPipeline.remove(infos[i], faces[i]);
        }
        else
        {
            // Reject face suggestion. Mark as Unknown.

            d->editPipeline.editTag(infos[i], faces[i], FaceTags::unknownPersonTagId());
        }
    }

    imageAlbumModel()->removeIndexes(sourceIndexes);

    clearSelection();
}

void DigikamItemView::ignoreFaces(const QList<QModelIndex>& indexes)
{
    QList<ItemInfo> infos;
    QList<FaceTagsIface> faces;
    QList<QModelIndex> sourceIndexes;

    for (const QModelIndex& index : std::as_const(indexes))
    {
        if (index.isValid())
        {
            faces         << d->faceDelegate->face(index);
            infos         << ItemModel::retrieveItemInfo(index);
            sourceIndexes << imageSortFilterModel()->mapToSourceItemModel(index);
        }
    }

    for (int i = 0 ; i < infos.size() ; ++i)
    {
        d->editPipeline.editTag(infos[i], faces[i],
                                FaceTags::ignoredPersonTagId());
    }

    imageAlbumModel()->removeIndexes(sourceIndexes);

    clearSelection();
}

QList<int> DigikamItemView::getFaceIds(const QList<QModelIndex>& indexes) const
{
    QList<int> ids;

    for (const QModelIndex& index : std::as_const(indexes))
    {
        if (index.isValid())
        {
            ids << d->faceDelegate->face(index).tagId();
        }
    }

    return ids;
}

void DigikamItemView::activated(const ItemInfo& info, Qt::KeyboardModifiers modifiers)
{
    if (info.isNull())
    {
        return;
    }

    if (modifiers != Qt::AltModifier)
    {
        int leftClickAction = ApplicationSettings::instance()->getItemLeftClickAction();

        if      (leftClickAction == ApplicationSettings::ShowPreview)
        {
            Q_EMIT previewRequested(info);
        }
        else if (leftClickAction == ApplicationSettings::StartEditor)
        {
            openFile(info);
        }
        else if (leftClickAction == ApplicationSettings::ShowOnTable)
        {
            d->utilities->insertToLightTable(allItemInfos(), info, false);
        }
        else
        {
            d->utilities->openInfosWithDefaultApplication(QList<ItemInfo>() << info);
        }
    }
    else
    {
        d->utilities->openInfosWithDefaultApplication(QList<ItemInfo>() << info);
    }
}

void DigikamItemView::showContextMenuOnInfo(QContextMenuEvent* event, const ItemInfo& info)
{
    Q_EMIT signalShowContextMenuOnInfo(event, info, QList<QAction*>(), imageFilterModel());
}

void DigikamItemView::showGroupContextMenu(const QModelIndex& index, QContextMenuEvent* event)
{
    Q_UNUSED(index);

    Q_EMIT signalShowGroupContextMenu(event, selectedItemInfosCurrentFirst(), imageFilterModel());
}

void DigikamItemView::showContextMenu(QContextMenuEvent* event)
{
    Q_EMIT signalShowContextMenu(event);
}

void DigikamItemView::openFile(const ItemInfo& info)
{
    d->utilities->openInfos(info, allItemInfos(), currentAlbum());
}

void DigikamItemView::deleteSelected(const ItemViewUtilities::DeleteMode deleteMode)
{
    ItemInfoList imageInfoList = selectedItemInfos(true);

    if (d->utilities->deleteImages(imageInfoList, deleteMode))
    {
        awayFromSelection();
    }
}

void DigikamItemView::deleteSelectedDirectly(const ItemViewUtilities::DeleteMode deleteMode)
{
    ItemInfoList imageInfoList = selectedItemInfos(true);

    d->utilities->deleteImagesDirectly(imageInfoList, deleteMode);
    awayFromSelection();
}

void DigikamItemView::assignRating(const QList<QModelIndex>& indexes, int rating)
{
    ItemInfoList infos = imageInfos(indexes, MetadataOps);
    FileActionMngr::instance()->assignRating(infos, rating);
}

void DigikamItemView::groupIndicatorClicked(const QModelIndex& index)
{
    ItemInfo info = imageFilterModel()->imageInfo(index);

    if (info.isNull())
    {
        return;
    }

    setCurrentIndex(index);
    imageFilterModel()->toggleGroupOpen(info.id());
    imageAlbumModel()->ensureHasGroupedImages(info);
}

void DigikamItemView::rename()
{
    ItemInfoList infos = selectedItemInfos();

    if (needGroupResolving(RenameOps, infos))
    {
        infos = resolveGrouping(infos);
    }

    QList<QUrl> urls = infos.toImageUrlList();
    bool loop        = false;
    NewNamesList newNamesList;

    do
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Selected URLs to rename: " << urls;

        QPointer<AdvancedRenameDialog> dlg = new AdvancedRenameDialog(this);
        dlg->slotAddImages(urls);

        if (dlg->exec() != QDialog::Accepted)
        {
            delete dlg;
            break;
        }

        if (!loop)
        {
            QUrl nextUrl = nextInOrder(infos.last(), 1).fileUrl();
            setCurrentUrl(nextUrl);
            loop = true;
        }

        newNamesList = dlg->newNames();
        delete dlg;

        setFocus();
        qApp->processEvents();

        if (!newNamesList.isEmpty())
        {
            QPointer<AdvancedRenameProcessDialog> dlg2 = new AdvancedRenameProcessDialog(newNamesList, this);
            (void)dlg2->exec();

            imageFilterModel()->invalidate();
            urls = dlg2->failedUrls();
            delete dlg2;
        }
    }
    while (!urls.isEmpty() && !newNamesList.isEmpty());
}

void DigikamItemView::slotRotateLeft(const QList<QModelIndex>& indexes)
{
    ItemInfoList infos = imageInfos(indexes, MetadataOps);
    FileActionMngr::instance()->transform(infos, MetaEngineRotation::Rotate270);
}

void DigikamItemView::slotRotateRight(const QList<QModelIndex>& indexes)
{
    ItemInfoList infos = imageInfos(indexes, MetadataOps);
    FileActionMngr::instance()->transform(infos, MetaEngineRotation::Rotate90);
}

void DigikamItemView::slotFullscreen(const QList<QModelIndex>& indexes)
{
   QList<ItemInfo> infos = imageInfos(indexes, SlideshowOps);

   if (infos.isEmpty())
   {
        return;
   }

   // Just fullscreen the first.

   const ItemInfo& info = infos.at(0);

   QList<DPluginAction*> actions = DPluginLoader::instance()->
                                   pluginActions(QLatin1String("org.kde.digikam.plugin.generic.SlideShow"),
                                   DigikamApp::instance());

   if (actions.isEmpty())
   {
       return;
   }

   // Trigger SlideShow manual

   actions[0]->setData(info.fileUrl());
   actions[0]->trigger();
}

void DigikamItemView::slotInitProgressIndicator()
{
    if (!ProgressManager::instance()->findItembyId(QLatin1String("FaceActionProgress")))
    {
        FileActionProgress* const item = new FileActionProgress(QLatin1String("FaceActionProgress"));

        connect(&d->editPipeline, SIGNAL(started(QString)),
                item, SLOT(slotProgressStatus(QString)));

        connect(&d->editPipeline, SIGNAL(progressValueChanged(float)),
                item, SLOT(slotProgressValue(float)));

        connect(&d->editPipeline, SIGNAL(finished()),
                item, SLOT(slotCompleted()));
    }
}

void DigikamItemView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
    // we do not want to change the view, when in the "Thumbnails" view in "People"
    // see bugs 444692, 40232, ...

    bool runningFaceAction = (ProgressManager::instance()->findItembyId(QLatin1String("FacesDetector"))    ||
                              ProgressManager::instance()->findItembyId(QLatin1String("FaceActionProgress")));

    if ((viewMode() == QListView::IconMode) && getFaceMode() && runningFaceAction)
    {
        return;
    }

    ItemCategorizedView::scrollTo(index, hint);
}

} // namespace Digikam

#include "moc_digikamitemview.cpp"
