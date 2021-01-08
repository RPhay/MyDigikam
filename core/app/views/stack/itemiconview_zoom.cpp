/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-16-10
 * Description : Item icon view interface - Zoom methods.
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

#include "itemiconview_p.h"

namespace Digikam
{

double ItemIconView::zoomMin() const
{
    return d->stackedview->zoomMin();
}

double ItemIconView::zoomMax() const
{
    return d->stackedview->zoomMax();
}

void ItemIconView::setZoomFactor(double zoom)
{
    d->stackedview->setZoomFactorSnapped(zoom);
}

void ItemIconView::slotZoomFactorChanged(double zoom)
{
    toggleZoomActions();

    emit signalZoomChanged(zoom);
}

void ItemIconView::toggleZoomActions()
{
    if      (viewMode() == StackedView::PreviewImageMode)
    {
        d->parent->enableZoomMinusAction(true);
        d->parent->enableZoomPlusAction(true);

        if (d->stackedview->maxZoom())
        {
            d->parent->enableZoomPlusAction(false);
        }

        if (d->stackedview->minZoom())
        {
            d->parent->enableZoomMinusAction(false);
        }
    }
    else if ((viewMode() == StackedView::IconViewMode) ||
             (viewMode() == StackedView::TableViewMode))
    {
        d->parent->enableZoomMinusAction(true);
        d->parent->enableZoomPlusAction(true);

        if (d->thumbSize >= ThumbnailSize::maxThumbsSize())
        {
            d->parent->enableZoomPlusAction(false);
        }

        if (d->thumbSize <= ThumbnailSize::Small)
        {
            d->parent->enableZoomMinusAction(false);
        }
    }
    else
    {
        d->parent->enableZoomMinusAction(false);
        d->parent->enableZoomPlusAction(false);
    }
}

void ItemIconView::slotZoomIn()
{
    if      ((viewMode() == StackedView::IconViewMode) ||
             (viewMode() == StackedView::TableViewMode))
    {
        setThumbSize(d->thumbSize + ThumbnailSize::Step);
        toggleZoomActions();
        emit signalThumbSizeChanged(d->thumbSize);
    }
    else if (viewMode() == StackedView::PreviewImageMode)
    {
        d->stackedview->increaseZoom();
    }
}

void ItemIconView::slotZoomOut()
{
    if      ((viewMode() == StackedView::IconViewMode) ||
             (viewMode() == StackedView::TableViewMode))
    {
        setThumbSize(d->thumbSize - ThumbnailSize::Step);
        toggleZoomActions();
        emit signalThumbSizeChanged(d->thumbSize);
    }
    else if (viewMode() == StackedView::PreviewImageMode)
    {
        d->stackedview->decreaseZoom();
    }
}

void ItemIconView::slotZoomTo100Percents()
{
    if (viewMode() == StackedView::PreviewImageMode)
    {
        d->stackedview->toggleFitToWindowOr100();
    }
}

void ItemIconView::slotFitToWindow()
{
    if      (viewMode() == StackedView::TableViewMode)
    {
        /// @todo We should choose an appropriate thumbnail size here
    }
    else if (viewMode() == StackedView::IconViewMode)
    {
        int nts = d->iconView->fitToWidthIcons();
        qCDebug(DIGIKAM_GENERAL_LOG) << "new thumb size = " << nts;
        setThumbSize(nts);
        toggleZoomActions();
        emit signalThumbSizeChanged(d->thumbSize);
    }
    else if (viewMode() == StackedView::PreviewImageMode)
    {
        d->stackedview->fitToWindow();
    }
}

void ItemIconView::setThumbSize(int size)
{
    if      (viewMode() == StackedView::PreviewImageMode)
    {
        double z = DZoomBar::zoomFromSize(size, zoomMin(), zoomMax());
        setZoomFactor(z);
    }
    else if ((viewMode() == StackedView::IconViewMode)  ||
             (viewMode() == StackedView::TableViewMode) ||
             (viewMode() == StackedView::TrashViewMode))
    {
        if      (size > ThumbnailSize::maxThumbsSize())
        {
            d->thumbSize = ThumbnailSize::maxThumbsSize();
        }
        else if (size < ThumbnailSize::Small)
        {
            d->thumbSize = ThumbnailSize::Small;
        }
        else
        {
            d->thumbSize = size;
        }

        emit signalThumbSizeChanged(d->thumbSize);

        d->thumbSizeTimer->start();
    }
}

void ItemIconView::slotThumbSizeEffect()
{
    d->iconView->setThumbnailSize(ThumbnailSize(d->thumbSize));
    d->tableView->setThumbnailSize(ThumbnailSize(d->thumbSize));
    d->trashView->setThumbnailSize(ThumbnailSize(d->thumbSize));
    toggleZoomActions();

    ApplicationSettings::instance()->setDefaultIconSize(d->thumbSize);
}

} // namespace Digikam
