/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-07-08
 * Description : Qt item view to import items - the delegate (private container)
 *
 * SPDX-FileCopyrightText: 2012      by Islam Wazery <wazery at ubuntu dot com>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMPORT_DELEGATE_P_H
#define DIGIKAM_IMPORT_DELEGATE_P_H

#include "importdelegate.h"

// Qt includes

#include <QRect>
#include <QCache>

// Local includes

#include "importcategorizedview.h"
#include "itemviewimportdelegate_p.h"

namespace Digikam
{

class ImportCategoryDrawer;

class Q_DECL_HIDDEN ImportDelegate::ImportDelegatePrivate : public ItemViewImportDelegatePrivate
{
public:

    explicit ImportDelegatePrivate()
      : contentWidth        (0),
        drawImageFormat     (false),
        drawCoordinates     (false),
        drawFocusFrame      (true),
        drawMouseOverFrame  (true),
        ratingOverThumbnail (false),
        categoryDrawer      (nullptr),
        currentView         (nullptr),
        currentModel        (nullptr)
    {
        actualPixmapRectCache.setMaxCost(250);
    }

    int                    contentWidth;

    QRect                  dateRect;
    QRect                  pixmapRect;
    QRect                  nameRect;
/*
    QRect                  titleRect;
    QRect                  commentsRect;
*/
    QRect                  resolutionRect;
    QRect                  sizeRect;
    QRect                  downloadRect;
    QRect                  lockRect;
    QRect                  coordinatesRect;
    QRect                  tagRect;
    QRect                  imageInformationRect;
    QRect                  pickLabelRect;
    QRect                  groupRect;

    bool                   drawImageFormat;
    bool                   drawCoordinates;
    bool                   drawFocusFrame;
    bool                   drawMouseOverFrame;
    bool                   ratingOverThumbnail;

    QCache<int, QRect>     actualPixmapRectCache;
    ImportCategoryDrawer*  categoryDrawer;

    ImportCategorizedView* currentView;
    QAbstractItemModel*    currentModel;

public:

    void clearRects() override;
};

// --- ImportThumbnailDelegate ----------------------------------------------------

class Q_DECL_HIDDEN ImportThumbnailDelegatePrivate : public ImportDelegate::ImportDelegatePrivate
{
public:

    explicit ImportThumbnailDelegatePrivate()
       : flow(QListView::LeftToRight)
    {
        drawFocusFrame      = false;       // switch off drawing of frames
        drawMouseOverFrame  = false;
        ratingOverThumbnail = true;        // switch off composing rating over background
    }

    void init(ImportThumbnailDelegate* const q);

public:

    QListView::Flow flow;
    QRect           viewSize;
};

// --- ImportNormalDelegate ----------------------------------------------------

class Q_DECL_HIDDEN ImportNormalDelegatePrivate : public ImportDelegate::ImportDelegatePrivate
{
public:

    explicit ImportNormalDelegatePrivate() {};
    ~ImportNormalDelegatePrivate() override;

    void init(ImportNormalDelegate* const q, ImportCategorizedView* const parent);
};

} // namespace Digikam

#endif // DIGIKAM_IMPORT_DELEGATE_P_H
