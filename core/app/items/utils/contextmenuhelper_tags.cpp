/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-15
 * Description : contextmenu helper class - Tags options.
 *
 * Copyright (C) 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "contextmenuhelper_p.h"

namespace Digikam
{

void ContextMenuHelper::addActionNewTag(TagModificationHelper* helper, TAlbum* tag)
{
    QAction* const newTagAction = new QAction(QIcon::fromTheme(QLatin1String("tag-new")),
                                              i18nc("@action: context menu", "New Tag..."), this);
    addAction(newTagAction);
    helper->bindTag(newTagAction, tag);

    connect(newTagAction, SIGNAL(triggered()),
            helper, SLOT(slotTagNew()));
}

void ContextMenuHelper::addActionDeleteTag(TagModificationHelper* helper, TAlbum* tag)
{
    QAction* const deleteTagAction = new QAction(QIcon::fromTheme(QLatin1String("edit-delete")),
                                                 i18nc("@action: context menu", "Delete Tag"), this);
    addAction(deleteTagAction);
    helper->bindTag(deleteTagAction, tag);

    connect(deleteTagAction, SIGNAL(triggered()),
            helper, SLOT(slotTagDelete()));
}

void ContextMenuHelper::addActionDeleteTags(TagModificationHelper* helper, const QList<TAlbum*>& tags)
{
    QAction* const deleteTagsAction = new QAction(QIcon::fromTheme(QLatin1String("edit-delete")),
                                                  i18nc("@action: context menu", "Delete Tags"), this);
    addAction(deleteTagsAction);
    helper->bindMultipleTags(deleteTagsAction, tags);

    connect(deleteTagsAction, SIGNAL(triggered()),
            helper, SLOT(slotMultipleTagDel()));
}

void ContextMenuHelper::addActionTagToFaceTag(TagModificationHelper* helper, TAlbum* tag)
{
    QAction* const tagToFaceTagAction = new QAction(QIcon::fromTheme(QLatin1String("smiley")),
                                                    i18nc("@action: context menu", "Mark As Face Tag"), this);
    addAction(tagToFaceTagAction);
    helper->bindTag(tagToFaceTagAction, tag);

    connect(tagToFaceTagAction, SIGNAL(triggered()),
            helper, SLOT(slotTagToFaceTag()));
}

void ContextMenuHelper::addActionTagsToFaceTags(TagModificationHelper* helper, const QList<TAlbum*>& tags)
{
    QAction* const tagToFaceTagsAction = new QAction(QIcon::fromTheme(QLatin1String("smiley")),
                                                     i18nc("@action: context menu", "Mark As Face Tags"), this);
    addAction(tagToFaceTagsAction);
    helper->bindMultipleTags(tagToFaceTagsAction, tags);

    connect(tagToFaceTagsAction, SIGNAL(triggered()),
            helper, SLOT(slotMultipleTagsToFaceTags()));
}

void ContextMenuHelper::addActionEditTag(TagModificationHelper* helper, TAlbum* tag)
{
    QAction* const editTagAction = new QAction(QIcon::fromTheme(QLatin1String("tag-properties")),
                                               i18nc("@action: edit tag properties", "Properties..."), this);

    // This is only for the user to give a hint for the shortcut key

    editTagAction->setShortcut(Qt::ALT + Qt::Key_Return);
    addAction(editTagAction);
    helper->bindTag(editTagAction, tag);

    connect(editTagAction, SIGNAL(triggered()),
            helper, SLOT(slotTagEdit()));
}

void ContextMenuHelper::addActionDeleteFaceTag(TagModificationHelper* helper, TAlbum* tag)
{
    QAction* const deleteFaceTagAction = new QAction(QIcon::fromTheme(QLatin1String("tag")),
                                                     i18nc("@action: context menu", "Unmark Tag As Face"), this);
    deleteFaceTagAction->setWhatsThis(i18nc("@info: context menu", "Removes the face property from the selected tag "
                                            "and the face region from the contained images. "
                                            "Can also untag the images if wished."));
    addAction(deleteFaceTagAction);
    helper->bindTag(deleteFaceTagAction, tag);

    connect(deleteFaceTagAction, SIGNAL(triggered()),
            helper, SLOT(slotFaceTagDelete()));
}

void ContextMenuHelper::addActionDeleteFaceTags(TagModificationHelper* helper, const QList<TAlbum*>& tags)
{
    QAction* const deleteFaceTagsAction = new QAction(QIcon::fromTheme(QLatin1String("tag")),
                                                      i18nc("@action: context menu", "Unmark Tags As Face"), this);
    deleteFaceTagsAction->setWhatsThis(i18nc("@info: context menu", "Removes the face property from the selected tags "
                                             "and the face region from the contained images. "
                                             "Can also untag the images if wished."));
    addAction(deleteFaceTagsAction);
    helper->bindMultipleTags(deleteFaceTagsAction, tags);

    connect(deleteFaceTagsAction, SIGNAL(triggered()),
            helper, SLOT(slotMultipleFaceTagDel()));
}

void ContextMenuHelper::addAssignTagsMenu(const imageIds& ids)
{
    setSelectedIds(ids);

    QMenu* const assignTagsPopup = new TagsPopupMenu(ids, TagsPopupMenu::RECENTLYASSIGNED, d->parent);
    assignTagsPopup->menuAction()->setText(i18nc("@action: context menu", "A&ssign Tag"));
    assignTagsPopup->menuAction()->setIcon(QIcon::fromTheme(QLatin1String("tag")));
    d->parent->addMenu(assignTagsPopup);

    connect(assignTagsPopup, SIGNAL(signalTagActivated(int)),
            this, SIGNAL(signalAssignTag(int)));

    connect(assignTagsPopup, SIGNAL(signalPopupTagsView()),
            this, SIGNAL(signalPopupTagsView()));
}

void ContextMenuHelper::addRemoveTagsMenu(const imageIds& ids)
{
    setSelectedIds(ids);

    QMenu* const removeTagsPopup = new TagsPopupMenu(ids, TagsPopupMenu::REMOVE, d->parent);
    removeTagsPopup->menuAction()->setText(i18nc("@action: context menu", "R&emove Tag"));
    removeTagsPopup->menuAction()->setIcon(QIcon::fromTheme(QLatin1String("tag")));
    d->parent->addMenu(removeTagsPopup);

    // Performance: Only check for tags if there are <250 images selected
    // Otherwise enable it regardless if there are tags or not

    if (ids.count() < 250)
    {
        QList<int> tagIDs = CoreDbAccess().db()->getItemCommonTagIDs(ids);
        bool enable       = false;

        foreach (int tag, tagIDs)
        {
            if (
                (TagsCache::instance()->colorLabelForTag(tag) == -1)    &&
                (TagsCache::instance()->pickLabelForTag(tag)  == -1)    &&
                (TagsCache::instance()->isInternalTag(tag)    == false)
               )
            {
                enable = true;
                break;
            }
        }

        removeTagsPopup->menuAction()->setEnabled(enable);
    }

    connect(removeTagsPopup, SIGNAL(signalTagActivated(int)),
            this, SIGNAL(signalRemoveTag(int)));
}

void ContextMenuHelper::addLabelsAction()
{
    QMenu* const menuLabels           = new QMenu(i18nc("@action: context menu", "Assign Labe&ls"), d->parent);
    PickLabelMenuAction* const pmenu  = new PickLabelMenuAction(d->parent);
    ColorLabelMenuAction* const cmenu = new ColorLabelMenuAction(d->parent);
    RatingMenuAction* const rmenu     = new RatingMenuAction(d->parent);
    menuLabels->addAction(pmenu->menuAction());
    menuLabels->addAction(cmenu->menuAction());
    menuLabels->addAction(rmenu->menuAction());
    addSubMenu(menuLabels);

    connect(pmenu, SIGNAL(signalPickLabelChanged(int)),
            this, SIGNAL(signalAssignPickLabel(int)));

    connect(cmenu, SIGNAL(signalColorLabelChanged(int)),
            this, SIGNAL(signalAssignColorLabel(int)));

    connect(rmenu, SIGNAL(signalRatingChanged(int)),
            this, SIGNAL(signalAssignRating(int)));
}

void ContextMenuHelper::addCreateTagFromAddressbookMenu()
{

#ifdef HAVE_AKONADICONTACT

    AkonadiIface* const abc = new AkonadiIface(d->parent);

    connect(abc, SIGNAL(signalContactTriggered(QString)),
            this, SIGNAL(signalAddNewTagFromABCMenu(QString)));

    // AkonadiIface instance will be deleted with d->parent.

#endif

}

} // namespace Digikam
