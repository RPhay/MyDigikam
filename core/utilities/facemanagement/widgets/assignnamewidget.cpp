/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-12
 * Description : Widget for assignment and confirmation of names for faces
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "assignnamewidget_p.h"

namespace Digikam
{

AssignNameWidget::AssignNameWidget(QWidget* const parent)
    : QFrame(parent),
      d     (new Private(this))
{
    setObjectName(QLatin1String("assignNameWidget"));
    setVisualStyle(StyledFrame);
}

AssignNameWidget::~AssignNameWidget()
{
    delete d;
}

void AssignNameWidget::setDefaultModel()
{
    setModel(nullptr, nullptr, nullptr);
}

void AssignNameWidget::setModel(TagModel* const model,
                                TagPropertiesFilterModel* const filteredModel,
                                CheckableAlbumFilterModel* const filterModel)
{
    // Restrict the tag properties filter model to people if configured.

    ApplicationSettings* const settings = ApplicationSettings::instance();

    if      (settings)
    {
        if (filteredModel && settings->showOnlyPersonTagsInPeopleSidebar())
        {
            filteredModel->listOnlyTagsWithProperty(TagPropertyName::person());
        }
    }

    if      (d->comboBox)
    {
        d->comboBox->setModel(model, filteredModel, filterModel);
    }
    else if (d->lineEdit)
    {
        d->lineEdit->setModel(model, filteredModel, filterModel);
    }

    if (model || filteredModel || filterModel)
    {
        // possibly set later on box

        d->modelsGiven      = true;
        d->tagModel         = model;
        d->tagFilterModel   = filterModel;
        d->tagFilteredModel = filteredModel;
    }
}

void AssignNameWidget::setParentTag(TAlbum* album)
{
    d->parentTag = album;

    if      (d->comboBox)
    {
        d->comboBox->setParentTag(album);
    }
    else if (d->lineEdit)
    {
        d->lineEdit->setParentTag(album);
    }
}

AddTagsComboBox* AssignNameWidget::comboBox() const
{
    return d->comboBox;
}

AddTagsLineEdit* AssignNameWidget::lineEdit() const
{
    return d->lineEdit;
}

void AssignNameWidget::setMode(Mode mode)
{
    /**
     * Reject tooltip should be updated even if the
     * same mode is passed, because Unconfirmed and Unknown
     * Faces have the same mode but different Tooltips.
     */
    if (mode == AssignNameWidget::UnconfirmedEditMode)
    {
        d->updateRejectButtonTooltip();
    }

    if (mode == d->mode)
    {
        return;
    }

    d->mode = mode;
    d->updateModes();
    d->updateContents();
}

AssignNameWidget::Mode AssignNameWidget::mode() const
{
    return d->mode;
}

void AssignNameWidget::setTagEntryWidgetMode(TagEntryWidgetMode mode)
{
    if (d->widgetMode == mode)
    {
        return;
    }

    d->widgetMode = mode;
    d->updateModes();
    d->updateContents();
}

AssignNameWidget::TagEntryWidgetMode AssignNameWidget::tagEntryWidgetMode() const
{
    return d->widgetMode;
}

void AssignNameWidget::setLayoutMode(LayoutMode mode)
{
    if (d->layoutMode == mode)
    {
        return;
    }

    d->layoutMode = mode;
    d->updateModes();
    d->updateContents();
}

AssignNameWidget::LayoutMode AssignNameWidget::layoutMode() const
{
    return d->layoutMode;
}

void AssignNameWidget::setVisualStyle(VisualStyle style)
{
    if (d->visualStyle == style)
    {
        return;
    }

    d->visualStyle = style;
    d->updateModes();
}

AssignNameWidget::VisualStyle AssignNameWidget::visualStyle() const
{
    return d->visualStyle;
}

void AssignNameWidget::setUserData(const ItemInfo& info, const QVariant& faceIdentifier)
{
    d->info            = info;
    d->faceIdentifier  = faceIdentifier;

    FaceTagsIface face = FaceTagsIface::fromVariant(faceIdentifier);

    /**
     * Ignored faces are drawn over with a different
     * overlay, as Reject button should be disabled.
     */
    if      (face.type() == FaceTagsIface::ConfirmedName)
    {
        setMode(AssignNameWidget::ConfirmedMode);
    }
    else if (face.type() == FaceTagsIface::IgnoredName)
    {
        setMode(AssignNameWidget::IgnoredMode);
    }
    else
    {
        setMode(AssignNameWidget::UnconfirmedEditMode);
    }
}

ItemInfo AssignNameWidget::info() const
{
    return d->info;
}

QVariant AssignNameWidget::faceIdentifier() const
{
    return d->faceIdentifier;
}

void AssignNameWidget::setCurrentFace(const FaceTagsIface& face)
{
    TAlbum* album = nullptr;

    if (!face.isNull() && !face.isUnknownName() && !face.isIgnoredName())
    {
        album = AlbumManager::instance()->findTAlbum(face.tagId());
    }

    setCurrentTag(album);
}

void AssignNameWidget::setCurrentTag(int tagId)
{
    setCurrentTag(AlbumManager::instance()->findTAlbum(tagId));
}

void AssignNameWidget::setCurrentTag(TAlbum* album)
{
    if (d->currentTag == album)
    {
        if (d->clickLabel)
        {
            d->clickLabel->setText(d->currentTag ? d->currentTag->title()
                                                 : QString());
        }

        return;
    }

    d->currentTag = album;
    d->updateContents();
}

void AssignNameWidget::slotConfirm()
{
    if      (d->comboBox)
    {
        emit assigned(d->comboBox->currentTaggingAction(), d->info, d->faceIdentifier);
    }
    else if (d->lineEdit)
    {
        emit assigned(d->lineEdit->currentTaggingAction(), d->info, d->faceIdentifier);
    }
}

void AssignNameWidget::slotReject()
{
    emit rejected(d->info, d->faceIdentifier);
}

void AssignNameWidget::slotActionActivated(const TaggingAction& action)
{
    emit assigned(action, d->info, d->faceIdentifier);
}

void AssignNameWidget::slotActionSelected(const TaggingAction& action)
{
    if (d->confirmButton)
    {
        d->confirmButton->setEnabled(action.isValid());
    }

    emit selected(action, d->info, d->faceIdentifier);
}

void AssignNameWidget::slotLabelClicked()
{
    emit labelClicked(d->info, d->faceIdentifier);
}

void AssignNameWidget::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        {
            return;
        }

        case Qt::Key_Escape:
        {
            slotReject();
            return;
        }
    }

    QWidget::keyPressEvent(e);
}

void AssignNameWidget::showEvent(QShowEvent* e)
{
    if ((d->mode == UnconfirmedEditMode) || (d->mode == ConfirmedEditMode))
    {
        if      (d->comboBox)
        {
            d->comboBox->lineEdit()->selectAll();
            d->comboBox->lineEdit()->setFocus();
        }
        else if (d->lineEdit)
        {
            d->lineEdit->selectAll();
            d->lineEdit->setFocus();
        }
    }

    QWidget::showEvent(e);
}

} // namespace Digikam
