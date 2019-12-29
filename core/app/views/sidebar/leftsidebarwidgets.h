/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : left sidebar widgets
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#ifndef DIGIKAM_LEFT_SIDE_BAR_WIDGETS_H
#define DIGIKAM_LEFT_SIDE_BAR_WIDGETS_H

// Local includes

#include "digikam_config.h"
#include "albummodel.h"
#include "albummodificationhelper.h"
#include "itemalbumfiltermodel.h"
#include "searchmodificationhelper.h"
#include "sidebarwidget.h"
#include "itemfiltermodel.h"
#include "labelstreeview.h"

#ifdef HAVE_MARBLE
#   include "gpssearchview.h"
#endif // HAVE_MARBLE

namespace Digikam
{

template <class T>
class AlbumPointer;

/**
 * SideBarWidget for the tag view.
 */
class TagViewSideBarWidget : public SidebarWidget
{

    Q_OBJECT

public:

    explicit TagViewSideBarWidget(QWidget* const parent, TagModel* const model);
    virtual ~TagViewSideBarWidget();

    void          setActive(bool active)                             override;
    void          doLoadState()                                      override;
    void          doSaveState()                                      override;
    void          applySettings()                                    override;
    void          changeAlbumFromHistory(const QList<Album*>& album) override;
    const QIcon   getIcon()                                          override;
    const QString getCaption()                                       override;

    AlbumPointer<TAlbum> currentAlbum() const;

private:

    void setNoTagsAlbum();

public Q_SLOTS:

    void setCurrentAlbum(TAlbum* album);
    void slotOpenTagManager();
    void slotToggleTagsSelection(int radioClicked);

Q_SIGNALS:

    void signalFindDuplicates(const QList<TAlbum*>& albums);

public:

    // Declared as public due to use by Private
    class Private;

private:

    Private* const d;
};

// -----------------------------------------------------------------------------------------

/**
 * SideBarWidget for the Labels.
 */
class LabelsSideBarWidget : public SidebarWidget
{
    Q_OBJECT

public:

    explicit LabelsSideBarWidget(QWidget* const parent);
    virtual ~LabelsSideBarWidget();

    LabelsTreeView* labelsTree();

    void          setActive(bool active)                             override;
    void          applySettings()                                    override;
    void          changeAlbumFromHistory(const QList<Album*>& album) override;
    void          doLoadState()                                      override;
    void          doSaveState()                                      override;
    const QIcon   getIcon()                                          override;
    const QString getCaption()                                       override;

    QHash<LabelsTreeView::Labels, QList<int> > selectedLabels();

private:

    class Private;
    Private* const d;
};

// -----------------------------------------------------------------------------------------

/**
 * SideBarWidget for the fuzzy search.
 */
class FuzzySearchSideBarWidget : public SidebarWidget
{
    Q_OBJECT

public:

    explicit FuzzySearchSideBarWidget(QWidget* const parent,
                                      SearchModel* const searchModel,
                                      SearchModificationHelper* const searchModificationHelper);
    virtual ~FuzzySearchSideBarWidget();

    void          setActive(bool active)                             override;
    void          doLoadState()                                      override;
    void          doSaveState()                                      override;
    void          applySettings()                                    override;
    void          changeAlbumFromHistory(const QList<Album*>& album) override;
    const QIcon   getIcon()                                          override;
    const QString getCaption()                                       override;

    void newDuplicatesSearch(PAlbum* album);
    void newDuplicatesSearch(const QList<PAlbum*>& albums);
    void newDuplicatesSearch(const QList<TAlbum*>& albums);
    void newSimilarSearch(const ItemInfo& imageInfo);

Q_SIGNALS:

    void signalActive(bool);

private:

    class Private;
    Private* const d;
};

// -----------------------------------------------------------------------------------------

#ifdef HAVE_MARBLE

/**
 * SideBarWidget for the gps search.
 */
class GPSSearchSideBarWidget : public SidebarWidget
{
    Q_OBJECT

public:

    explicit GPSSearchSideBarWidget(QWidget* const parent,
                                    SearchModel* const searchModel,
                                    SearchModificationHelper* const searchModificationHelper,
                                    ItemFilterModel* const imageFilterModel,
                                    QItemSelectionModel* const itemSelectionModel);
    virtual ~GPSSearchSideBarWidget();

    void          setActive(bool active)                             override;
    void          doLoadState()                                      override;
    void          doSaveState()                                      override;
    void          applySettings()                                    override;
    void          changeAlbumFromHistory(const QList<Album*>& album) override;
    const QIcon   getIcon()                                          override;
    const QString getCaption()                                       override;

Q_SIGNALS:

    void signalMapSoloItems(const QList<qlonglong>&, const QString&);

private:

    class Private;
    Private* const d;
};

#endif // HAVE_MARBLE

// -----------------------------------------------------------------------------------------

/**
 * SideBarWidget for People
 */
class PeopleSideBarWidget : public SidebarWidget
{
    Q_OBJECT

public:

    explicit PeopleSideBarWidget(QWidget* const parent,
                                 TagModel* const tagModel,
                                 SearchModificationHelper* const searchModificationHelper);
    virtual ~PeopleSideBarWidget();

    void          setActive(bool active)                             override;
    void          doLoadState()                                      override;
    void          doSaveState()                                      override;
    void          applySettings()                                    override;
    void          changeAlbumFromHistory(const QList<Album*>& album) override;
    const QIcon   getIcon()                                          override;
    const QString getCaption()                                       override;

private Q_SLOTS:

    void slotInit();
    void slotScanForFaces();
    void slotScanComplete();

Q_SIGNALS:

    void requestFaceMode(bool on);

    void signalFindDuplicates(const QList<TAlbum*>& albums);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_LEFT_SIDE_BAR_WIDGETS_H
