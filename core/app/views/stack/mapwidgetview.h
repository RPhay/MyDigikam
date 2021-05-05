/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-07-15
 * Description : central Map view
 *
 * Copyright (C) 2010      by Gabriel Voicu <ping dot gabi at gmail dot com>
 * Copyright (C) 2010      by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_MAP_WIDGET_VIEW_H
#define DIGIKAM_MAP_WIDGET_VIEW_H

// Qt includes

#include <QWidget>

// Local includes

#include "geomodelhelper.h"
#include "digikam_export.h"
#include "dcategorizedsortfilterproxymodel.h"
#include "statesavingobject.h"
#include "itemalbummodel.h"
#include "thumbnailloadthread.h"
#include "itemfiltermodel.h"
#include "camiteminfo.h"

namespace Digikam
{

class AlbumWidgetStack;
class ImageChangeset;

class DIGIKAM_GUI_EXPORT MapWidgetView : public QWidget,
                                         public StateSavingObject
{
    Q_OBJECT

public:

    enum Application
    {
        ApplicationDigikam  = 1,
        ApplicationImportUI = 2
    };

public:

    explicit MapWidgetView(QItemSelectionModel* const selectionModel,
                           DCategorizedSortFilterProxyModel* const imageFilterModel,
                           QWidget* const parent,
                           const Application application);
    ~MapWidgetView()                        override;

    void openAlbum(Album* const album);
    void setActive(const bool state);
    bool getActiveState()            const;

    ItemInfo   currentItemInfo()     const;
    CamItemInfo currentCamItemInfo() const;

protected:

    void doLoadState()                      override;
    void doSaveState()                      override;

private:

    class Private;
    const QScopedPointer<Private> d;
};

// ----------------------------------------------------------------------------------------------

class MapViewModelHelper : public GeoModelHelper
{
    Q_OBJECT

public:

    explicit MapViewModelHelper(QItemSelectionModel* const selection,
                                DCategorizedSortFilterProxyModel* const filterModel,
                                QObject* const parent,
                                const MapWidgetView::Application application);
    ~MapViewModelHelper()                                                     override;

    QAbstractItemModel* model()                                         const override;
    QItemSelectionModel* selectionModel()                               const override;
    bool itemCoordinates(const QModelIndex& index,
                         GeoCoordinates* const coordinates)             const override;

    QPixmap pixmapFromRepresentativeIndex(const QPersistentModelIndex& index,
                                          const QSize& size)                  override;
    QPersistentModelIndex bestRepresentativeIndexFromList(const QList<QPersistentModelIndex>& list,
                                                          const int sortKey)  override;

    void onIndicesClicked(const QList<QPersistentModelIndex>& clickedIndices) override;

Q_SIGNALS:

    void signalFilteredImages(const QList<qlonglong>& idList);

private Q_SLOTS:

    void slotThumbnailLoaded(const LoadingDescription&, const QPixmap&);
    void slotThumbnailLoaded(const CamItemInfo& info);
    void slotImageChange(const ImageChangeset& changeset);

private:

    class Private;
    const QScopedPointer<Private> d;
};

} // namespace Digikam

#endif // DIGIKAM_MAP_WIDGET_VIEW_H
