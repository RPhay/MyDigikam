/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Managing of focus point items on a GraphicsDImgView
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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


#ifndef DIGIKAM_FOCUSPOINT_GROUP_H
#define DIGIKAM_FOCUSPOINT_GROUP_H

// Qt includes

#include <QObject>
#include <QPointF>
#include <QGraphicsWidget>
#include <QGraphicsSceneHoverEvent>

// Local inclues

#include "assignnamewidget.h"
#include "regionframeitem.h"

#include "addtagscombobox.h"
#include "albummodel.h"
#include "albumfiltermodel.h"
#include "albummanager.h"
#include "clickdragreleaseitem.h"
#include "dimgpreviewitem.h"
#include "graphicsdimgview.h"
#include "iteminfo.h"
#include "taggingaction.h"
#include "metadatahub.h"
#include "tagregion.h"
#include "digikam_debug.h"
#include "itemvisibilitycontroller.h"

class QGraphicsSceneHoverEvent;

namespace Digikam
{

class Album;
class ItemInfo;
class GraphicsDImgView;
class RegionFrameItem;
class TaggingAction;

class FocusPointGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible)

public:

    explicit FocusPointGroup(GraphicsDImgView* const view);
    ~FocusPointGroup() override;

    bool isVisible()                                                     const;
    ItemInfo info()                                                      const;
    bool hasVisibleItems()                                               const;

    QList<RegionFrameItem*> items()                                      const;

    void leaveEvent(QEvent*);

public:
    bool isAllowedToAddFocusPoint()                                      const;

public Q_SLOTS:

    /**
     * Shows or hides the frames
     */
    void setVisible(bool visible);
    void setVisibleItem(RegionFrameItem* item);

    /**
     * Sets the current ItemInfo
     */
    void setInfo(const ItemInfo& info);

    void addPoint();

    void aboutToSetInfoAfterRotate(const ItemInfo& info);

protected:

    void load();
    void clear();
    // void applyItemGeometryChanges();

protected Q_SLOTS:

    void itemStateChanged(int);
    void slotAlbumsUpdated(int type);

    void slotAddItemStarted(const QPointF& pos);
    void slotAddItemMoving(const QRectF& rect);
    void slotAddItemFinished(const QRectF& rect);
    void cancelAddItem();

private:

    // Disable
    FocusPointGroup()                                   = delete;
    FocusPointGroup(const FocusPointGroup&)             = delete;
    FocusPointGroup& operator=(const FocusPointGroup&)  = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FOCUSPOINT_GROUP_H
