/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-09-07
 * Description : Qt Model for ImportUI - drag and drop handling
 *
 * SPDX-FileCopyrightText: 2012      by Islam Wazery <wazery at ubuntu dot com>
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "importdragdrop.h"

// Qt includes

#include <QDropEvent>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "importiconview.h"
#include "importui.h"
#include "ddragobjects.h"
#include "importcategorizedview.h"
#include "camiteminfo.h"
#include "albummanager.h"
#include "digikamapp.h"
#include "itemiconview.h"

namespace Digikam
{

ImportDragDropHandler::ImportDragDropHandler(ImportItemModel* const model)
    : AbstractItemDragDropHandler(model)
{
}

QAction* ImportDragDropHandler::addGroupAction(QMenu* const menu)
{
    return menu->addAction(QIcon::fromTheme(QLatin1String("go-bottom")),
                           i18nc("@action:inmenu Group images with this image", "Group here"));
}

QAction* ImportDragDropHandler::addCancelAction(QMenu* const menu)
{
    return menu->addAction(QIcon::fromTheme(QLatin1String("dialog-cancel")), i18n("C&ancel"));
}

ImportDragDropHandler::DropAction ImportDragDropHandler::copyOrMove(const QDropEvent* e,
                                                                    QWidget* const view,
                                                                    bool allowMove,
                                                                    bool askForGrouping)
{

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    if      (e->modifiers() & Qt::ControlModifier)

#else

    if      (e->keyboardModifiers() & Qt::ControlModifier)

#endif

    {
        return CopyAction;
    }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    else if (e->modifiers() & Qt::ShiftModifier)

#else

    else if (e->keyboardModifiers() & Qt::ShiftModifier)

#endif

    {
        return MoveAction;
    }

    if (!allowMove && !askForGrouping)
    {
        switch (e->proposedAction())
        {
            case Qt::CopyAction:
            {
                return CopyAction;
            }

            case Qt::MoveAction:
            {
                return MoveAction;
            }

            default:
            {
                return NoAction;
            }
        }
    }

    QMenu popMenu(view);

    QAction* moveAction       = nullptr;

    if (allowMove)
    {
        moveAction = popMenu.addAction(QIcon::fromTheme(QLatin1String("go-jump")), i18n("&Move Here"));
    }

    QAction* const copyAction = popMenu.addAction(QIcon::fromTheme(QLatin1String("edit-copy")), i18n("&Copy Here"));
    popMenu.addSeparator();

    QAction* groupAction      = nullptr;

    if (askForGrouping)
    {
        groupAction = addGroupAction(&popMenu);
        popMenu.addSeparator();
    }

    addCancelAction(&popMenu);

    popMenu.setMouseTracking(true);
    QAction* const choice = popMenu.exec(QCursor::pos());

    if      (moveAction && (choice == moveAction))
    {
        return MoveAction;
    }
    else if (choice == copyAction)
    {
        return CopyAction;
    }
    else if (groupAction && (choice == groupAction))
    {
        return GroupAction;
    }

    return NoAction;
}

/*
static DropAction tagAction(const QDropEvent*, QWidget* view, bool askForGrouping)
{
}

static DropAction groupAction(const QDropEvent*, QWidget* view)
{
}
*/

bool ImportDragDropHandler::dropEvent(QAbstractItemView* abstractview,
                                      const QDropEvent* e,
                                      const QModelIndex& droppedOn)
{
    ImportCategorizedView* const view = static_cast<ImportCategorizedView*>(abstractview);

    if (accepts(e, droppedOn) == Qt::IgnoreAction)
    {
        return false;
    }

    if (DItemDrag::canDecode(e->mimeData()))
    {
        QList<QUrl> lst         = DigikamApp::instance()->view()->selectedUrls();

        QMenu popMenu(view);
        popMenu.addSection(QIcon::fromTheme(QLatin1String("digikam")), i18n("Exporting"));
        QAction* const upAction = popMenu.addAction(QIcon::fromTheme(QLatin1String("media-flash-sd-mmc")),
                                                    i18n("Upload to Camera"));
        popMenu.addSeparator();
        popMenu.addAction(QIcon::fromTheme(QLatin1String("dialog-cancel")), i18n("C&ancel"));
        popMenu.setMouseTracking(true);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        QAction* const choice   = popMenu.exec(view->mapToGlobal(e->position().toPoint()));

#else

        QAction* const choice   = popMenu.exec(view->mapToGlobal(e->pos()));

#endif

        if (choice)
        {
            if (choice == upAction)
            {
                ImportUI::instance()->slotUploadItems(lst);
            }
        }

        return true;
    }
/*
    TODO: Implement tag dropping in import tool.
    else if (DTagListDrag::canDecode(e->mimeData()))
    {
    }
*/
    return false;
}

Qt::DropAction ImportDragDropHandler::accepts(const QDropEvent* e, const QModelIndex& /*dropIndex*/)
{
    if (DItemDrag::canDecode(e->mimeData()) || e->mimeData()->hasUrls())
    {

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        if      (e->modifiers() & Qt::ControlModifier)

#else

        if      (e->keyboardModifiers() & Qt::ControlModifier)

#endif

        {
            return Qt::CopyAction;
        }

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        else if (e->modifiers() & Qt::ShiftModifier)

#else

        else if (e->keyboardModifiers() & Qt::ShiftModifier)

#endif
        {
            return Qt::MoveAction;
        }

        return Qt::MoveAction;
    }

    if (DTagListDrag::canDecode(e->mimeData())        ||
        DCameraItemListDrag::canDecode(e->mimeData()) ||
        DCameraDragObject::canDecode(e->mimeData()))
    {
        return Qt::MoveAction;
    }

    return Qt::IgnoreAction;
}

QStringList ImportDragDropHandler::mimeTypes() const
{
    QStringList mimeTypes;
    mimeTypes << DItemDrag::mimeTypes()
              << DTagListDrag::mimeTypes()
              << DCameraItemListDrag::mimeTypes()
              << DCameraDragObject::mimeTypes()
              << QLatin1String("text/uri-list");

    return mimeTypes;
}

QMimeData* ImportDragDropHandler::createMimeData(const QList<QModelIndex>& indexes)
{
    QList<CamItemInfo> infos = model()->camItemInfos(indexes);

    QStringList lst;

    for (const CamItemInfo& info : std::as_const(infos))
    {
        lst.append(info.folder + info.name);
    }

    if (lst.isEmpty())
    {
        return nullptr;
    }

    return (new DCameraItemListDrag(lst));
}

ImportItemModel* ImportDragDropHandler::model() const
{
    return static_cast<ImportItemModel*>(m_model);
}

} // namespace Digikam

#include "moc_importdragdrop.cpp"
