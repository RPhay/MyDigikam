/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-21-12
 * Description : a embedded view to show item preview widget.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2011 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itempreviewview.h"

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

// KDE includes

#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <kactioncollection.h>
#include <klocalizedstring.h>

// Restore warnings
#if !defined(Q_OS_DARWIN) && defined(Q_CC_GNU)
#   pragma GCC diagnostic pop
#endif

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "digikam_globals.h"
#include "itempreviewcanvas.h"
#include "applicationsettings.h"
#include "contextmenuhelper.h"
#include "ddragobjects.h"
#include "digikamapp.h"
#include "dimg.h"
#include "dimgpreviewitem.h"
#include "fileactionmngr.h"
#include "metaenginesettings.h"
#include "regionframeitem.h"
#include "tagspopupmenu.h"
#include "thememanager.h"
#include "previewlayout.h"
#include "previewsettings.h"
#include "tagscache.h"
#include "itemtagpair.h"
#include "albummanager.h"
#include "facegroup.h"
#include "focuspointgroup.h"
#include "faceutils.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemPreviewView::Private
{
public:

    Private() = default;

    bool                   fullSize             = false;
    double                 scale                = 1.0;
    bool                   isValid              = false;
    bool                   rotationLock         = false;

    ItemPreviewView::Mode  mode                 = ItemPreviewView::IconViewPreview;

    ItemPreviewCanvas*     item                 = nullptr;

    QAction*               prevAction           = nullptr;
    QAction*               nextAction           = nullptr;
    QAction*               rotLeftAction        = nullptr;
    QAction*               rotRightAction       = nullptr;

    QToolBar*              toolBar              = nullptr;

    FaceGroup*             faceGroup            = nullptr;
    QAction*               peopleToggleAction   = nullptr;
    QAction*               addPersonAction      = nullptr;
    QAction*               forgetFacesAction    = nullptr;
    QAction*               markAsIgnoredAction  = nullptr;

    FocusPointGroup*       focusPointGroup      = nullptr;
    QAction*               addFocusPointAction  = nullptr;
    QAction*               showFocusPointAction = nullptr;

    QAction*               fullscreenAction     = nullptr;

    Album*                 currAlbum            = nullptr;
};

ItemPreviewView::ItemPreviewView(QWidget* const parent, Mode mode, Album* const currAlbum)
    : GraphicsDImgView(parent),
      d               (new Private())
{
    d->mode      = mode;
    d->item      = new ItemPreviewCanvas();
    d->currAlbum = currAlbum;
    setItem(d->item);

    d->faceGroup = new FaceGroup(this);
    d->faceGroup->setShowOnHover(true);
    d->item->setFaceGroup(d->faceGroup);

    d->focusPointGroup = new FocusPointGroup(this);

    connect(d->item, SIGNAL(loaded()),
            this, SLOT(imageLoaded()));

    connect(d->item, SIGNAL(loadingFailed()),
            this, SLOT(imageLoadingFailed()));

    connect(d->item, SIGNAL(imageChanged()),
            this, SLOT(slotUpdateFaces()));

    connect(d->item, SIGNAL(showContextMenu(QGraphicsSceneContextMenuEvent*)),
            this, SLOT(slotShowContextMenu(QGraphicsSceneContextMenuEvent*)));

    // set default zoom

    layout()->fitToWindow();

    // ------------------------------------------------------------

    installPanIcon();

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ------------------------------------------------------------

    KActionCollection* const ac = DigikamApp::instance()->actionCollection();

    d->prevAction               = new QAction(QIcon::fromTheme(QLatin1String("go-previous")),
                                              i18nc("go to previous image", "Back"),  this);
    d->nextAction               = new QAction(QIcon::fromTheme(QLatin1String("go-next")),
                                              i18nc("go to next image", "Forward"),   this);
    d->rotLeftAction            = new QAction(QIcon::fromTheme(QLatin1String("object-rotate-left")),
                                              i18nc("@info:tooltip", "Rotate Left"),  this);
    d->rotRightAction           = new QAction(QIcon::fromTheme(QLatin1String("object-rotate-right")),
                                              i18nc("@info:tooltip", "Rotate Right"), this);

    d->addPersonAction          = ac->action(QLatin1String("add_face_tag_manually"));

    if (!d->addPersonAction)
    {
        d->addPersonAction      = new QAction(QIcon::fromTheme(QLatin1String("list-add-user")),
                                              i18n("Add a Face Tag"),                 this);
        ac->addAction(QLatin1String("add_face_tag_manually"), d->addPersonAction);
    }

    d->forgetFacesAction        = new QAction(QIcon::fromTheme(QLatin1String("list-remove-user")),
                                              i18n("Clear all faces on this image"),  this);

    d->markAsIgnoredAction      = new QAction(QIcon::fromTheme(QLatin1String("dialog-cancel")),
                                              i18n("Mark all unconfirmed faces as ignored"),  this);

    d->peopleToggleAction       = ac->action(QLatin1String("toggle_show_face_tags"));

    if (!d->peopleToggleAction)
    {
        d->peopleToggleAction   = new QAction(QIcon::fromTheme(QLatin1String("im-user")),
                                              i18n("Show Face Tags"),                 this);
        ac->addAction(QLatin1String("toggle_show_face_tags"), d->peopleToggleAction);
    }

    d->addFocusPointAction      = new QAction(QIcon::fromTheme(QLatin1String("list-add-user")),
                                              i18n("Add a focus point"),              this);
    d->showFocusPointAction     = new QAction(QIcon::fromTheme(QLatin1String("im-user")),
                                              i18n("Show focus points"),              this);

    d->peopleToggleAction->setCheckable(true);
    d->showFocusPointAction->setCheckable(true);

    d->fullscreenAction         = new QAction(QIcon::fromTheme(QLatin1String("media-playback-start")),
                                              i18n("Show Fullscreen"),                this);
    d->toolBar                  = new QToolBar(this);
    d->toolBar->setStyleSheet(toolButtonStyleSheet());

    if (mode == IconViewPreview)
    {
        d->toolBar->addAction(d->prevAction);
        d->toolBar->addAction(d->nextAction);
    }

    d->toolBar->addAction(d->rotLeftAction);
    d->toolBar->addAction(d->rotRightAction);

    d->toolBar->addAction(d->peopleToggleAction);
    d->toolBar->addAction(d->addPersonAction);
    d->toolBar->addAction(d->fullscreenAction);

    connect(d->prevAction, SIGNAL(triggered()),
            this, SIGNAL(toPreviousImage()));

    connect(d->nextAction, SIGNAL(triggered()),
            this, SIGNAL(toNextImage()));

    connect(d->rotLeftAction, SIGNAL(triggered()),
            this, SLOT(slotRotateLeft()));

    connect(d->rotRightAction, SIGNAL(triggered()),
            this, SLOT(slotRotateRight()));

    connect(d->peopleToggleAction, SIGNAL(toggled(bool)),
            d->faceGroup, SLOT(setVisible(bool)));

    connect(d->addPersonAction, &QAction::triggered,
            this, [this]()
        {
            if (isVisible() && hasFocus())
            {
                d->faceGroup->addFace();
            }
        }
    );

    connect(d->forgetFacesAction, SIGNAL(triggered()),
            d->faceGroup, SLOT(rejectAll()));

    connect(d->markAsIgnoredAction, SIGNAL(triggered()),
            d->faceGroup, SLOT(markAllAsIgnored()));

    connect(d->addFocusPointAction, SIGNAL(triggered()),
            d->focusPointGroup, SLOT(addPoint()));

    connect(d->showFocusPointAction, &QAction::toggled,
            this, [this](bool checked)
        {
            bool add = false;

            if (checked)
            {
                d->focusPointGroup->setInfo(d->item->imageInfo());
                add = d->focusPointGroup->isAllowedToAddFocusPoint();
            }
            else
            {
                d->focusPointGroup->setInfo(ItemInfo());
            }

            d->focusPointGroup->setVisible(checked);
            d->addFocusPointAction->setEnabled(add);
        }
    );

    connect(d->fullscreenAction, SIGNAL(triggered()),
            this, SLOT(slotSlideShowCurrent()));

    // ------------------------------------------------------------

    connect(this, SIGNAL(toNextImage()),
            this, SIGNAL(signalNextItem()));

    connect(this, SIGNAL(toPreviousImage()),
            this, SIGNAL(signalPrevItem()));

    connect(this, SIGNAL(activated()),
            this, SIGNAL(signalEscapePreview()));

    connect(ThemeManager::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));

    connect(ApplicationSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSetupChanged()));

    slotSetupChanged();
}

ItemPreviewView::~ItemPreviewView()
{
    delete d->item;
    delete d->focusPointGroup;
    delete d;
}

void ItemPreviewView::reload()
{
    previewItem()->reload();
}

void ItemPreviewView::imageLoaded()
{
    Q_EMIT signalPreviewLoaded(true);

    d->rotLeftAction->setEnabled(true);
    d->rotRightAction->setEnabled(true);

    d->faceGroup->setInfo(d->item->imageInfo());

    bool add = false;

    if (d->showFocusPointAction->isChecked())
    {
        d->focusPointGroup->setInfo(d->item->imageInfo());
        add = d->focusPointGroup->isAllowedToAddFocusPoint();
    }
    else
    {
        d->focusPointGroup->setInfo(ItemInfo());
    }

    d->addFocusPointAction->setEnabled(add);
}

void ItemPreviewView::imageLoadingFailed()
{
    Q_EMIT signalPreviewLoaded(false);

    d->rotLeftAction->setEnabled(false);
    d->rotRightAction->setEnabled(false);
    d->addFocusPointAction->setEnabled(false);

    d->faceGroup->setInfo(ItemInfo());
    d->focusPointGroup->setInfo(ItemInfo());

}

void ItemPreviewView::setItemInfo(const ItemInfo& info, const ItemInfo& previous, const ItemInfo& next)
{
    d->faceGroup->aboutToSetInfo(info);
    d->item->setItemInfo(info);

    d->prevAction->setEnabled(!previous.isNull());
    d->nextAction->setEnabled(!next.isNull());

    QStringList previewPaths;

    if (next.category() == DatabaseItem::Image)
    {
        previewPaths << next.filePath();
    }

    if (previous.category() == DatabaseItem::Image)
    {
        previewPaths << previous.filePath();
    }

    d->item->setPreloadPaths(previewPaths);
}

ItemInfo ItemPreviewView::getItemInfo() const
{
    return d->item->imageInfo();
}

bool ItemPreviewView::acceptsMouseClick(QMouseEvent* e)
{
    if (!GraphicsDImgView::acceptsMouseClick(e))
    {
        return false;
    }

    return d->faceGroup->acceptsMouseClick(mapToScene(e->pos()));
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

void ItemPreviewView::enterEvent(QEnterEvent* e)

#else

void ItemPreviewView::enterEvent(QEvent* e)

#endif

{
    d->faceGroup->enterEvent(e);
}

void ItemPreviewView::leaveEvent(QEvent* e)
{
    d->faceGroup->leaveEvent(e);
}

void ItemPreviewView::showEvent(QShowEvent* e)
{
    GraphicsDImgView::showEvent(e);
    d->faceGroup->setVisible(d->peopleToggleAction->isChecked());
    d->focusPointGroup->setVisible(d->showFocusPointAction->isChecked());
}

void ItemPreviewView::slotShowContextMenu(QGraphicsSceneContextMenuEvent* event)
{
    ItemInfo info = d->item->imageInfo();

    if (info.isNull())
    {
        return;
    }

    event->accept();

    QList<qlonglong> idList;
    idList << info.id();

    // --------------------------------------------------------

    QMenu popmenu(this);
    ContextMenuHelper cmHelper(&popmenu);

    cmHelper.addAction(QLatin1String("full_screen"));
    cmHelper.addAction(QLatin1String("options_show_menubar"));
    cmHelper.addSeparator();

    // --------------------------------------------------------

    if (d->mode == IconViewPreview)
    {
        cmHelper.addAction(d->prevAction, true);
        cmHelper.addAction(d->nextAction, true);
        cmHelper.addSeparator();
    }

    // --------------------------------------------------------

    cmHelper.addAction(d->peopleToggleAction,  true);
    cmHelper.addAction(QLatin1String("image_scan_for_faces"));
    cmHelper.addAction(d->addPersonAction,     true);

    // if there is a face in the image, give the option to remove all faces

    if (d->faceGroup->items().length() > 0)
    {
        cmHelper.addAction(d->forgetFacesAction,   true);
    }

    // if there is at least one unconfirmed face

    if (d->faceGroup->hasUnconfirmed())
    {
        // give the option to ignore unconfirmed faces

        cmHelper.addAction(d->markAsIgnoredAction, true);

        // give the option to recognize faces

        cmHelper.addAction(QLatin1String("image_recognize_faces"));
    }
    cmHelper.addSeparator();

    // -------------------------------------------------------

    cmHelper.addAction(d->addFocusPointAction,  true);
    cmHelper.addAction(d->showFocusPointAction, true);
    cmHelper.addSeparator();

    // --------------------------------------------------------

    cmHelper.addOpenAndNavigateActions(idList, (d->mode == LightTablePreview));
    cmHelper.addSeparator();

    // --------------------------------------------------------

    cmHelper.addAction(QLatin1String("image_find_similar"));

    if (d->mode == IconViewPreview)
    {
        cmHelper.addStandardActionLightTable();
    }

    cmHelper.addQueueManagerMenu();
    cmHelper.addSeparator();

    // --------------------------------------------------------

    cmHelper.addAction(QLatin1String("image_rotate"));
    cmHelper.addStandardActionItemDelete(this, SLOT(slotDeleteItem()));
    cmHelper.addSeparator();

    // --------------------------------------------------------

    if (d->mode == IconViewPreview && d->currAlbum)
    {
        cmHelper.addStandardActionThumbnail(idList, d->currAlbum);
    }

    cmHelper.addAssignTagsMenu(idList);
    cmHelper.addRemoveTagsMenu(idList);
    cmHelper.addRemoveAllTags(idList);
    cmHelper.addLabelsAction();

    // special action handling --------------------------------

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

    connect(&cmHelper, SIGNAL(signalAddToExistingQueue(int)),
            this, SIGNAL(signalAddToExistingQueue(int)));

    connect(&cmHelper, SIGNAL(signalGotoTag(int)),
            this, SIGNAL(signalGotoTagAndItem(int)));

    connect(&cmHelper, SIGNAL(signalGotoAlbum(ItemInfo)),
            this, SIGNAL(signalGotoAlbumAndItem(ItemInfo)));

    connect(&cmHelper, SIGNAL(signalGotoDate(ItemInfo)),
            this, SIGNAL(signalGotoDateAndItem(ItemInfo)));

    cmHelper.exec(event->screenPos());
}

void ItemPreviewView::slotSlideShowCurrent()
{
    QList<DPluginAction*> actions = DPluginLoader::instance()->
                                        pluginActions(QLatin1String("org.kde.digikam.plugin.generic.SlideShow"),
                                        DigikamApp::instance());

    if (actions.isEmpty())
    {
        return;
    }

    //Trigger SlideShow manual

    actions[0]->setData(getItemInfo().fileUrl());

    actions[0]->trigger();
}

void ItemPreviewView::slotAssignTag(int tagID)
{
    FileActionMngr::instance()->assignTag(d->item->imageInfo(), tagID);
}

void ItemPreviewView::slotRemoveTag(int tagID)
{
    FileActionMngr::instance()->removeTag(d->item->imageInfo(), tagID);
}

void ItemPreviewView::slotAssignPickLabel(int pickId)
{
    FileActionMngr::instance()->assignPickLabel(d->item->imageInfo(), pickId);
}

void ItemPreviewView::slotAssignColorLabel(int colorId)
{
    FileActionMngr::instance()->assignColorLabel(d->item->imageInfo(), colorId);
}

void ItemPreviewView::slotAssignRating(int rating)
{
    FileActionMngr::instance()->assignRating(d->item->imageInfo(), rating);
}

void ItemPreviewView::slotThemeChanged()
{
    QPalette plt(palette());
    plt.setColor(backgroundRole(), qApp->palette().color(QPalette::Base));
    setPalette(plt);
}

void ItemPreviewView::slotSetupChanged()
{
    previewItem()->setPreviewSettings(ApplicationSettings::instance()->getPreviewSettings());

    d->toolBar->setVisible(ApplicationSettings::instance()->getPreviewShowIcons());
    setScaleFitToWindow(ApplicationSettings::instance()->getScaleFitToWindow());
    setShowText(ApplicationSettings::instance()->getPreviewShowIcons());

    layout()->updateZoomAndSize();

    // pass auto-suggest?
}

void ItemPreviewView::slotRotateLeft()
{
    if (d->rotationLock)
    {
        return;
    }

    d->rotationLock = true;

    /**
     * Setting lock won't allow mouse hover events in ItemPreviewCanvas class
     */
    d->item->setAcceptHoverEvents(false);

    /**
     * aboutToSetInfo will delete all face tags from FaceGroup storage
     */
    d->faceGroup->aboutToSetInfo(ItemInfo());

    FileActionMngr::instance()->transform(QList<ItemInfo>() << d->item->imageInfo(), MetaEngineRotation::Rotate270);

    // Set the newly rotated image as not processed by the face recognition pipeline
    FaceUtils utils;
    utils.markAsScanned(d->item->imageInfo(), false);
}

void ItemPreviewView::slotRotateRight()
{
    if (d->rotationLock)
    {
        return;
    }

    d->rotationLock = true;

    /**
     * Setting lock won't allow mouse hover events in ItemPreviewCanvas class
     */
    d->item->setAcceptHoverEvents(false);

    /**
     * aboutToSetInfo will delete all face tags from FaceGroup storage
     */
    d->faceGroup->aboutToSetInfo(ItemInfo());

    FileActionMngr::instance()->transform(QList<ItemInfo>() << d->item->imageInfo(), MetaEngineRotation::Rotate90);

    // Set the newly rotated image as not processed by the face recognition pipeline
    FaceUtils utils;
    utils.markAsScanned(d->item->imageInfo(), false);
}

void ItemPreviewView::slotDeleteItem()
{
    Q_EMIT signalDeleteItem();
}

void Digikam::ItemPreviewView::slotUpdateFaces()
{
/*
    d->faceGroup->aboutToSetInfo(ItemInfo());
*/
    d->faceGroup->aboutToSetInfoAfterRotate(ItemInfo());
    d->item->setAcceptHoverEvents(true);

    /**
     * Release rotation lock after rotation
     */
    d->rotationLock = false;
}

void ItemPreviewView::dragMoveEvent(QDragMoveEvent* e)
{
    if (DTagListDrag::canDecode(e->mimeData()))
    {
        e->accept();
        return;
    }

    e->ignore();
}

void ItemPreviewView::dragEnterEvent(QDragEnterEvent* e)
{
    if (DTagListDrag::canDecode(e->mimeData()))
    {
        e->accept();
        return;
    }

    e->ignore();
}

void ItemPreviewView::dropEvent(QDropEvent* e)
{
    if (DTagListDrag::canDecode(e->mimeData()))
    {
        QList<int> tagIDs;

        if (!DTagListDrag::decode(e->mimeData(), tagIDs))
        {
            return;
        }

        QMenu popMenu(this);
        QAction* const assignToThisAction = popMenu.addAction(QIcon::fromTheme(QLatin1String("tag")), i18n("Assign Tags to &This Item"));
        popMenu.addSeparator();
        popMenu.addAction(QIcon::fromTheme(QLatin1String("dialog-cancel")), i18n("&Cancel"));
        popMenu.setMouseTracking(true);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        QAction* const choice             = popMenu.exec(this->mapToGlobal(e->position().toPoint()));

#else

        QAction* const choice             = popMenu.exec(this->mapToGlobal(e->pos()));

#endif

        if (choice == assignToThisAction)
        {
            FileActionMngr::instance()->assignTags(d->item->imageInfo(), tagIDs);
        }
    }

    e->accept();

    return;
}

void ItemPreviewView::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::LeftButton) && (QApplication::keyboardModifiers() == Qt::ControlModifier))
    {
        d->faceGroup->addFace();
    }

    GraphicsDImgView::mousePressEvent(e);
}

} // namespace Digikam

#include "moc_itempreviewview.cpp"
