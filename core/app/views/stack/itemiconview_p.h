/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : implementation of item icon view interface.
 *
 * Copyright (C) 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2011-2013 by Michael G. Hansen <mike at mghansen dot de>
 * Copyright (C) 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
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

#ifndef DIGIKAM_ITEM_ICON_VIEW_P_H
#define DIGIKAM_ITEM_ICON_VIEW_P_H

#include "itemiconview.h"

// Qt includes

#include <QTimer>
#include <QShortcut>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "albumhistory.h"
#include "labelstreeview.h"
#include "albumpointer.h"
#include "coredbsearchxml.h"
#include "dbinfoiface.h"
#include "digikamapp.h"
#include "digikamitemview.h"
#include "dfileoperations.h"
#include "dmessagebox.h"
#include "dzoombar.h"
#include "dtrashitemmodel.h"
#include "facescansettings.h"
#include "facesdetector.h"
#include "fileactionmngr.h"
#include "fileactionprogress.h"
#include "filtersidebarwidget.h"
#include "filterstatusbar.h"
#include "itemalbummodel.h"
#include "itemdescedittab.h"
#include "itempreviewview.h"
#include "itempropertiessidebardb.h"
#include "itempropertiesversionstab.h"
#include "itemthumbnailbar.h"
#include "itemviewutilities.h"
#include "albumfolderviewsidebarwidget.h"
#include "datefolderviewsidebarwidget.h"
#include "timelinesidebarwidget.h"
#include "searchsidebarwidget.h"
#include "fuzzysearchsidebarwidget.h"
#include "labelssidebarwidget.h"
#include "peoplesidebarwidget.h"
#include "tagviewsidebarwidget.h"
#include "loadingcacheinterface.h"
#include "metadatahub.h"
#include "metaenginesettings.h"
#include "metadatasynchronizer.h"
#include "newitemsfinder.h"
#include "queuemgrwindow.h"
#include "scancontroller.h"
#include "setup.h"
#include "sidebar.h"
#include "statusprogressbar.h"
#include "tableview.h"
#include "tagmodificationhelper.h"
#include "tagsactionmngr.h"
#include "tagscache.h"
#include "tagsmanager.h"
#include "thumbsgenerator.h"
#include "trashview.h"
#include "versionmanagersettings.h"
#include "contextmenuhelper.h"
#include "albumlabelssearchhandler.h"
#include "dnotificationwidget.h"
#include "coredb.h"
#include "coredbaccess.h"

#ifdef HAVE_MEDIAPLAYER
#   include "mediaplayerview.h"
#endif //HAVE_MEDIAPLAYER

#ifdef HAVE_MARBLE
#include "gpssearchsidebarwidget.h"
#   include "mapwidgetview.h"
#endif // HAVE_MARBLE

namespace Digikam
{

class Q_DECL_HIDDEN ItemIconView::Private
{
public:

    explicit Private()
      : needDispatchSelection   (false),
        useAlbumHistory         (false),
        initialAlbumID          (0),
        thumbSize               (ThumbnailSize::Medium),
        dockArea                (nullptr),
        splitter                (nullptr),
        selectionTimer          (nullptr),
        thumbSizeTimer          (nullptr),
        albumFolderSideBar      (nullptr),
        tagViewSideBar          (nullptr),
        labelsSideBar           (nullptr),
        dateViewSideBar         (nullptr),
        timelineSideBar         (nullptr),
        searchSideBar           (nullptr),
        fuzzySearchSideBar      (nullptr),

#ifdef HAVE_MARBLE

        gpsSearchSideBar        (nullptr),
        mapView                 (nullptr),

#endif // HAVE_MARBLE

        peopleSideBar           (nullptr),
        parent                  (nullptr),
        iconView                (nullptr),
        tableView               (nullptr),
        trashView               (nullptr),
        utilities               (nullptr),
        albumManager            (nullptr),
        albumHistory            (nullptr),
        stackedview             (nullptr),
        lastViewMode            (StackedView::IconViewMode),
        albumModificationHelper (nullptr),
        tagModificationHelper   (nullptr),
        searchModificationHelper(nullptr),
        leftSideBar             (nullptr),
        rightSideBar            (nullptr),
        filterWidget            (nullptr),
        optionAlbumViewPrefix   (QLatin1String("AlbumView")),
        modelCollection         (nullptr),
        labelsSearchHandler     (nullptr),
        errorWidget             (nullptr)
    {
    }

public:

    QString userPresentableAlbumTitle(const QString& title) const
    {
        if      (title == SAlbum::getTemporaryHaarTitle(DatabaseSearch::HaarSketchSearch))
        {
            return i18n("Fuzzy Sketch Search");
        }
        else if (title == SAlbum::getTemporaryHaarTitle(DatabaseSearch::HaarImageSearch))
        {
            return i18n("Fuzzy Image Search");
        }
        else if (title == SAlbum::getTemporaryTitle(DatabaseSearch::MapSearch))
        {
            return i18n("Map Search");
        }
        else if ((title == SAlbum::getTemporaryTitle(DatabaseSearch::AdvancedSearch)) ||
                 (title == SAlbum::getTemporaryTitle(DatabaseSearch::KeywordSearch)))
        {
            return i18n("Last Search");
        }
        else if (title == SAlbum::getTemporaryTitle(DatabaseSearch::TimeLineSearch))
        {
            return i18n("Timeline");
        }

        return title;
    }

    void addPageUpDownActions(ItemIconView* const q, QWidget* const w)
    {
        defineShortcut(w, Qt::Key_PageDown, q, SLOT(slotNextItem()));
        defineShortcut(w, Qt::Key_Down,     q, SLOT(slotNextItem()));
        defineShortcut(w, Qt::Key_Right,    q, SLOT(slotNextItem()));

        defineShortcut(w, Qt::Key_PageUp,   q, SLOT(slotPrevItem()));
        defineShortcut(w, Qt::Key_Up,       q, SLOT(slotPrevItem()));
        defineShortcut(w, Qt::Key_Left,     q, SLOT(slotPrevItem()));
    }

public:

    bool                          needDispatchSelection;
    bool                          useAlbumHistory;

    int                           initialAlbumID;
    int                           thumbSize;

    QMainWindow*                  dockArea;

    SidebarSplitter*              splitter;

    QTimer*                       selectionTimer;
    QTimer*                       thumbSizeTimer;

    // left side bar

    AlbumFolderViewSideBarWidget* albumFolderSideBar;
    TagViewSideBarWidget*         tagViewSideBar;
    LabelsSideBarWidget*          labelsSideBar;
    DateFolderViewSideBarWidget*  dateViewSideBar;
    TimelineSideBarWidget*        timelineSideBar;
    SearchSideBarWidget*          searchSideBar;
    FuzzySearchSideBarWidget*     fuzzySearchSideBar;

#ifdef HAVE_MARBLE

    GPSSearchSideBarWidget*       gpsSearchSideBar;
    MapWidgetView*                mapView;

#endif // HAVE_MARBLE

    PeopleSideBarWidget*          peopleSideBar;
    DigikamApp*                   parent;
    DigikamItemView*              iconView;
    TableView*                    tableView;
    TrashView*                    trashView;
    ItemViewUtilities*            utilities;
    AlbumManager*                 albumManager;
    AlbumHistory*                 albumHistory;
    StackedView*                  stackedview;
    StackedView::StackedViewMode  lastViewMode;

    AlbumModificationHelper*      albumModificationHelper;
    TagModificationHelper*        tagModificationHelper;
    SearchModificationHelper*     searchModificationHelper;

    Sidebar*                      leftSideBar;
    ItemPropertiesSideBarDB*      rightSideBar;

    FilterSideBarWidget*          filterWidget;

    QString                       optionAlbumViewPrefix;

    QList<SidebarWidget*>         leftSideBarWidgets;

    DModelFactory*                modelCollection;
    AlbumLabelsSearchHandler*     labelsSearchHandler;
    DNotificationWidget*          errorWidget;
};

} // namespace Digikam

#endif // DIGIKAM_ITEM_ICON_VIEW_P_H
