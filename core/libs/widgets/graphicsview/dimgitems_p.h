/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-30
 * Description : Graphics View item for DImg - Private containers
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DIMG_ITEMS_P_H
#define DIGIKAM_DIMG_ITEMS_P_H

// Qt includes

#include <QPixmapCache>
#include <QQueue>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "dimg.h"
#include "dimgpreviewitem.h"
#include "imagezoomsettings.h"
#include "previewsettings.h"

namespace Digikam
{

class CachedPixmapKey
{
public:

    QRect             region;
    QPixmapCache::Key key;
};

// -------------------------------------------------------------------------------

class CachedPixmaps
{
public:

    explicit CachedPixmaps(int maxCount = 2);
    ~CachedPixmaps();

    void setMaxCount(int);
    void clear();
    bool find(const QRect& region, QPixmap* const pix, QRect* const source);
    void insert(const QRect& region, const QPixmap& pixmap);

protected:

    int                     maxCount;
    QQueue<CachedPixmapKey> keys;
};

// -------------------------------------------------------------------------------

class DIGIKAM_EXPORT GraphicsDImgItem::GraphicsDImgItemPrivate
{
public:

    explicit GraphicsDImgItemPrivate()
    {
    }

    void init(GraphicsDImgItem* const q);

public:

    DImg                  image;
    ImageZoomSettings     zoomSettings;
    mutable CachedPixmaps cachedPixmaps;
};

// -------------------------------------------------------------------------------

class PreviewLoadThread;
class DImgPreviewItem;

class DIGIKAM_EXPORT DImgPreviewItem::DImgPreviewItemPrivate : public GraphicsDImgItem::GraphicsDImgItemPrivate
{
public:

    explicit DImgPreviewItemPrivate();
    void init(DImgPreviewItem* const q);

public:

    DImgPreviewItem::State state;
    bool                   exifRotate;
    int                    previewSize;

    QString                path;
    PreviewSettings        previewSettings;
    PreviewLoadThread*     previewThread;
    PreviewLoadThread*     preloadThread;
    QStringList            pathsToPreload;
};

} // namespace Digikam

#endif // DIGIKAM_DIMG_ITEMS_P_H
