/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 20013-07-03
 * Description : Tag Manager main class
 *
 * Copyright (C) 2013 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 * Copyright (C) 2014 by Michael G. Hansen <mike at mghansen dot de>
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

#include "tagsmanager.h"

// Qt includes

#include <QQueue>
#include <QTreeView>
#include <QLabel>
#include <QHBoxLayout>
#include <QSplitter>
#include <QApplication>
#include <QToolBar>
#include <QScreen>
#include <QWindow>
#include <QAction>
#include <QMessageBox>
#include <QMenu>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_config.h"
#include "dmessagebox.h"
#include "tagpropwidget.h"
#include "tagmngrtreeview.h"
#include "taglist.h"
#include "tagfolderview.h"
#include "ddragobjects.h"
#include "searchtextbardb.h"
#include "tageditdlg.h"
#include "coredb.h"
#include "facetags.h"
#include "dlogoaction.h"
#include "metadatasynchronizer.h"
#include "fileactionmngr.h"
#include "metaenginesettings.h"

namespace
{

QString JoinTagNamesToList(const QStringList& stringList)
{
    const QString joinedStringList = stringList.join(QLatin1String("', '"));

    return QLatin1Char('\'') + joinedStringList + QLatin1Char('\'');
}

} // namespace

namespace Digikam
{

QPointer<TagsManager> TagsManager::internalPtr = QPointer<TagsManager>();

class Q_DECL_HIDDEN TagsManager::Private
{
public:

    explicit Private()
      : tagMngrView     (nullptr),
        tagPixmap       (nullptr),
        searchBar       (nullptr),
        splitter        (nullptr),
        treeWindow      (nullptr),
        mainToolbar     (nullptr),
        organizeAction  (nullptr),
        syncexportAction(nullptr),
        tagProperties   (nullptr),
        addAction       (nullptr),
        delAction       (nullptr),
        titleEdit       (nullptr),
        listView        (nullptr),
        tagPropWidget   (nullptr),
        tagModel        (nullptr),
        tagPropVisible  (false)
    {
    }

    TagMngrTreeView* tagMngrView;
    QLabel*          tagPixmap;
    SearchTextBarDb* searchBar;


    QSplitter*       splitter;
    QWidget*         treeWindow;
    QToolBar*        mainToolbar;
    QMenu*           organizeAction;
    QMenu*           syncexportAction;
    QAction*         tagProperties;
    QAction*         addAction;
    QAction*         delAction;
    QAction*         titleEdit;

    /**
     * Options unavailable for root tag
     */
    QList<QAction*>  rootDisabledOptions;

    TagList*         listView;
    TagPropWidget*   tagPropWidget;
    TagModel*        tagModel;

    bool             tagPropVisible;
};

TagsManager::TagsManager()
    : QMainWindow      (nullptr),
      StateSavingObject(this),
      d                (new Private())
{
    setObjectName(QLatin1String("Tags Manager"));
    d->tagModel = new TagModel(AbstractAlbumModel::IncludeRootAlbum, this);
    d->tagModel->setCheckable(false);
    setupUi();

    /*----------------------------Connects---------------------------*/

    connect(d->tagMngrView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotSelectionChanged()));

    connect(d->addAction, SIGNAL(triggered()),
            this, SLOT(slotAddAction()));

    connect(d->delAction, SIGNAL(triggered()),
            this, SLOT(slotDeleteAction()));

    d->tagMngrView->setCurrentIndex(d->tagMngrView->model()->index(0, 0));

    StateSavingObject::loadState();
}

TagsManager::~TagsManager()
{
    StateSavingObject::saveState();

    delete d;
}

TagsManager* TagsManager::instance()
{
    if (TagsManager::internalPtr.isNull())
    {
        TagsManager::internalPtr = new TagsManager();
    }

    return TagsManager::internalPtr;
}

void TagsManager::setupUi()
{
     resize(970, 720);
     setWindowTitle(i18n("Tags Manager"));

     d->tagPixmap   = new QLabel();
     d->tagPixmap->setText(QLatin1String("Tag Pixmap"));
     d->tagPixmap->setMaximumWidth(40);
     d->tagPixmap->setPixmap(QIcon::fromTheme(QLatin1String("tag")).pixmap(30, 30));

     d->tagMngrView = new TagMngrTreeView(this, d->tagModel);
     d->tagMngrView->setConfigGroup(getConfigGroup());

     d->searchBar   = new SearchTextBarDb(this, QLatin1String("ItemIconViewTagSearchBar"));
     d->searchBar->setHighlightOnResult(true);
     d->searchBar->setModel(d->tagMngrView->filteredModel(),
                            AbstractAlbumModel::AlbumIdRole,
                            AbstractAlbumModel::AlbumTitleRole);
     d->searchBar->setMaximumWidth(200);
     d->searchBar->setFilterModel(d->tagMngrView->albumFilterModel());

     setupActions();

     // Tree Widget + Actions + Tag Properties

     d->tagPropWidget = new TagPropWidget(this);
     d->listView      = new TagList(d->tagMngrView, this);

     d->splitter      = new QSplitter(Qt::Horizontal, this);
     d->splitter->addWidget(d->listView);
     d->splitter->addWidget(d->tagMngrView);
     d->splitter->addWidget(d->tagPropWidget);

     connect(d->tagPropWidget, SIGNAL(signalTitleEditReady()),
             this, SLOT(slotTitleEditReady()));

     d->splitter->setStretchFactor(0, 0);
     d->splitter->setStretchFactor(1, 1);
     d->splitter->setStretchFactor(2, 0);

     QWidget* const centralView    = new QWidget(this);
     QHBoxLayout* const mainLayout = new QHBoxLayout(centralView);
     mainLayout->addWidget(d->splitter);
     centralView->setLayout(mainLayout);
     setCentralWidget(centralView);
}

void TagsManager::slotSelectionChanged()
{
    QList<Album*> selectedTags = d->tagMngrView->selectedTags();

    if (selectedTags.isEmpty() || ((selectedTags.size() == 1) && selectedTags.at(0)->isRoot()))
    {
        enableRootTagActions(false);
        d->listView->enableAddButton(false);
    }
    else
    {
        enableRootTagActions(true);
        d->listView->enableAddButton(true);
        d->titleEdit->setEnabled((selectedTags.size() == 1));
    }

    d->tagPropWidget->slotSelectionChanged(selectedTags);
}

void TagsManager::slotItemChanged()
{
}

void TagsManager::slotAddAction()
{
    TAlbum*      parent = d->tagMngrView->currentAlbum();
    QString      title;
    QString      icon;
    QKeySequence ks;

    if (!parent)
    {
        parent = static_cast<TAlbum*>(d->tagMngrView->albumForIndex(d->tagMngrView->model()->index(0, 0)));
    }

    if (!TagEditDlg::tagCreate(qApp->activeWindow(), parent, title, icon, ks))
    {
        return;
    }

    QMap<QString, QString> errMap;
    AlbumList tList = TagEditDlg::createTAlbum(parent, title, icon, ks, errMap);
    TagEditDlg::showtagsListCreationError(qApp->activeWindow(), errMap);
}

void TagsManager::slotDeleteAction()
{
    const QModelIndexList selected = d->tagMngrView->selectionModel()->selectedIndexes();

    QStringList tagNames;
    QStringList tagsWithChildren;
    QStringList tagsWithImages;
    QMultiMap<int, TAlbum*> sortedTags;

    foreach (const QModelIndex& index, selected)
    {
        if (!index.isValid())
        {
            return;
        }

        TAlbum* const t = static_cast<TAlbum*>(d->tagMngrView->albumForIndex(index));

        if (!t || t->isRoot())
        {
            return;
        }

        AlbumPointer<TAlbum> tag(t);
        tagNames.append(tag->title());

        // find number of subtags

        int children = 0;
        AlbumIterator iter(tag);

        while (iter.current())
        {
            ++children;
            ++iter;
        }

        if (children)
        {
            tagsWithChildren.append(tag->title());
        }

        QList<qlonglong> assignedItems = CoreDbAccess().db()->getItemIDsInTag(tag->id());

        if (!assignedItems.isEmpty())
        {
            tagsWithImages.append(tag->title());
        }

        /**
         * Tags must be deleted from children to parents, if we don't want
         * to step on invalid index. Use QMultiMap to order them by distance
         * to root tag
         */
        Album* parent = t;
        int depth     = 0;

        while (!parent->isRoot())
        {
            parent = parent->parent();
            depth++;
        }

        sortedTags.insert(depth, tag);
    }

    // ask for deletion of children

    if (!tagsWithChildren.isEmpty())
    {
        const int result = QMessageBox::warning(this, qApp->applicationName(),
                                                i18ncp("%2 is a comma separated list of tags to be deleted.",
                                                       "Tag %2 has one or more subtags. "
                                                       "Deleting it will also delete "
                                                       "the subtags. "
                                                       "Do you want to continue?",
                                                       "Tags %2 have one or more subtags. "
                                                       "Deleting them will also delete "
                                                       "the subtags. "
                                                       "Do you want to continue?",
                                                       tagsWithChildren.count(),
                                                       JoinTagNamesToList(tagsWithChildren)),
                                                QMessageBox::Yes | QMessageBox::Cancel);

        if (result != QMessageBox::Yes)
        {
            return;
        }
    }

    QString message;

    if (!tagsWithImages.isEmpty())
    {
        message = i18ncp("%2 is a comma separated list of tags to be deleted.",
                         "Tag %2 is assigned to one or more items. "
                         "Do you want to delete it?",
                         "Tags %2 are assigned to one or more items. "
                         "Do you want to delete them?",
                         tagsWithImages.count(),
                         JoinTagNamesToList(tagsWithImages));
    }
    else
    {
        message = i18ncp("%2 is a comma separated list of tags to be deleted.",
                         "Delete tag %2?",
                         "Delete tags %2?",
                         tagNames.count(),
                         JoinTagNamesToList(tagNames));
    }

    const int result = QMessageBox::warning(this, i18np("Delete tag", "Delete tags", tagNames.count()),
                                            message, QMessageBox::Yes | QMessageBox::Cancel);

    if (result == QMessageBox::Yes)
    {
        QMultiMap<int, TAlbum*>::iterator it;

        /**
         * QMultimap doesn't provide reverse iterator, -1 is required
         * because end() points after the last element
         */
        for (it = sortedTags.end()-1 ; it != sortedTags.begin()-1 ; --it)
        {
            QString errMsg;

            if (!AlbumManager::instance()->deleteTAlbum(it.value(), errMsg))
            {
                QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(), errMsg);
            }
        }
    }
}

void TagsManager::slotEditTagTitle()
{
    QList<Album*> selectedTags = d->tagMngrView->selectedTags();

    if ((selectedTags.size() == 1) && !selectedTags.at(0)->isRoot())
    {
        d->tagPropWidget->show();
        d->tagPropWidget->slotFocusTitleEdit();
    }
}

void TagsManager::slotTitleEditReady()
{
    if (!d->tagPropVisible)
    {
        d->tagPropWidget->hide();
    }

    d->tagMngrView->setFocus();
}

void TagsManager::slotResetTagIcon()
{
    QString errMsg;

    const QList<TAlbum*> selected = d->tagMngrView->selectedTagAlbums();

    for (QList<TAlbum*>::const_iterator it = selected.constBegin() ; it != selected.constEnd() ; ++it)
    {
        TAlbum* const tag = *it;

        if (tag)
        {
            if (!AlbumManager::instance()->updateTAlbumIcon(tag, tag->standardIconName(), 0, errMsg))
            {
                QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(), errMsg);
            }
        }
    }
}

void TagsManager::slotCreateTagAddr()
{
}

void TagsManager::slotInvertSel()
{
    QModelIndex root                 = d->tagMngrView->model()->index(0, 0);
    QItemSelectionModel* const model = d->tagMngrView->selectionModel();
    QModelIndexList selected         = model->selectedIndexes();

    QQueue<QModelIndex> greyNodes;
    bool currentSet = false;

    greyNodes.append(root);

    model->clearSelection();

    while (!greyNodes.isEmpty())
    {
        QModelIndex current = greyNodes.dequeue();

        if (!(current.isValid()))
        {
            continue;
        }

        int it            = 0;
        QModelIndex child = current.model()->index(it++, 0, current);

        while (child.isValid())
        {
            if (!selected.contains(child))
            {
                if (!currentSet)
                {
                    /**
                     * Must set a new current item when inverting selection
                     * it should be done only once
                     */
                    d->tagMngrView->setCurrentIndex(child);
                    currentSet = true;
                }

                model->select(child, model->Select);
            }

            if (d->tagMngrView->isExpanded(child))
            {
                greyNodes.enqueue(child);
            }

            child = current.model()->index(it++, 0, current);
        }
    }
}

void TagsManager::slotWriteToImg()
{
    int result = QMessageBox::warning(this, qApp->applicationName(),
                                      i18n("<qt>digiKam will clean up tag metadata before setting "
                                           "tags from database.<br/> You may <b>lose tags</b> if you did not "
                                           "read tags before (by calling Read Tags from Image).<br/> "
                                           "Do you want to continue?</qt>"),
                                      QMessageBox::Yes | QMessageBox::Cancel);

    if (result != QMessageBox::Yes)
    {
        return;
    }

    result = QMessageBox::warning(this, qApp->applicationName(),
                                  i18n("This operation can take long time "
                                       "depending on collection size.\n"
                                       "Do you want to continue?"),
                                  QMessageBox::Yes | QMessageBox::Cancel);

    if (result != QMessageBox::Yes)
    {
        return;
    }

    MetadataSynchronizer* const tool = new MetadataSynchronizer(AlbumList(),
                                                                MetadataSynchronizer::WriteFromDatabaseToFile);
    tool->setTagsOnly(true);
    tool->start();
}

void TagsManager::slotReadFromImg()
{
    int result = QMessageBox::warning(this, qApp->applicationName(),
                                      i18n("This operation can take long time "
                                           "depending on collection size.\n"
                                           "Do you want to continue?"),
                                      QMessageBox::Yes | QMessageBox::Cancel);

    if (result != QMessageBox::Yes)
    {
        return;
    }

    MetadataSynchronizer* const tool = new MetadataSynchronizer(AlbumList(),
                                                                MetadataSynchronizer::ReadFromFileToDatabase);
    tool->setUseMultiCoreCPU(false);
    tool->setTagsOnly(true);
    tool->start();
}

void TagsManager::slotWipeAll()
{
    const int result = QMessageBox::warning(this, qApp->applicationName(),
                                            i18n("This operation will wipe all tags from database only.\n"
                                                 "To apply changes to files, "
                                                 "you must choose write metadata to file later.\n"
                                                 "Do you want to continue?"),
                                            QMessageBox::Yes | QMessageBox::Cancel);

    if (result != QMessageBox::Yes)
    {
        return;
    }

    /**
     * Disable writing tags to images
     */
    MetaEngineSettings* const metaSettings      = MetaEngineSettings::instance();
    MetaEngineSettingsContainer backUpContainer = metaSettings->settings();
    MetaEngineSettingsContainer newContainer    = backUpContainer;
    bool settingsChanged                        = false;

    if ((backUpContainer.saveTags == true) || (backUpContainer.saveFaceTags == true))
    {
        settingsChanged           = true;
        newContainer.saveTags     = false;
        newContainer.saveFaceTags = false;
        metaSettings->setSettings(newContainer);
    }

    AlbumPointerList<TAlbum> tagList;
    const QModelIndex root  = d->tagMngrView->model()->index(0, 0);
    int iter                = 0;
    QModelIndex child       = root.model()->index(iter++, 0, root);

    while (child.isValid())
    {
        tagList <<  AlbumPointer<TAlbum>(d->tagMngrView->albumForIndex(child));
        child = root.model()->index(iter++, 0, root);
    }

    AlbumPointerList<TAlbum>::iterator it;

    for (it = tagList.begin() ; it != tagList.end() ; ++it)
    {
        QString errMsg;

        if (!AlbumManager::instance()->deleteTAlbum(*it, errMsg))
        {
            QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(), errMsg);
        }
    }

    /**
     * Restore settings after tag deletion
     */
    if (settingsChanged)
    {
        metaSettings->setSettings(backUpContainer);
    }
}

void TagsManager::slotRemoveTagsFromImgs()
{
    const QModelIndexList selList = d->tagMngrView->selectionModel()->selectedIndexes();

    const int result = QMessageBox::warning(this, qApp->applicationName(),
                                            i18np("Do you really want to remove the selected tag from all images?",
                                                  "Do you really want to remove the selected tags from all images?",
                                                  selList.count()),
                                            QMessageBox::Yes | QMessageBox::Cancel);

    if (result != QMessageBox::Yes)
    {
        return;
    }

    foreach (const QModelIndex& index, selList)
    {
        TAlbum* const t = static_cast<TAlbum*>(d->tagMngrView->albumForIndex(index));

        AlbumPointer<TAlbum> tag(t);

        if (tag->isRoot())
        {
            continue;
        }

        QList<qlonglong> assignedItems = CoreDbAccess().db()->getItemIDsInTag(tag->id());
        ItemInfoList imgList(assignedItems);
        FileActionMngr::instance()->removeTag(imgList, tag->id());
    }
}

void TagsManager::closeEvent(QCloseEvent* event)
{
    d->listView->saveSettings();
    QMainWindow::closeEvent(event);
}

void TagsManager::showEvent(QShowEvent* event)
{
   // Set main window in center of the screen

    QScreen* screen = qApp->primaryScreen();

    if (QWidget* const widget = qApp->activeWindow())
    {
        if (QWindow* const window = widget->windowHandle())
        {
            screen = window->screen();
        }
    }

    move(screen->geometry().center() - rect().center());

    QMainWindow::showEvent(event);
}

void TagsManager::setupActions()
{
    d->mainToolbar = new QToolBar(this);
    d->mainToolbar->setMovable(false);
    d->mainToolbar->setFloatable(false);
    d->mainToolbar->layout()->setContentsMargins(QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin),
                                                 QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin),
                                                 QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin),
                                                 QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin));

    QWidgetAction* const pixMapAction = new QWidgetAction(this);
    pixMapAction->setDefaultWidget(d->tagPixmap);

    QWidgetAction* const searchAction = new QWidgetAction(this);
    searchAction->setDefaultWidget(d->searchBar);

    d->mainToolbar->addAction(pixMapAction);
    d->mainToolbar->addAction(searchAction);

    d->mainToolbar->addSeparator();

    d->addAction                 = new QAction(QIcon::fromTheme(QLatin1String("list-add")),
                                               QLatin1String(""), this);

    d->delAction                 = new QAction(QIcon::fromTheme(QLatin1String("list-remove")),
                                               QLatin1String(""), this);

    /**
     * organize group
     */
    d->organizeAction            = new QMenu(i18nc("@title:menu", "Organize"), this);
    d->organizeAction->setIcon(QIcon::fromTheme(QLatin1String("autocorrection")));

    d->titleEdit                 = new QAction(QIcon::fromTheme(QLatin1String("document-edit")),
                                               i18n("Edit Tag Title"), this);
    d->titleEdit->setShortcut(QKeySequence(Qt::Key_F2));

    QAction* const resetIcon     = new QAction(QIcon::fromTheme(QLatin1String("view-refresh")),
                                               i18n("Reset Tag Icon"), this);

    QAction* const createTagAddr = new QAction(QIcon::fromTheme(QLatin1String("tag-addressbook")),
                                               i18n("Create Tag from Address Book"), this);

    QAction* const markUnused    = new QAction(QIcon::fromTheme(QLatin1String("edit-select")),
                                               i18n("Mark Unassigned Tags"), this);

    QAction* const invSel        = new QAction(QIcon::fromTheme(QLatin1String("tag-reset")),
                                               i18n("Invert Selection"), this);

    QAction* const expandTree    = new QAction(QIcon::fromTheme(QLatin1String("format-indent-more")),
                                               i18n("Expand Tag Tree"), this);

    QAction* const expandSel     = new QAction(QIcon::fromTheme(QLatin1String("format-indent-more")),
                                               i18n("Expand Selected Nodes"), this);
    QAction* const delTagFromImg = new QAction(QIcon::fromTheme(QLatin1String("tag-delete")),
                                               i18n("Remove Tag from Images"), this);

    /**
     * Tool tips
     */
    setHelpText(d->addAction, i18n("Add new tag to current tag. "
                                   "Current tag is last clicked tag."));

    setHelpText(d->delAction, i18n("Delete selected items. "
                                   "Also work with multiple items, "
                                   "but will not delete the root tag."));

    setHelpText(d->titleEdit, i18n("Edit title from selected tag."));

    setHelpText(resetIcon, i18n("Reset icon to selected tags. "
                                "Works with multiple selection."));

    setHelpText(markUnused, i18n("Mark all tags that are not assigned to images."));

    setHelpText(invSel, i18n("Invert selection. "
                             "Only visible items will be selected"));

    setHelpText(expandTree, i18n("Expand tag tree by one level"));

    setHelpText(expandSel, i18n("Selected items will be expanded"));

    setHelpText(delTagFromImg, i18n("Delete selected tag(s) from images. "
                                    "Works with multiple selection."));

    connect(d->titleEdit, SIGNAL(triggered()),
            this, SLOT(slotEditTagTitle()));

    connect(resetIcon, SIGNAL(triggered()),
            this, SLOT(slotResetTagIcon()));

    connect(createTagAddr, SIGNAL(triggered()),
            this, SLOT(slotCreateTagAddr()));

    connect(invSel, SIGNAL(triggered()),
            this, SLOT(slotInvertSel()));

    connect(expandTree, SIGNAL(triggered()),
            d->tagMngrView, SLOT(slotExpandTree()));

    connect(expandSel, SIGNAL(triggered()),
            d->tagMngrView, SLOT(slotExpandSelected()));

    connect(delTagFromImg, SIGNAL(triggered()),
            this, SLOT(slotRemoveTagsFromImgs()));

    connect(markUnused, SIGNAL(triggered()),
            this, SLOT(slotMarkNotAssignedTags()));

    d->organizeAction->addAction(d->titleEdit);
    d->organizeAction->addAction(resetIcon);
    d->organizeAction->addAction(createTagAddr);
    d->organizeAction->addAction(markUnused);
    d->organizeAction->addAction(invSel);
    d->organizeAction->addAction(expandTree);
    d->organizeAction->addAction(expandSel);
    d->organizeAction->addAction(delTagFromImg);

    /**
     * Sync & Export Group
     */
    d->syncexportAction     = new QMenu(i18n("Sync &Export"), this);
    d->syncexportAction->setIcon(QIcon::fromTheme(QLatin1String("network-server-database")));

    QAction* const wrDbImg  = new QAction(QIcon::fromTheme(QLatin1String("view-refresh")),
                                          i18n("Write Tags from Database to Image"), this);

    QAction* const readTags = new QAction(QIcon::fromTheme(QLatin1String("tag-new")),
                                          i18n("Read Tags from Image"), this);

    QAction* const wipeAll  = new QAction(QIcon::fromTheme(QLatin1String("draw-eraser")),
                                          i18n("Wipe all tags from Database only"), this);

    setHelpText(wrDbImg, i18n("Write Tags Metadata to Image."));

    setHelpText(readTags, i18n("Read tags from Images into Database. "
                               "Existing tags will not be affected"));

    setHelpText(wipeAll, i18n("Delete all tags from database only. Will not sync with files. "
                              "Proceed with caution."));

    connect(wrDbImg, SIGNAL(triggered()),
            this, SLOT(slotWriteToImg()));

    connect(readTags, SIGNAL(triggered()),
            this, SLOT(slotReadFromImg()));

    connect(wipeAll, SIGNAL(triggered()),
            this, SLOT(slotWipeAll()));

    d->syncexportAction->addAction(wrDbImg);
    d->syncexportAction->addAction(readTags);
    d->syncexportAction->addAction(wipeAll);

    d->mainToolbar->addAction(d->addAction);
    d->mainToolbar->addAction(d->delAction);
    d->mainToolbar->addAction(d->organizeAction->menuAction());
    d->mainToolbar->addAction(d->syncexportAction->menuAction());
    d->mainToolbar->addAction(new DLogoAction(this));
    addToolBar(d->mainToolbar);

    d->rootDisabledOptions.append(d->delAction);
    d->rootDisabledOptions.append(d->titleEdit);
    d->rootDisabledOptions.append(resetIcon);
    d->rootDisabledOptions.append(delTagFromImg);
}

/// helper based on KAction::setHelpText
void TagsManager::setHelpText(QAction* const action, const QString& text)
{
    action->setStatusTip(text);
    action->setToolTip(text);

    if (action->whatsThis().isEmpty())
    {
        action->setWhatsThis(text);
    }
}

void TagsManager::enableRootTagActions(bool value)
{
    foreach (QAction* const action, d->rootDisabledOptions)
    {
        if (value)
        {
            action->setEnabled(true);
        }
        else
        {
            action->setEnabled(false);
        }
    }
}

void TagsManager::doLoadState()
{
    KConfigGroup group = getConfigGroup();
    d->tagMngrView->doLoadState();
    group.sync();
}

void TagsManager::doSaveState()
{
    KConfigGroup group = getConfigGroup();
    d->tagMngrView->doSaveState();
    group.sync();
}

void TagsManager::slotMarkNotAssignedTags()
{
    QModelIndex root = d->tagMngrView->model()->index(0, 0);

    QQueue<QModelIndex> greyNodes;
    QList<QModelIndex>  redNodes;
    QSet<QModelIndex>   greenNodes;

    int iter = 0;

    while (root.model()->hasIndex(iter, 0, root))
    {
        greyNodes.append(root.model()->index(iter++, 0, root));
    }

    while (!greyNodes.isEmpty())
    {
        QModelIndex current = greyNodes.dequeue();

        if (!(current.isValid()))
        {
            continue;
        }

        if (current.model()->hasIndex(0, 0, current))
        {
            // Add in the list

            int iterator = 0;

            while (current.model()->hasIndex(iterator, 0, current))
            {
                greyNodes.append(current.model()->index(iterator++, 0, current));
            }
        }
        else
        {
            TAlbum* const t = static_cast<TAlbum*>(d->tagMngrView->albumForIndex(current));

            if (t && !t->isRoot() && !t->isInternalTag())
            {
                QList<qlonglong> assignedItems = CoreDbAccess().db()->getItemIDsInTag(t->id());

                if (assignedItems.isEmpty())
                {
                    redNodes.append(current);
                }
                else
                {
                    QModelIndex tmp = current.parent();

                    while (tmp.isValid())
                    {
                        greenNodes.insert(tmp);
                        tmp = tmp.parent();
                    }
                }
            }
        }
    }

    QItemSelectionModel* const model = d->tagMngrView->selectionModel();
    model->clearSelection();
    QList<int> toMark;

    foreach (const QModelIndex& index, redNodes)
    {
        QModelIndex current = index;

        while (current.isValid() && !greenNodes.contains(current))
        {
            TAlbum* const t = static_cast<TAlbum*>(d->tagMngrView->albumForIndex(current));

            if (t && !t->isRoot() && !t->isInternalTag()    &&
                (t->id() != FaceTags::ignoredPersonTagId()) &&
                (t->id() != FaceTags::unknownPersonTagId()) &&
                (t->id() != FaceTags::unconfirmedPersonTagId()))
            {
                QList<qlonglong> assignedItems = CoreDbAccess().db()->getItemIDsInTag(t->id());

                if (assignedItems.isEmpty() && !toMark.contains(t->id()))
                {
                    model->select(current, model->Select);
                    toMark.append(t->id());
                }
                else
                {
                    break;
                }
            }

            current = current.parent();
        }
    }
}

} // namespace Digikam
