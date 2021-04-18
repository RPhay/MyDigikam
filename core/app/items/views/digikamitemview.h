/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-04-24
 * Description : Qt model-view for items
 *
 * Copyright (C) 2009-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DIGIKAMITEM_VIEW_H
#define DIGIKAM_DIGIKAMITEM_VIEW_H

// Local includes

#include "applicationsettings.h"
#include "itemcategorizedview.h"
#include "itemviewutilities.h"
#include "groupingviewimplementation.h"

namespace Digikam
{

class ItemViewUtilities;
class ItemInfoList;

class DigikamItemView : public ItemCategorizedView,
                        public GroupingViewImplementation
{
    Q_OBJECT

public:

    explicit DigikamItemView(QWidget* const parent = nullptr);
    ~DigikamItemView()                                                                             override;

    ItemViewUtilities* utilities()                                                           const;

    int  fitToWidthIcons();
    bool getFaceMode()                                                                       const;

    void setThumbnailSize(const ThumbnailSize& size)                                               override;

    ItemInfoList  allItemInfos(bool grouping = false)                                        const;
    ItemInfoList  selectedItemInfos(bool grouping = false)                                   const;
    ItemInfoList  selectedItemInfosCurrentFirst(bool grouping = false)                       const;
    bool          allNeedGroupResolving(const ApplicationSettings::OperationType type)       const;
    bool          selectedNeedGroupResolving(const ApplicationSettings::OperationType type)  const;

public Q_SLOTS:

    void openFile(const ItemInfo& info);

    void deleteSelected(const ItemViewUtilities::DeleteMode deleteMode = ItemViewUtilities::DeleteUseTrash);
    void deleteSelectedDirectly(const ItemViewUtilities::DeleteMode deleteMode = ItemViewUtilities::DeleteUseTrash);

    void rename();

    void assignRating(const QList<QModelIndex>& index, int rating);

    void setFaceMode(bool on);
    void confirmFaces(const QList<QModelIndex>& indexes, int tagId);

    /**
     * Removes the Face from the Database.
     * You will have to run Facial Detection again,
     * to recover the face.
     */
    void removeFaces(const QList<QModelIndex>& indexes);

    /**
     * This slot is connected to the reject signal of
     * AssignNameOverlay, and handles two cases.
     *
     * If reject is done on an Unknown Face,
     * it will mark the face as Ignored.
     *
     * If reject is done on Unconfirmed suggestions,
     * the Suggestion is rejected and the face is
     * marked as Unknown.
     */
    void rejectFaces(const QList<QModelIndex>& indexes);

    void dragDropSort(const ItemInfo& pick, const QList<ItemInfo>& infos);

Q_SIGNALS:

    void previewRequested(const ItemInfo& info);
    void signalSeparationModeChanged(int category);
    void signalShowContextMenu(QContextMenuEvent* event,
                               const QList<QAction*>& actions = QList<QAction*>());

    void signalShowContextMenuOnInfo(QContextMenuEvent* event, const ItemInfo& info,
                                     const QList<QAction*>& actions,
                                     ItemFilterModel* filterModel);

    void signalShowGroupContextMenu(QContextMenuEvent* event,
                                    const QList<ItemInfo>& selectedInfos,
                                    ItemFilterModel* filterModel);

protected Q_SLOTS:

    void groupIndicatorClicked(const QModelIndex& index);
    void showGroupContextMenu(const QModelIndex& index, QContextMenuEvent* event);

protected:

    void addRejectionOverlay(ItemDelegate* delegate = nullptr);
    void addAssignNameOverlay(ItemDelegate* delegate = nullptr);

    void activated(const ItemInfo& info, Qt::KeyboardModifiers modifiers)      override;
    void showContextMenuOnInfo(QContextMenuEvent* event, const ItemInfo& info) override;
    void showContextMenu(QContextMenuEvent* event)                             override;
    void slotSetupChanged()                                                    override;

    bool hasHiddenGroupedImages(const ItemInfo& info)                    const override;

    ItemInfoList imageInfos(const QList<QModelIndex>& indexes,
                            ApplicationSettings::OperationType type)     const;

private Q_SLOTS:

    void slotRotateLeft(const QList<QModelIndex>&);
    void slotRotateRight(const QList<QModelIndex>&);
    void slotFullscreen(const QList<QModelIndex>&);
    void slotInitProgressIndicator();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DIGIKAMITEM_VIEW_H
