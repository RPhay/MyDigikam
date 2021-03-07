/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : factory of basic models used for views in digikam
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010      by Andi Clemens <andi dot clemens at gmail dot com>
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

#include "dmodelfactory.h"

// Qt includes

#include <QIcon>

// Local settings

#include "applicationsettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN DModelFactory::Private
{

public:

    explicit Private()
     :  albumModel       (nullptr),
        tagModel         (nullptr),
        tagFilterModel   (nullptr),
        tagFacesModel    (nullptr),
        searchModel      (nullptr),
        dateAlbumModel   (nullptr),
        imageVersionModel(nullptr)
    {
    }

    AlbumModel*        albumModel;
    TagModel*          tagModel;
    TagModel*          tagFilterModel;
    TagModel*          tagFacesModel;
    SearchModel*       searchModel;
    DateAlbumModel*    dateAlbumModel;
    ItemVersionsModel* imageVersionModel;
};

DModelFactory::DModelFactory()
    : d(new Private)
{
    d->tagModel          = new TagModel(AbstractAlbumModel::IncludeRootAlbum);
    d->tagFacesModel     = new TagModel(AbstractAlbumModel::IgnoreRootAlbum);
    d->tagFacesModel->setTagCount(TagModel::FaceTagCount);
    d->tagFilterModel    = new TagModel(AbstractAlbumModel::IgnoreRootAlbum);
    d->tagFilterModel->setAddExcludeTristate(true);

    d->albumModel        = new AlbumModel(AlbumModel::IncludeRootAlbum);
    d->searchModel       = new SearchModel();
    d->dateAlbumModel    = new DateAlbumModel();
    d->imageVersionModel = new ItemVersionsModel();

    // set icons initially

    slotApplicationSettingsChanged();

    connect(ApplicationSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotApplicationSettingsChanged()));
}

DModelFactory::~DModelFactory()
{
    delete d->tagModel;
    delete d->tagFacesModel;
    delete d->tagFilterModel;

    delete d->albumModel;
    delete d->searchModel;
    delete d->dateAlbumModel;
    delete d->imageVersionModel;

    delete d;
}

TagModel* DModelFactory::getTagModel() const
{
    return d->tagModel;
}

TagModel* DModelFactory::getTagFacesModel() const
{
    return d->tagFacesModel;
}

TagModel* DModelFactory::getTagFilterModel() const
{
    return d->tagFilterModel;
}

AlbumModel* DModelFactory::getAlbumModel() const
{
    return d->albumModel;
}

SearchModel* DModelFactory::getSearchModel() const
{
    return d->searchModel;
}

DateAlbumModel* DModelFactory::getDateAlbumModel() const
{
    return d->dateAlbumModel;
}

ItemVersionsModel* DModelFactory::getItemVersionsModel() const
{
    return d->imageVersionModel;
}

void DModelFactory::slotApplicationSettingsChanged()
{
    int size = ApplicationSettings::instance()->getTreeViewIconSize();
    d->dateAlbumModel->setPixmaps(QIcon::fromTheme(QLatin1String("view-calendar-list")).pixmap(size),
                                  QIcon::fromTheme(QLatin1String("view-calendar")).pixmap(size));
}

} // namespace Digikam
