/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-17
 * Description : Managing of face tag region items on a GraphicsDImgView
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "facegroup.h"

// Qt includes

#include <QGraphicsSceneHoverEvent>
#include <QTimer>

// Local includes

#include "digikam_debug.h"
#include "addtagscombobox.h"
#include "albummodel.h"
#include "albumfiltermodel.h"
#include "albummanager.h"
#include "clickdragreleaseitem.h"
#include "dimgpreviewitem.h"
#include "facepipeline.h"
#include "faceitem.h"
#include "facetags.h"
#include "faceutils.h"
#include "facetagseditor.h"
#include "graphicsdimgview.h"
#include "iteminfo.h"
#include "taggingaction.h"
#include "metaenginesettings.h"
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

    explicit Private(FaceGroup* const qq);

    void                       applyVisible();
    FaceItem*                  createItem(const FaceTagsIface& face);
    FaceItem*                  addItem(const FaceTagsIface& face);
    AssignNameWidget*          createAssignNameWidget(const FaceTagsIface& face,
                                                      const QVariant& identifier);
    AssignNameWidget::Mode     assignWidgetMode(FaceTagsIface::Type type);
    void                       checkModels();
    QList<QGraphicsItem*>      hotItems(const QPointF& scenePos);

public:

    GraphicsDImgView*          view                     = nullptr;
    ItemInfo                   info;
    bool                       exifRotate               = true;
    bool                       autoSuggest              = false;
    bool                       showOnHover              = false;

    QList<FaceItem*>           items;

    ClickDragReleaseItem*      manuallyAddWrapItem      = nullptr;
    FaceItem*                  manuallyAddedItem        = nullptr;

    FaceGroupState             state                    = NoFaces;
    ItemVisibilityController*  visibilityController     = nullptr;

    TagModel*                  tagModel                 = nullptr;
    CheckableAlbumFilterModel* filterModel              = nullptr;
    TagPropertiesFilterModel*  filteredModel            = nullptr;

    FacePipeline               editPipeline;

    FaceGroup* const           q                        = nullptr;
};

} // namespace Digikam
