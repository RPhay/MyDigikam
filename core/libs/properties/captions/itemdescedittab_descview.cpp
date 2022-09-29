/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-03-09
 * Description : Captions, Tags, and Rating properties editor - Description view.
 *
 * SPDX-FileCopyrightText: 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2003-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2015      by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemdescedittab_p.h"

namespace Digikam
{

void ItemDescEditTab::setFocusToTitlesEdit()
{
    d->tabWidget->setCurrentIndex(Private::DESCRIPTIONS);
    d->titleEdit->textEdit()->setFocus();
}

void ItemDescEditTab::setFocusToCommentsEdit()
{
    d->tabWidget->setCurrentIndex(Private::DESCRIPTIONS);
    d->captionsEdit->altLangStrEdit()->textEdit()->setFocus();
}

void ItemDescEditTab::slotCommentChanged()
{
    d->hub.setComments(d->captionsEdit->values());
    setMetadataWidgetStatus(d->hub.commentsStatus(), d->captionsEdit);
    slotModified();
}

void ItemDescEditTab::slotTitleChanged()
{
    CaptionsMap titles;

    titles.fromAltLangMap(d->titleEdit->values());
    d->hub.setTitles(titles);
    setMetadataWidgetStatus(d->hub.titlesStatus(), d->titleEdit);
    slotModified();
}

void ItemDescEditTab::slotDateTimeChanged(const QDateTime& dateTime)
{
    d->hub.setDateTime(dateTime);
    setMetadataWidgetStatus(d->hub.dateTimeStatus(), d->dateTimeEdit);
    slotModified();
}

void ItemDescEditTab::slotPickLabelChanged(int pickId)
{
    d->hub.setPickLabel(pickId);

    // no handling for MetadataDisjoint needed for pick label,
    // we set it to 0 when disjoint, see below

    slotModified();
}

void ItemDescEditTab::slotColorLabelChanged(int colorId)
{
    d->hub.setColorLabel(colorId);

    // no handling for MetadataDisjoint needed for color label,
    // we set it to 0 when disjoint, see below

    slotModified();
}

void ItemDescEditTab::slotRatingChanged(int rating)
{
    d->hub.setRating(rating);

    // no handling for MetadataDisjoint needed for rating,
    // we set it to 0 when disjoint, see below

    slotModified();
}

void ItemDescEditTab::assignPickLabel(int pickId)
{
    d->pickLabelSelector->setPickLabel((PickLabel)pickId);
}

void ItemDescEditTab::assignColorLabel(int colorId)
{
    d->colorLabelSelector->setColorLabel((ColorLabel)colorId);
}

void ItemDescEditTab::assignRating(int rating)
{
    d->ratingWidget->setRating(rating);
}

void ItemDescEditTab::updateComments()
{
    d->captionsEdit->blockSignals(true);
    d->captionsEdit->setValues(d->hub.comments());
    setMetadataWidgetStatus(d->hub.commentsStatus(), d->captionsEdit);
    d->captionsEdit->blockSignals(false);

    d->titleEdit->blockSignals(true);
    d->titleEdit->setValues(d->hub.titles().toAltLangMap());
    setMetadataWidgetStatus(d->hub.titlesStatus(), d->titleEdit);
    d->titleEdit->blockSignals(false);
}

void ItemDescEditTab::updatePickLabel()
{
    d->pickLabelSelector->blockSignals(true);

    if (d->hub.pickLabelStatus() == DisjointMetadataDataFields::MetadataDisjoint)
    {
        d->pickLabelSelector->setPickLabel(NoPickLabel);
    }
    else
    {
        d->pickLabelSelector->setPickLabel((PickLabel)d->hub.pickLabel());
    }

    d->pickLabelSelector->blockSignals(false);
}

void ItemDescEditTab::updateColorLabel()
{
    d->colorLabelSelector->blockSignals(true);

    if (d->hub.colorLabelStatus() == DisjointMetadataDataFields::MetadataDisjoint)
    {
        d->colorLabelSelector->setColorLabel(NoColorLabel);
    }
    else
    {
        d->colorLabelSelector->setColorLabel((ColorLabel)d->hub.colorLabel());
    }

    d->colorLabelSelector->blockSignals(false);
}

void ItemDescEditTab::updateRating()
{
    d->ratingWidget->blockSignals(true);

    if (d->hub.ratingStatus() == DisjointMetadataDataFields::MetadataDisjoint)
    {
        d->ratingWidget->setRating(0);
    }
    else
    {
        d->ratingWidget->setRating(d->hub.rating());
    }

    d->ratingWidget->blockSignals(false);
}

void ItemDescEditTab::updateDate()
{
    d->dateTimeEdit->blockSignals(true);
    d->dateTimeEdit->setDateTime(d->hub.dateTime());
    setMetadataWidgetStatus(d->hub.dateTimeStatus(), d->dateTimeEdit);
    d->dateTimeEdit->blockSignals(false);
}

void ItemDescEditTab::slotImageRatingChanged(qlonglong imageId)
{
    metadataChange(imageId);
}

void ItemDescEditTab::slotImageCaptionChanged(qlonglong imageId)
{
    metadataChange(imageId);
}

void ItemDescEditTab::slotImageDateChanged(qlonglong imageId)
{
    metadataChange(imageId);
}

void ItemDescEditTab::resetTitleEditPlaceholderText()
{
    d->titleEdit->setPlaceholderText(i18n("Enter title text here."));
}

void ItemDescEditTab::resetCaptionEditPlaceholderText()
{
    d->captionsEdit->setPlaceholderText(i18n("Enter caption text here."));
}

} // namespace Digikam
