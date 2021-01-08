/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-17
 * Description : Managing of face tag region items on a GraphicsDImgView
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#include "facegroup_p.h"

namespace Digikam
{

FaceGroup::Private::Private(FaceGroup* const q)
    : view                  (nullptr),
      exifRotate            (true),
      autoSuggest           (false),
      showOnHover           (false),
      manuallyAddWrapItem   (nullptr),
      manuallyAddedItem     (nullptr),
      state                 (NoFaces),
      visibilityController  (nullptr),
      tagModel              (nullptr),
      filterModel           (nullptr),
      filteredModel         (nullptr),
      q                     (q)
{
}

QList<QGraphicsItem*> FaceGroup::Private::hotItems(const QPointF& scenePos)
{
    if (!q->hasVisibleItems())
    {
        return QList<QGraphicsItem*>();
    }

    const int distance               = 15;
    QRectF hotSceneRect              = QRectF(scenePos, QSize(0, 0)).adjusted(-distance, -distance, distance, distance);
    QList<QGraphicsItem*> closeItems = view->scene()->items(hotSceneRect, Qt::IntersectsItemBoundingRect);

    closeItems.removeOne(view->previewItem());

    return closeItems;

/*
    qreal distance;
    d->faceGroup->closestItem(mapToScene(e->pos()), &distance);

    if (distance < 15)
    {
        return false;
    }
*/
}

void FaceGroup::Private::applyVisible()
{
    if      (state == NoFaces)
    {
        // If not yet loaded, load. load() will transitionToVisible after loading.

        q->load();
    }
    else if (state == FacesLoaded)
    {
        // show existing faces, if we have an image

        if (view->previewItem()->isLoaded())
        {
            visibilityController->show();
        }
    }
}

FaceItem* FaceGroup::Private::createItem(const FaceTagsIface& face)
{
    FaceItem* const item = new FaceItem(view->previewItem());
    item->setFace(face);

    QRect faceRect       = face.region().toRect();

    if (!exifRotate)
    {
        TagRegion::reverseToOrientation(faceRect,
                                        info.orientation(),
                                        info.dimensions());
    }

    item->setOriginalRect(faceRect);
    item->setVisible(false);

    return item;
}

FaceItem* FaceGroup::Private::addItem(const FaceTagsIface& face)
{
    FaceItem* const item                 = createItem(face);

    // for identification, use index in our list

    QList<QVariant> identifier(face.toVariant().toList());
    identifier << items.size();

    AssignNameWidget* const assignWidget = createAssignNameWidget(face, identifier);
    item->setHudWidget(assignWidget);
/*
    new StyleSheetDebugger(assignWidget);
*/
    visibilityController->addItem(item);

    items << item;

    return item;
}

void FaceGroup::Private::checkModels()
{
    if (!tagModel)
    {
        tagModel = new TagModel(AbstractAlbumModel::IgnoreRootAlbum, q);
    }

    if (!filterModel)
    {
        filterModel = new CheckableAlbumFilterModel(q);
    }

    if (!filteredModel)
    {
        filteredModel = new TagPropertiesFilterModel(q);
    }
}

AssignNameWidget::Mode FaceGroup::Private::assignWidgetMode(FaceTagsIface::Type type)
{
    switch (type)
    {
        case FaceTagsIface::UnknownName:
        case FaceTagsIface::UnconfirmedName:
        {
            return AssignNameWidget::UnconfirmedEditMode;
        }

        case FaceTagsIface::Type::IgnoredName:
        {
            return AssignNameWidget::IgnoredMode;
        }

        case FaceTagsIface::ConfirmedName:
        {
            return AssignNameWidget::ConfirmedMode;
        }

        default:
        {
            return AssignNameWidget::InvalidMode;
        }
    }
}

AssignNameWidget* FaceGroup::Private::createAssignNameWidget(const FaceTagsIface& face, const QVariant& identifier)
{
    AssignNameWidget* const assignWidget = new AssignNameWidget(view);
    assignWidget->setMode(assignWidgetMode(face.type()));
    assignWidget->setTagEntryWidgetMode(AssignNameWidget::AddTagsComboBoxMode);
    assignWidget->setVisualStyle(AssignNameWidget::TranslucentDarkRound);
    assignWidget->setLayoutMode(AssignNameWidget::TwoLines);
    assignWidget->setUserData(info, identifier);
    checkModels();
    assignWidget->setModel(tagModel, filteredModel, filterModel);
    assignWidget->setParentTag(AlbumManager::instance()->findTAlbum(FaceTags::personParentTag()));

    q->connect(assignWidget, SIGNAL(assigned(TaggingAction,ItemInfo,QVariant)),
               q, SLOT(slotAssigned(TaggingAction,ItemInfo,QVariant)));

    q->connect(assignWidget, SIGNAL(rejected(ItemInfo,QVariant)),
               q, SLOT(slotRejected(ItemInfo,QVariant)));

    q->connect(assignWidget, SIGNAL(labelClicked(ItemInfo,QVariant)),
               q, SLOT(slotLabelClicked(ItemInfo,QVariant)));

    return assignWidget;
}

} // namespace Digikam
