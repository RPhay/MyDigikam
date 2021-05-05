/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-17
 * Description : Managing of face tag region items on a GraphicsDImgView
 *
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

#ifndef DIGIKAM_FACE_GROUP_P_H
#define DIGIKAM_FACE_GROUP_P_H

#include "facegroup.h"

// Qt includes

#include <QGraphicsSceneHoverEvent>

// Local includes

#include "addtagscombobox.h"
#include "albummodel.h"
#include "albumfiltermodel.h"
#include "albummanager.h"
#include "clickdragreleaseitem.h"
#include "dimgpreviewitem.h"
#include "facepipeline.h"
#include "faceitem.h"
#include "facetags.h"
#include "facetagseditor.h"
#include "graphicsdimgview.h"
#include "iteminfo.h"
#include "metadatahub.h"
#include "taggingaction.h"
#include "digikam_debug.h"
#include "itemvisibilitycontroller.h"

namespace Digikam
{

enum FaceGroupState
{
    NoFaces,
    LoadingFaces,
    FacesLoaded
};

//-------------------------------------------------------------------------------

class Q_DECL_HIDDEN FaceGroup::Private
{
public:

    explicit Private(FaceGroup* const q);

    void                       applyVisible();
    FaceItem*                  createItem(const FaceTagsIface& face);
    FaceItem*                  addItem(const FaceTagsIface& face);
    AssignNameWidget*          createAssignNameWidget(const FaceTagsIface& face,
                                                      const QVariant& identifier);
    AssignNameWidget::Mode     assignWidgetMode(FaceTagsIface::Type type);
    void                       checkModels();
    QList<QGraphicsItem*>      hotItems(const QPointF& scenePos);

public:

    GraphicsDImgView*          view;
    ItemInfo                   info;
    bool                       exifRotate;
    bool                       autoSuggest;
    bool                       showOnHover;

    QList<FaceItem*>           items;

    ClickDragReleaseItem*      manuallyAddWrapItem;
    FaceItem*                  manuallyAddedItem;

    FaceGroupState             state;
    ItemVisibilityController*  visibilityController;

    TagModel*                  tagModel;
    CheckableAlbumFilterModel* filterModel;
    TagPropertiesFilterModel*  filteredModel;

    FacePipeline               editPipeline;

    FaceGroup* const           q;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_GROUP_P_H
