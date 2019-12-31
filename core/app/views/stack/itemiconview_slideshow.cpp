/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Slideshow methods.
 *
 * Copyright (C) 2002-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2002-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "itemiconview_p.h"

namespace Digikam
{

void ItemIconView::slotSlideShowAll()
{
    slideShow(allInfo(ApplicationSettings::Slideshow));
}

void ItemIconView::slotSlideShowSelection()
{
    slideShow(selectedInfoList(ApplicationSettings::Slideshow));
}

void ItemIconView::slotSlideShowRecursive()
{
    QList<Album*> albumList = AlbumManager::instance()->currentAlbums();
    Album* album            = nullptr;

    if (!albumList.isEmpty())
    {
        album = albumList.first();
    }

    if (album)
    {
        SlideShowBuilder* const builder = new SlideShowBuilder(album);

        connect(builder, SIGNAL(signalComplete(SlideShowSettings)),
                this, SLOT(slotSlideShowBuilderComplete(SlideShowSettings)));

        builder->run();
    }
}

void ItemIconView::slotSlideShowManualFromCurrent()
{
    slotSlideShowManualFrom(currentInfo());
}

void ItemIconView::slotSlideShowManualFrom(const ItemInfo& info)
{
   SlideShowBuilder* const builder
           = new SlideShowBuilder(allInfo(ApplicationSettings::Slideshow));
   builder->setOverrideStartFrom(info);
   builder->setAutoPlayEnabled(false);

   connect(builder, SIGNAL(signalComplete(SlideShowSettings)),
           this, SLOT(slotSlideShowBuilderComplete(SlideShowSettings)));

   builder->run();
}

void ItemIconView::slideShow(const ItemInfoList& infoList)
{
    SlideShowBuilder* const builder = new SlideShowBuilder(infoList);

    connect(builder, SIGNAL(signalComplete(SlideShowSettings)),
            this, SLOT(slotSlideShowBuilderComplete(SlideShowSettings)));

    builder->run();
}

void ItemIconView::slotSlideShowBuilderComplete(const SlideShowSettings& settings)
{
    QPointer<Digikam::SlideShow> slide = new SlideShow(new DBInfoIface(this, QList<QUrl>()), settings);
    TagsActionMngr::defaultManager()->registerActionsToWidget(slide);

    if (settings.imageUrl.isValid())
    {
        slide->setCurrentItem(settings.imageUrl);
    }
    else if (settings.startWithCurrent)
    {
        slide->setCurrentItem(currentInfo().fileUrl());
    }

    connect(slide, SIGNAL(signalRatingChanged(QUrl,int)),
            this, SLOT(slotRatingChanged(QUrl,int)));

    connect(slide, SIGNAL(signalColorLabelChanged(QUrl,int)),
            this, SLOT(slotColorLabelChanged(QUrl,int)));

    connect(slide, SIGNAL(signalPickLabelChanged(QUrl,int)),
            this, SLOT(slotPickLabelChanged(QUrl,int)));

    connect(slide, SIGNAL(signalToggleTag(QUrl,int)),
            this, SLOT(slotToggleTag(QUrl,int)));

    connect(slide, SIGNAL(signalLastItemUrl(QUrl)),
            d->iconView, SLOT(setCurrentUrl(QUrl)));

    slide->show();
}

} // namespace Digikam
