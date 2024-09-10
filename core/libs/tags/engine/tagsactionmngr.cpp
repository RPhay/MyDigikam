/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-01-24
 * Description : Tags Action Manager
 *
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "tagsactionmngr.h"

// Qt includes

#include <QList>
#include <QShortcut>
#include <QIcon>
#include <QKeySequence>
#include <QApplication>
#include <QAction>

// KDE includes

#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <klocalizedstring.h>
#include <kactioncollection.h>

// Restore warnings
#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

// Local includes

#include "digikam_debug.h"
#include "album.h"
#include "coredb.h"
#include "albummanager.h"
#include "coredbaccess.h"
#include "coredbconstants.h"
#include "coredbwatch.h"
#include "coredbinfocontainers.h"
#include "digikamapp.h"
#include "dxmlguiwindow.h"
#include "itemiconview.h"
#include "imagewindow.h"
#include "lighttablewindow.h"
#include "picklabelwidget.h"
#include "colorlabelwidget.h"
#include "tagscache.h"
#include "tagproperties.h"
#include "ratingwidget.h"
#include "syncjob.h"

namespace Digikam
{

TagsActionMngr* TagsActionMngr::m_defaultManager = nullptr;

TagsActionMngr* TagsActionMngr::defaultManager()
{
    return m_defaultManager;
}

class Q_DECL_HIDDEN TagsActionMngr::Private
{
public:

    Private() = default;

    QMultiMap<int, QAction*>  tagsActionMap;
    QList<KActionCollection*> actionCollectionList;

    const QString             ratingShortcutPrefix  = QLatin1String("rateshortcut");
    const QString             tagShortcutPrefix     = QLatin1String("tagshortcut");
    const QString             pickShortcutPrefix    = QLatin1String("pickshortcut");
    const QString             colorShortcutPrefix   = QLatin1String("colorshortcut");
};

// -------------------------------------------------------------------------------------------------

TagsActionMngr::TagsActionMngr(QWidget* const parent)
    : QObject(parent),
      d      (new Private)
{
    if (!m_defaultManager)
    {
        m_defaultManager = this;
    }

    connect(AlbumManager::instance(), SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotAlbumDeleted(Album*)));
}

TagsActionMngr::~TagsActionMngr()
{
    delete d;

    if (m_defaultManager == this)
    {
        m_defaultManager = nullptr;
    }
}

QString TagsActionMngr::ratingShortcutPrefix() const
{
    return d->ratingShortcutPrefix;
}

QString TagsActionMngr::tagShortcutPrefix() const
{
    return d->tagShortcutPrefix;
}

QString TagsActionMngr::pickShortcutPrefix() const
{
    return d->pickShortcutPrefix;
}

QString TagsActionMngr::colorShortcutPrefix() const
{
    return d->colorShortcutPrefix;
}

void TagsActionMngr::registerTagsActionCollections()
{
    d->actionCollectionList.append(DigikamApp::instance()->actionCollection());
    d->actionCollectionList.append(ImageWindow::imageWindow()->actionCollection());
    d->actionCollectionList.append(LightTableWindow::lightTableWindow()->actionCollection());

    // Create Tags shortcuts.

    QList<int> tagIds = TagsCache::instance()->tagsWithProperty(TagPropertyName::tagKeyboardShortcut());

    for (int tagId : std::as_const(tagIds))
    {
        createTagActionShortcut(tagId);
    }
}

QList<KActionCollection*> TagsActionMngr::actionCollections() const
{
    return d->actionCollectionList;
}

void TagsActionMngr::registerLabelsActions(KActionCollection* const ac)
{
    // Create Rating shortcuts.

    for (int i = RatingMin ; i <= RatingMax ; ++i)
    {
        createRatingActionShortcut(ac, i);
    }

    // Create Color Label shortcuts.

    for (int i = NoColorLabel ; i <= WhiteLabel ; ++i)
    {
        createColorLabelActionShortcut(ac, i);
    }

    // Create Pick Label shortcuts.

    for (int i = NoPickLabel ; i <= AcceptedLabel ; ++i)
    {
        createPickLabelActionShortcut(ac, i);
    }
}

void TagsActionMngr::registerActionsToWidget(QWidget* const wdg)
{
    DXmlGuiWindow* const win = dynamic_cast<DXmlGuiWindow*>(qApp->activeWindow());

    if (win)
    {
        const auto acs = win->actionCollection()->actions();

        for (QAction* const ac : acs)
        {
            if (
                ac->objectName().startsWith(d->ratingShortcutPrefix) ||
                ac->objectName().startsWith(d->tagShortcutPrefix)    ||
                ac->objectName().startsWith(d->pickShortcutPrefix)   ||
                ac->objectName().startsWith(d->colorShortcutPrefix)
               )
            {
                wdg->addAction(ac);
            }
        }
    }
}

bool TagsActionMngr::createRatingActionShortcut(KActionCollection* const ac, int rating)
{
    if (ac)
    {
        QAction* const action = ac->addAction(QString::fromUtf8("%1-%2").arg(d->ratingShortcutPrefix).arg(rating));
        action->setText(i18n("Assign Rating \"%1 Star\"", rating));
        ac->setDefaultShortcut(action, QKeySequence(QString::fromUtf8("CTRL+%1").arg(rating)));
        action->setIcon(RatingWidget::buildIcon(rating, 32));
        action->setData(rating);

        connect(action, SIGNAL(triggered()),
                this, SLOT(slotAssignFromShortcut()));

        return true;
    }

    return false;
}

bool TagsActionMngr::createPickLabelActionShortcut(KActionCollection* const ac, int pickId)
{
    if (ac)
    {
        QAction* const action = ac->addAction(QString::fromUtf8("%1-%2").arg(d->pickShortcutPrefix).arg(pickId));
        action->setText(i18n("Assign Pick Label \"%1\"", PickLabelWidget::labelPickName((PickLabel)pickId)));
        ac->setDefaultShortcut(action, QKeySequence(QString::fromUtf8("ALT+%1").arg(pickId)));
        action->setIcon(PickLabelWidget::buildIcon((PickLabel)pickId));
        action->setData(pickId);

        connect(action, SIGNAL(triggered()),
                this, SLOT(slotAssignFromShortcut()));

        return true;
    }

    return false;
}

bool TagsActionMngr::createColorLabelActionShortcut(KActionCollection* const ac, int colorId)
{
    if (ac)
    {
        QAction* const action = ac->addAction(QString::fromUtf8("%1-%2").arg(d->colorShortcutPrefix).arg(colorId));
        action->setText(i18n("Assign Color Label \"%1\"", ColorLabelWidget::labelColorName((ColorLabel)colorId)));
        ac->setDefaultShortcut(action, QKeySequence(QString::fromUtf8("ALT+CTRL+%1").arg(colorId)));
        action->setIcon(ColorLabelWidget::buildIcon((ColorLabel)colorId, 32));
        action->setData(colorId);

        connect(action, SIGNAL(triggered()),
                this, SLOT(slotAssignFromShortcut()));

        return true;
    }

    return false;
}

bool TagsActionMngr::createTagActionShortcut(int tagId)
{
    if (!tagId)
    {
        return false;
    }

    TAlbum* const talbum = AlbumManager::instance()->findTAlbum(tagId);

    if (!talbum)
    {
        return false;
    }

    QString value = TagsCache::instance()->propertyValue(tagId, TagPropertyName::tagKeyboardShortcut());

    if (value.isEmpty())
    {
        return false;
    }

    QKeySequence ks(value);

    // FIXME: tag icons can be files on disk, or system icon names. Only the latter will work here.

    QIcon     icon(SyncJob::getTagThumbnail(talbum));

    qCDebug(DIGIKAM_GENERAL_LOG) << "Create Shortcut " << ks.toString()
                                 << " to Tag " << talbum->title()
                                 << " (" << tagId << ")";

    for (KActionCollection* const ac : std::as_const(d->actionCollectionList))
    {
        QString actionName    = QString::fromUtf8("%1-%2").arg(d->tagShortcutPrefix).arg(tagId);
        QAction* const action = new QAction(i18n("Assign Tag \"%1\"", talbum->title()), this);
        action->setShortcut(ks);
        ac->addAction(actionName, action);
        ac->writeSettings(nullptr, true, action);
        action->setIcon(icon);
        action->setData(tagId);

        connect(action, SIGNAL(triggered()),
                this, SLOT(slotAssignFromShortcut()));

        connect(action, SIGNAL(changed()),
                this, SLOT(slotTagActionChanged()));

        d->tagsActionMap.insert(tagId, action);
    }

    return true;
}

void TagsActionMngr::slotTagActionChanged()
{
    QAction* const action = dynamic_cast<QAction*>(sender());

    if (!action)
    {
        return;
    }

    int tagId       = action->data().toInt();

    QKeySequence ks;
    QStringList lst = action->shortcut().toString().split(QLatin1Char(','));

    if (!lst.isEmpty())
    {
        ks = QKeySequence(lst.first());
    }

    updateTagShortcut(tagId, ks, false);
}

void TagsActionMngr::updateTagShortcut(int tagId, const QKeySequence& ks, bool delAction)
{
    if (!tagId)
    {
        return;
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "Tag Shortcut " << tagId << "Changed to " << ks;

    QString value = TagsCache::instance()->propertyValue(tagId, TagPropertyName::tagKeyboardShortcut());

    if (value == ks.toString())
    {
        return;
    }

    TagProperties tprop(tagId);

    if (ks.isEmpty())
    {
        for (KActionCollection* const ac : std::as_const(d->actionCollectionList))
        {
            QString actionName    = QString::fromUtf8("%1-%2").arg(d->tagShortcutPrefix).arg(tagId);
            QAction* const action = ac->action(actionName);

            if (action)
            {
                action->setShortcut(QKeySequence());
                ac->writeSettings(nullptr, true, action);
            }
        }

        removeTagActionShortcut(tagId, delAction);
        tprop.removeProperties(TagPropertyName::tagKeyboardShortcut());
    }
    else
    {
        removeTagActionShortcut(tagId, delAction);
        tprop.setProperty(TagPropertyName::tagKeyboardShortcut(), ks.toString());
        createTagActionShortcut(tagId);
    }
}

void TagsActionMngr::slotAlbumDeleted(Album* album)
{
    TAlbum* const talbum = dynamic_cast<TAlbum*>(album);

    if (!talbum)
    {
        return;
    }

    if (removeTagActionShortcut(talbum->id()))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Delete Shortcut assigned to tag " << album->id();
    }
}

bool TagsActionMngr::removeTagActionShortcut(int tagId, bool delAction)
{
    if (!d->tagsActionMap.contains(tagId))
    {
        return false;
    }

    const auto acs = d->tagsActionMap.values(tagId);

    for (QAction* const act : acs)
    {
        if (act)
        {
            KActionCollection* const ac = dynamic_cast<KActionCollection*>(act->parent());

            if (ac)
            {
                ac->takeAction(act);
            }

            if (delAction)
            {
                delete act;
            }
        }
    }

    d->tagsActionMap.remove(tagId);

    return true;
}

void TagsActionMngr::slotAssignFromShortcut()
{
    QAction* const action = dynamic_cast<QAction*>(sender());

    if (!action)
    {
        return;
    }

    int val               = action->data().toInt();
    qCDebug(DIGIKAM_GENERAL_LOG) << "Shortcut value: " << val;

    QWidget* const w      = qApp->activeWindow();
    DigikamApp* const dkw = dynamic_cast<DigikamApp*>(w);

    if (dkw)
    {
        //qCDebug(DIGIKAM_GENERAL_LOG) << "Handling by DigikamApp";

        if      (action->objectName().startsWith(d->ratingShortcutPrefix))
        {
            dkw->view()->slotAssignRating(val);
        }
        else if (action->objectName().startsWith(d->pickShortcutPrefix))
        {
            dkw->view()->slotAssignPickLabel(val);
        }
        else if (action->objectName().startsWith(d->colorShortcutPrefix))
        {
            dkw->view()->slotAssignColorLabel(val);
        }
        else if (action->objectName().startsWith(d->tagShortcutPrefix))
        {
            dkw->view()->toggleTag(val);
        }

        return;
    }

    ImageWindow* const imw = dynamic_cast<ImageWindow*>(w);

    if (imw)
    {
        //qCDebug(DIGIKAM_GENERAL_LOG) << "Handling by ImageWindow";

        if      (action->objectName().startsWith(d->ratingShortcutPrefix))
        {
            imw->slotAssignRating(val);
        }
        else if (action->objectName().startsWith(d->pickShortcutPrefix))
        {
            imw->slotAssignPickLabel(val);
        }
        else if (action->objectName().startsWith(d->colorShortcutPrefix))
        {
            imw->slotAssignColorLabel(val);
        }
        else if (action->objectName().startsWith(d->tagShortcutPrefix))
        {
            imw->toggleTag(val);
        }

        return;
    }

    LightTableWindow* const ltw = dynamic_cast<LightTableWindow*>(w);

    if (ltw)
    {
        //qCDebug(DIGIKAM_GENERAL_LOG) << "Handling by LightTableWindow";

        if      (action->objectName().startsWith(d->ratingShortcutPrefix))
        {
            ltw->slotAssignRating(val);
        }
        else if (action->objectName().startsWith(d->pickShortcutPrefix))
        {
            ltw->slotAssignPickLabel(val);
        }
        else if (action->objectName().startsWith(d->colorShortcutPrefix))
        {
            ltw->slotAssignColorLabel(val);
        }
        else if (action->objectName().startsWith(d->tagShortcutPrefix))
        {
            ltw->toggleTag(val);
        }

        return;
    }

    // Emit signal to DInfoInterface to broadcast to another component:

    Q_EMIT signalShortcutPressed(action->objectName(), val);
}

} // namespace Digikam

#include "moc_tagsactionmngr.cpp"
