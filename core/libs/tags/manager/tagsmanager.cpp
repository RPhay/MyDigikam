/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 20013-07-03
 * Description : Tag Manager main class
 *
 * SPDX-FileCopyrightText:      2013 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 * SPDX-FileCopyrightText:      2014 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2015-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2020-2025 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "tagsmanager_p.h"

namespace Digikam
{

TagsManager::TagsManager()
    : QMainWindow      (nullptr),
      StateSavingObject(this),
      d                (new Private(this))
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

    KConfigGroup group = getConfigGroup();
    DXmlGuiWindow::saveWindowSize(windowHandle(), group);
    group.sync();

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

bool TagsManager::isCreated()
{
    return !internalPtr.isNull();
}

void TagsManager::setupUi()
{
    setWindowTitle(i18nc("@title:window", "Tags Manager"));

    d->tagPixmap   = new QLabel();
    d->tagPixmap->setText(QLatin1String("Tag Pixmap"));
    d->tagPixmap->setMaximumWidth(40);
    d->tagPixmap->setPixmap(QIcon::fromTheme(QLatin1String("tag")).pixmap(30, 30));

    d->tagMngrView = new TagMngrTreeView(this, d->tagModel);
    d->tagMngrView->setConfigGroup(getConfigGroup());

    d->searchBar   = new SearchTextBarDb(this,
                                         QLatin1String("ItemIconViewTagSearchBar"),
                                         i18n("Search for tags..."));
    d->searchBar->setHighlightOnResult(true);
    d->searchBar->setModel(d->tagMngrView->filteredModel(),
                           AbstractAlbumModel::AlbumIdRole,
                           AbstractAlbumModel::AlbumTitleRole);
    d->searchBar->setMaximumWidth(200);
    d->searchBar->setFilterModel(d->tagMngrView->albumFilterModel());

    d->setupActions();

    // Tree Widget + Actions + Tag Properties

    d->tagPropWidget = new TagPropWidget(this);
    d->listView      = new TagList(d->tagMngrView, this);

    d->splitter      = new QSplitter(Qt::Horizontal, this);
    d->splitter->addWidget(d->listView);
    d->splitter->addWidget(d->tagMngrView);
    d->splitter->addWidget(d->tagPropWidget);

    connect(d->tagPropWidget, SIGNAL(signalTitleEditReady()),
            this, SLOT(slotTitleEditReady()));

    d->splitter->setStretchFactor(d->splitter->indexOf(d->tagMngrView), 10);

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
        d->enableRootTagActions(false);
        d->listView->enableAddButton(false);
    }
    else
    {
        d->enableRootTagActions(true);
        d->listView->enableAddButton(true);
        d->titleEdit->setEnabled((selectedTags.size() == 1));
    }

    d->tagPropWidget->slotSelectionChanged(selectedTags);
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

    Q_UNUSED(tList);

    TagEditDlg::showtagsListCreationError(qApp->activeWindow(), errMap);
}

void TagsManager::slotDeleteAction()
{
    const QModelIndexList selected = d->tagMngrView->selectionModel()->selectedIndexes();

    QStringList tagNames;
    QStringList tagsWithChildren;
    QStringList tagsWithImages;
    QMultiMap<int, TAlbum*> sortedTags;

    for (const QModelIndex& index : std::as_const(selected))
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
                                                       d->joinTagNamesToList(tagsWithChildren)),
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
                         d->joinTagNamesToList(tagsWithImages));
    }
    else
    {
        message = i18ncp("%2 is a comma separated list of tags to be deleted.",
                         "Delete tag %2?",
                         "Delete tags %2?",
                         tagNames.count(),
                         d->joinTagNamesToList(tagNames));
    }

    const int result = QMessageBox::warning(this, i18ncp("@title:window", "Delete tag", "Delete tags", tagNames.count()),
                                            message, QMessageBox::Yes | QMessageBox::Cancel);

    if (result == QMessageBox::Yes)
    {
        const QList<TAlbum*>& sortedTagsList = sortedTags.values();
        QList<TAlbum*>::const_reverse_iterator it;
        QList<qlonglong> imageIds;

        /**
         * QMultimap doesn't provide reverse iterator, use QList.
         */
        for (it = sortedTagsList.crbegin() ; it != sortedTagsList.crend() ; ++it)
        {
            QString errMsg;

            if (!AlbumManager::instance()->deleteTAlbum(*it, errMsg, &imageIds))
            {
                QMessageBox::critical(qApp->activeWindow(), qApp->applicationName(), errMsg);
            }
        }

        AlbumManager::instance()->askUserForWriteChangedTAlbumToFiles(imageIds);
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
                                           "tags from database.<br> You may <b>lose tags</b> if you did not "
                                           "read tags before (by calling Read Tags from Image).<br> "
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

void TagsManager::slotSaveTags()
{
    QList<Album*> selectedTags = d->tagMngrView->selectedTags();

    if ((selectedTags.size() != 1) || !selectedTags.constFirst())
    {
        QMessageBox::information(this, qApp->applicationName(),
                                 i18n("Select a base tag from which the tags should be exported."));

        return;
    }

    QString baseTag = static_cast<TAlbum*>(selectedTags.constFirst())->tagPath();

    if (baseTag.count(QLatin1Char('/')) > 1)
    {
        QMessageBox::warning(this, qApp->applicationName(),
                                 i18n("Selected tag is not a base tag at the top of the hierarchy."));

        return;
    }

    QString tagsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QString savePath = DFileDialog::getSaveFileName(qApp->activeWindow(),
                                                    i18nc("@title:window", "Export Controlled Vocabulary Tag File"),
                                                    tagsPath, i18n("Text File (*.txt)"), nullptr,
                                                    QFileDialog::DontConfirmOverwrite);

    if (savePath.isEmpty())
    {
        return;
    }

    if (!savePath.endsWith(QLatin1String(".txt"), Qt::CaseInsensitive))
    {
        savePath.append(QLatin1String(".txt"));
    }

    QFile tagFile(savePath);

    if (!tagFile.open(QIODevice::WriteOnly))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot open file to export tags" << savePath;

        return;
    }

    QTextStream tagStream(&tagFile);

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)

    // In Qt5 only. Qt6 uses UTF-8 by default.

    tagStream.setCodec(QTextCodec::codecForName("UTF-8"));

#endif

    QApplication::setOverrideCursor(Qt::WaitCursor);

    tagStream.setAutoDetectUnicode(true);

    QStringList sortedTagList;
    AlbumList::const_iterator it;
    AlbumList tList = AlbumManager::instance()->allTAlbums();

    for (it = tList.constBegin() ; it != tList.constEnd() ; ++it)
    {
        TAlbum* const tag = static_cast<TAlbum*>(*it);

        if (
            tag                                    &&
            !tag->isRoot()                         &&
            !tag->isInternalTag()                  &&
            !FaceTags::isSystemPersonTagId(tag->id())
           )
        {
            if (
                (baseTag == tag->tagPath())                        ||
                (baseTag == QLatin1String("/"))                    ||
                tag->tagPath().startsWith(baseTag + QLatin1Char('/'))
               )
            {
                sortedTagList << tag->tagPath();
            }
        }
    }

    sortedTagList.sort();

    for (const QString& stag : std::as_const(sortedTagList))
    {
        QStringList tagPathList = stag.split(QLatin1Char('/'),
                                             Qt::SkipEmptyParts);

        if (tagPathList.isEmpty())
        {
            continue;
        }

        QString ctag = tagPathList.takeLast();
        int tabs     = tagPathList.size();

        while (tabs > 0)
        {
            tagStream << QLatin1Char('\t');
            --tabs;
        }

        tagStream << ctag << Qt::endl;
    }

    tagFile.close();

    QApplication::restoreOverrideCursor();
}

void TagsManager::slotLoadTags()
{
    QList<Album*> selectedTags = d->tagMngrView->selectedTags();

    if ((selectedTags.size() != 1) || !selectedTags.constFirst())
    {
        QMessageBox::information(this, qApp->applicationName(),
                                 i18n("Select a tag into which the tags should be imported."));

        return;
    }

    TAlbum* const selTag = static_cast<TAlbum*>(selectedTags.constFirst());

    if (FaceTags::isSystemPersonTagId(selTag->id()))
    {
        QMessageBox::warning(this, qApp->applicationName(),
                             i18n("No tags can be imported to the selected face tag."));

        return;
    }

    QString baseTag  = selTag->tagPath();
    QString tagsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    QString loadPath = DFileDialog::getOpenFileName(qApp->activeWindow(),
                                                    i18nc("@title:window", "Import Controlled Vocabulary Tag File"),
                                                    tagsPath, i18n("Text File (*.txt)"));

    if (loadPath.isEmpty())
    {
        return;
    }

    QFile tagFile(loadPath);

    if (!tagFile.open(QIODevice::ReadOnly))
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Cannot open file to import tags" << loadPath;

        return;
    }

    QTextStream tagStream(&tagFile);

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)

    // In Qt5 only. Qt6 uses UTF-8 by default.

    tagStream.setCodec(QTextCodec::codecForName("UTF-8"));

#endif

    QApplication::setOverrideCursor(Qt::WaitCursor);

    tagStream.setAutoDetectUnicode(true);

    QStringList tagPathList;
    int startPos = -1;
    QString line;

    while (tagStream.readLineInto(&line))
    {
        QString currTag;
        int tabPos = 0;

        for (int i = 0 ; i < line.size() ; ++i)
        {
            if (
                (line.at(i) == QLatin1Char('[')) ||
                (line.at(i) == QLatin1Char('{'))
               )
            {
                break;
            }

            if (line.at(i) == QLatin1Char('\t'))
            {
                ++tabPos;
                continue;
            }

            if (startPos == -1)
            {
                startPos = i;
            }

            currTag = line.mid(i);
            break;
        }

        if (!currTag.isEmpty())
        {
            const int count = tabPos - startPos;

            while (!tagPathList.isEmpty() && (tagPathList.size() > count))
            {
                tagPathList.removeLast();
            }

            QString tagPath = baseTag;

            tagPathList.append(currTag);

            if (baseTag != QLatin1String("/"))
            {
                tagPath.append(QLatin1Char('/'));
            }

            tagPath.append(tagPathList.join(QLatin1Char('/')));

            TagsCache::instance()->getOrCreateTag(tagPath);
        }
    }

    tagFile.close();

    QApplication::restoreOverrideCursor();
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

    for (const QModelIndex& index : std::as_const(selList))
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
    if (d->firstShowEvent)
    {
        KConfigGroup group = getConfigGroup();

        DXmlGuiWindow::setGoodDefaultWindowSize(windowHandle());
        DXmlGuiWindow::restoreWindowSize(windowHandle(), group);
        resize(windowHandle()->size());

        d->firstShowEvent = false;
    }

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

    for (const QModelIndex& index : std::as_const(redNodes))
    {
        QModelIndex current = index;

        while (current.isValid() && !greenNodes.contains(current))
        {
            TAlbum* const t = static_cast<TAlbum*>(d->tagMngrView->albumForIndex(current));

            if (
                t                                       &&
                !t->isRoot()                            &&
                !t->isInternalTag()                     &&
                !FaceTags::isSystemPersonTagId(t->id()) &&
                (FaceTags::personParentTag() != t->id())
               )
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

#include "moc_tagsmanager.cpp"
