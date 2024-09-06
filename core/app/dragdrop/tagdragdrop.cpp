/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-21
 * Description : Qt Model for Tag - drag and drop handling
 *
 * SPDX-FileCopyrightText: 2009      by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2013      by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "tagdragdrop.h"
#include "facetags.h"
#include "digikamitemview_p.h"

// Qt includes

#include <QDropEvent>
#include <QMenu>
#include <QIcon>
#include <QMessageBox>
#include <QApplication>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "ddragobjects.h"
#include "iteminfo.h"
#include "albumtreeview.h"

namespace Digikam
{

TagDragDropHandler::TagDragDropHandler(TagModel* const model)
    : AlbumModelDragDropHandler(model)
{
}

TagModel* TagDragDropHandler::model() const
{
    return (static_cast<TagModel*>(m_model));
}

bool TagDragDropHandler::dropEvent(QAbstractItemView* view,
                                   const QDropEvent* e,
                                   const QModelIndex& droppedOn)
{
    if (accepts(e, droppedOn) == Qt::IgnoreAction)
    {
        return false;
    }

    TAlbum* const destAlbum = model()->albumForIndex(droppedOn);

    if (DTagListDrag::canDecode(e->mimeData()))
    {
        QList<int> tagIDs;

        if (!DTagListDrag::decode(e->mimeData(), tagIDs))
        {
            return false;
        }

        if (tagIDs.isEmpty())
        {
            return false;
        }

        QMenu popMenu(view);
        QAction* const gotoAction  = popMenu.addAction(QIcon::fromTheme(QLatin1String("go-jump")), i18n("&Move Here"));
        QAction* const mergeAction = popMenu.addAction(QIcon::fromTheme(QLatin1String("merge")),   i18n("M&erge Here"));
        popMenu.addSeparator();
        popMenu.addAction(QIcon::fromTheme(QLatin1String("dialog-cancel")), i18n("C&ancel"));
        popMenu.setMouseTracking(true);
        QAction* const choice      = popMenu.exec(QCursor::pos());

        for (int index = 0 ; index < tagIDs.count() ; ++index)
        {
            TAlbum* const talbum = AlbumManager::instance()->findTAlbum(tagIDs.at(index));

            if (!talbum)
            {
                return false;
            }

            if (destAlbum && (talbum == destAlbum))
            {
                return false;
            }

            if      (choice == gotoAction)
            {
                TAlbum* newParentTag = nullptr;

                if (!destAlbum)
                {
                    // move dragItem to the root

                    newParentTag = AlbumManager::instance()->findTAlbum(0);
                }
                else
                {
                    // move dragItem as child of dropItem

                    newParentTag = destAlbum;
                }

                QString errMsg;

                if (!AlbumManager::instance()->moveTAlbum(talbum, newParentTag, errMsg))
                {
                    QMessageBox::critical(view, qApp->applicationName(), errMsg);
                }

                if (view && !view->isVisible())
                {
                    view->setVisible(true);
                }
            }
            else if (choice == mergeAction)
            {
                if (!destAlbum)
                {
                    return false;
                }

                QString errMsg;

                if (!AlbumManager::instance()->mergeTAlbum(talbum, destAlbum, true, errMsg))
                {
                    QMessageBox::critical(view, qApp->applicationName(), errMsg);
                }

                if (view && !view->isVisible())
                {
                    view->setVisible(true);
                }
            }
        }

        return true;
    }
    else if (DItemDrag::canDecode(e->mimeData()))
    {
        QList<QUrl>      urls;
        QList<int>       albumIDs;
        QList<qlonglong> imageIDs;

        if (!DItemDrag::decode(e->mimeData(), urls, albumIDs, imageIDs))
        {
            return false;
        }

        if (urls.isEmpty() || albumIDs.isEmpty() || imageIDs.isEmpty())
        {
            return false;
        }

        if (destAlbum->id() == FaceTags::personParentTag())
        {
            return false;
        }

        // here the user dragged a "faceitem" to a "person" album

        if (destAlbum->hasProperty(TagPropertyName::person()))
        {
            // use dropped on album

            QString targetName           = destAlbum->title();
            DigikamItemView* const dview = qobject_cast<DigikamItemView*>(e->source());

            if (dview)
            {
                // get selected indexes

                QModelIndexList selectedIndexes = dview->selectionModel()->selectedIndexes();

                // get the tag ids for each of the selected faces

                QList<int> faceIds              = dview->getFaceIds(selectedIndexes);

                // create list with face names

                QStringList faceNames;

                for (const int& faceId : std::as_const(faceIds))
                {
                    // check that all selected faces are the same person

                    if (!faceId || faceId != faceIds.first())
                    {
                        return false;
                    }

                    faceNames << FaceTags::faceNameForTag(faceId);
                }

                // here we set a new thumbnail

                if      ((imageIDs.size() == 1) && (destAlbum->id() == faceIds.first()))
                {
                    bool set = false;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

                    if (e->modifiers() == Qt::ControlModifier)

#else

                    if (e->keyboardModifiers() == Qt::ControlModifier)

#endif

                    {
                        set = true;
                    }
                    else
                    {
                        QMenu popMenu(view);
                        QAction* const setAction = popMenu.addAction(i18n("Set as Tag Thumbnail"));
                        popMenu.addSeparator();
                        popMenu.addAction(QIcon::fromTheme(QLatin1String("dialog-cancel")), i18n("C&ancel"));

                        popMenu.setMouseTracking(true);
                        QAction* const choice   = popMenu.exec(QCursor::pos());
                        set                     = (choice == setAction);
                    }

                    if (set)
                    {
                        QString errMsg;
                        AlbumManager::instance()->updateTAlbumIcon(destAlbum, QString(), imageIDs.first(), errMsg);
                    }

                    return true;
                }
                else if (destAlbum->id() != faceIds.first())
                {
                    //here we move assign a new face tag to the selected faces

                    if (destAlbum->id() == FaceTags::unconfirmedPersonTagId())
                    {
                        return false;
                    }

                    bool assign = false;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

                    if (e->modifiers() == Qt::ControlModifier)

#else

                    if (e->keyboardModifiers() == Qt::ControlModifier)

#endif

                    {
                        assign = true;
                    }
                    else
                    {
                        QMenu popMenu(view);
                        QAction* const assignAction = popMenu.addAction(QIcon::fromTheme(QLatin1String("tag")),
                                                                        i18np("Change face name from '%2' to '%3'",
                                                                              "Change face names from '%2' to '%3'",
                                                                              faceNames.count(),
                                                                              faceNames.first(),
                                                                              targetName));
                        popMenu.addSeparator();
                        popMenu.addAction(QIcon::fromTheme(QLatin1String("dialog-cancel")), i18n("C&ancel"));

                        popMenu.setMouseTracking(true);
                        QAction* const choice       = popMenu.exec(QCursor::pos());
                        assign                      = (choice == assignAction);
                    }

                    if (assign)
                    {
                        if      (destAlbum->id() == FaceTags::unknownPersonTagId())
                        {
                            dview->unknownFaces(selectedIndexes);
                        }
                        else if (destAlbum->id() == FaceTags::ignoredPersonTagId())
                        {
                            dview->ignoreFaces(selectedIndexes);
                        }
                        else
                        {
                            int dstId = destAlbum->parent() ? destAlbum->parent()->id() : -1;
                            int tagId = FaceTags::getOrCreateTagForPerson(targetName, dstId);
                            dview->confirmFaces(selectedIndexes, tagId);
                        }
                    }

                    return true;
                }

                return false;
            }
        }

        // If a ctrl key is pressed while dropping the drag object,
        // the tag is assigned to the images without showing a
        // popup menu.

        bool hasSameTopId = false;
        bool assign       = false;
        bool set          = false;

        // Use selected tags instead of dropped on.

        QList<int> tagIdList;
        QStringList tagNames;

        AbstractAlbumTreeView* const tview = dynamic_cast<AbstractAlbumTreeView*>(view);

        if (!tview)
        {
            return false;
        }

        if (imageIDs.size() == 1)
        {
            const int topId = AlbumManager::instance()->findTopId(destAlbum->id());
            const auto tids = ItemInfo(imageIDs.first()).tagIds();

            for (int tid : tids)
            {
                if (AlbumManager::instance()->findTopId(tid) == topId)
                {
                    hasSameTopId = true;
                    break;
                }
            }
        }

        QList<Album*> selTags = tview->selectedItems();

        for (int it = 0 ; it < selTags.count() ; ++it)
        {
            TAlbum* const temp = dynamic_cast<TAlbum*>(selTags.at(it));

            if (temp)
            {
                tagIdList << temp->id();
                tagNames  << temp->title();
            }
        }

        // If nothing selected, use dropped on tag

        if (tagIdList.isEmpty())
        {
            tagIdList << destAlbum->id();
            tagNames  << destAlbum->title();
        }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        if (e->modifiers() == Qt::ControlModifier)

#else

        if (e->keyboardModifiers() == Qt::ControlModifier)

#endif

        {
            assign = true;
        }
        else
        {
            QMenu popMenu(view);
            QAction* setAction = nullptr;
            QAction* const assignAction = popMenu.addAction(QIcon::fromTheme(QLatin1String("tag")),
                                                            i18n("Assign Tag(s) '%1' to Items",
                                                                 tagNames.join(QLatin1String(", "))));

            if (hasSameTopId)
            {
                setAction = popMenu.addAction(i18n("Set as Tag Thumbnail"));
            }

            popMenu.addSeparator();
            popMenu.addAction(QIcon::fromTheme(QLatin1String("dialog-cancel")), i18n("C&ancel"));

            popMenu.setMouseTracking(true);
            QAction* const choice = popMenu.exec(QCursor::pos());
            assign                = (choice == assignAction);
            set                   = (setAction && (choice == setAction));
        }

        if      (assign)
        {
            Q_EMIT assignTags(imageIDs, tagIdList);
        }
        else if (set)
        {
            QString errMsg;
            AlbumManager::instance()->updateTAlbumIcon(destAlbum, QString(), imageIDs.first(), errMsg);
        }

        return true;
    }

    return false;
}

Qt::DropAction TagDragDropHandler::accepts(const QDropEvent* e, const QModelIndex& dropIndex)
{
    TAlbum* const destAlbum = model()->albumForIndex(dropIndex);

    if      (DTagListDrag::canDecode(e->mimeData()))
    {
/*
        int droppedId = 0;
*/
        QList<int> droppedId;

        if (!DTagListDrag::decode(e->mimeData(), droppedId))
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "List decode error" << droppedId.isEmpty();
            return Qt::IgnoreAction;
        }

        TAlbum* const droppedAlbum = AlbumManager::instance()->findTAlbum(droppedId.first());

        if (!droppedAlbum)
        {
            return Qt::IgnoreAction;
        }

        // Allow dragging on empty space to move the dragged album under the root albumForIndex
        // unless the itemDrag is already at root level

        if (!destAlbum)
        {
            Album* const palbum = droppedAlbum->parent();

            if (!palbum)
            {
                 return Qt::IgnoreAction;
            }

            if (palbum->isRoot())
            {
                return Qt::IgnoreAction;
            }
            else
            {
                return Qt::MoveAction;
            }
        }

        // Dragging an item on itself makes no sense

        if (destAlbum == droppedAlbum)
        {
            return Qt::IgnoreAction;
        }

        // Dragging a parent on its child makes no sense

        if (droppedAlbum && droppedAlbum->isAncestorOf(destAlbum))
        {
            return Qt::IgnoreAction;
        }

        return Qt::MoveAction;
    }
    else if (DItemDrag::canDecode(e->mimeData()) && destAlbum && destAlbum->parent())
    {
        // Only other possibility is image items being dropped
        // And allow this only if there is a Tag to be dropped
        // on and also the Tag is not root.

        return Qt::CopyAction;
    }

    return Qt::IgnoreAction;
}

QStringList TagDragDropHandler::mimeTypes() const
{
    QStringList mimeTypes;

    mimeTypes << DTagListDrag::mimeTypes()
              << DItemDrag::mimeTypes();

    return mimeTypes;
}

QMimeData* TagDragDropHandler::createMimeData(const QList<Album*>& albums)
{

    if (albums.isEmpty())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot drag no tag";
        return nullptr;
    }

    QList<int> ids;

    for (Album* const album : std::as_const(albums))
    {
        ids << album->id();
    }

    return new DTagListDrag(ids);
}

} // namespace Digikam

#include "moc_tagdragdrop.cpp"
