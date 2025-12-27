/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-21-12
 * Description : a embedded view to show item preview widget.
 *
 * SPDX-FileCopyrightText: 2006-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itempreviewvideo.h"

// Qt includes

#include <QApplication>
#include <QGraphicsSceneContextMenuEvent>
#include <QMouseEvent>
#include <QToolBar>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QLayout>

// KDE includes

#include <kactioncollection.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "digikam_globals.h"
#include "applicationsettings.h"
#include "dbinfoiface.h"
#include "ratingwidget.h"
#include "colorlabelwidget.h"
#include "picklabelwidget.h"
#include "fileactionmngr.h"
#include "contextmenuhelper.h"
#include "itempreviewosd.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemPreviewVideo::Private
{
public:

    Private() = default;

public:

    RatingWidget*          ratingWidget         = nullptr;
    ColorLabelSelector*    clWidget             = nullptr;
    PickLabelSelector*     plWidget             = nullptr;

    ItemPreviewOsd*        osd                  = nullptr;

    ItemInfo               info;
};

ItemPreviewVideo::ItemPreviewVideo(QWidget* const parent)
    : MediaPlayerView(parent),
      d               (new Private)
{
    setObjectName(QLatin1String("main_media_player"));
    setInfoInterface(new DBInfoIface(this, QList<QUrl>()));
    setContextMenuPolicy(Qt::CustomContextMenu);

    QString btnStyleSheet       = QLatin1String("%1 { padding: 1px; background-color: "
                                                "  qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                                                "  stop: 0 rgba(100, 100, 100, 70%), "
                                                "  stop: 1 rgba(170, 170, 170, 70%)); "
                                                "border: 1px solid rgba(170, 170, 170, 10%); } ");

    DHBox* const labelsBox      = new DHBox(this);
    labelsBox->setStyleSheet(btnStyleSheet.arg(QLatin1String("QFrame")));

    d->clWidget                 = new ColorLabelSelector(labelsBox);
    d->clWidget->setStyleSheet(btnStyleSheet.arg(QLatin1String("QPushButton")));
    d->clWidget->setFocusPolicy(Qt::NoFocus);

    d->plWidget                 = new PickLabelSelector(labelsBox);
    d->plWidget->setStyleSheet(btnStyleSheet.arg(QLatin1String("QPushButton")));
    d->plWidget->setFocusPolicy(Qt::NoFocus);

    d->ratingWidget             = new RatingWidget(labelsBox);
    d->ratingWidget->setTracking(false);
    d->ratingWidget->setFading(false);
    d->ratingWidget->setFocusPolicy(Qt::NoFocus);
    labelsBox->layout()->setAlignment(d->ratingWidget, Qt::AlignVCenter | Qt::AlignLeft);

    setToolbarExtraWidget(labelsBox);

    d->osd                      = new ItemPreviewOsd(this);

    setOsdWidget(d->osd);

    // ---

    connect(d->ratingWidget, SIGNAL(signalRatingChanged(int)),
            this, SLOT(slotAssignRating(int)));

    connect(d->clWidget, SIGNAL(signalColorLabelChanged(int)),
            this, SLOT(slotAssignColorLabel(int)));

    connect(d->plWidget, SIGNAL(signalPickLabelChanged(int)),
            this, SLOT(slotAssignPickLabel(int)));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotContextMenu()));

    connect(ApplicationSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSetupChanged()));

    slotSetupChanged();
}

ItemPreviewVideo::~ItemPreviewVideo()
{
    delete d;
}

void ItemPreviewVideo::slotContextMenu()
{
    if (d->info.isNull())
    {
        return;
    }

    QList<qlonglong> idList;
    idList << d->info.id();

    QMenu popmenu(this);
    ContextMenuHelper cmHelper(&popmenu);

    cmHelper.addAction(QLatin1String("full_screen"));
    cmHelper.addAction(QLatin1String("options_show_menubar"));
    cmHelper.addSeparator();

    // --------------------------------------------------------

    for (auto* const ac : actionsList())
    {
        cmHelper.addAction(ac, true);
    }

    cmHelper.addSeparator();

    // --------------------------------------------------------

    cmHelper.addOpenAndNavigateActions(idList, true);
    cmHelper.addSeparator();

    // --------------------------------------------------------

    cmHelper.addStandardActionItemDelete(this, SLOT(slotDeleteItem()));
    cmHelper.addSeparator();

    // --------------------------------------------------------

    cmHelper.addAssignTagsMenu(idList);
    cmHelper.addRemoveTagsMenu(idList);
    cmHelper.addRemoveAllTags(idList);
    cmHelper.addLabelsAction();

    // --------------------------------------------------------

    connect(&cmHelper, SIGNAL(signalAssignTag(int)),
            this, SLOT(slotAssignTag(int)));

    connect(&cmHelper, SIGNAL(signalPopupTagsView()),
            this, SIGNAL(signalPopupTagsView()));

    connect(&cmHelper, SIGNAL(signalRemoveTag(int)),
            this, SLOT(slotRemoveTag(int)));

    connect(&cmHelper, SIGNAL(signalAssignPickLabel(int)),
            this, SLOT(slotAssignPickLabel(int)));

    connect(&cmHelper, SIGNAL(signalAssignColorLabel(int)),
            this, SLOT(slotAssignColorLabel(int)));

    connect(&cmHelper, SIGNAL(signalAssignRating(int)),
            this, SLOT(slotAssignRating(int)));

    // --------------------------------------------------------

    cmHelper.exec(QCursor::pos());
}

void ItemPreviewVideo::setItemInfo(const ItemInfo& info, const ItemInfo& previous, const ItemInfo& next)
{
    d->info = info;
    setCurrentItem(info.fileUrl(), !previous.isNull(), !next.isNull());
    d->osd->setItemInfo(info);
    d->osd->setVisible(ApplicationSettings::instance()->getPreviewOverlay());

    d->clWidget->blockSignals(true);
    d->plWidget->blockSignals(true);
    d->ratingWidget->blockSignals(true);

    d->clWidget->setColorLabel((ColorLabel)info.colorLabel());
    d->plWidget->setPickLabel((PickLabel)info.pickLabel());
    d->ratingWidget->setRating(info.rating());

    d->clWidget->blockSignals(false);
    d->plWidget->blockSignals(false);
    d->ratingWidget->blockSignals(false);
}

void ItemPreviewVideo::slotDeleteItem()
{
    Q_EMIT signalDeleteItem();
}

void ItemPreviewVideo::slotAssignTag(int tagID)
{
    FileActionMngr::instance()->assignTag(d->info, tagID);
}

void ItemPreviewVideo::slotRemoveTag(int tagID)
{
    FileActionMngr::instance()->removeTag(d->info, tagID);
}

void ItemPreviewVideo::slotAssignPickLabel(int pickId)
{
    FileActionMngr::instance()->assignPickLabel(d->info, pickId);
}

void ItemPreviewVideo::slotAssignColorLabel(int colorId)
{
    FileActionMngr::instance()->assignColorLabel(d->info, colorId);
}

void ItemPreviewVideo::slotAssignRating(int rating)
{
    FileActionMngr::instance()->assignRating(d->info, rating);
}

void ItemPreviewVideo::slotSetupChanged()
{
    setToolbarVisible(ApplicationSettings::instance()->getPreviewShowIcons());
    d->osd->setVisible(ApplicationSettings::instance()->getPreviewOverlay());
}

} // namespace Digikam

#include "moc_itempreviewvideo.cpp"
