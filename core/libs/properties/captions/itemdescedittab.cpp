/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-03-09
 * Description : Captions, Tags, and Rating properties editor
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

ItemDescEditTab::ItemDescEditTab(QWidget* const parent)
    : DVBox(parent),
      d    (new Private)
{
    const int spacing      = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                                  QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    setContentsMargins(QMargins());
    setSpacing(spacing);
    d->tabWidget           = new QTabWidget(this);

    d->metadataChangeTimer = new QTimer(this);
    d->metadataChangeTimer->setSingleShot(true);
    d->metadataChangeTimer->setInterval(250);

    // Captions/Date/Rating view -----------------------------------

    initDescriptionView();

    // Tags view ---------------------------------------------------

    initTagsView();

    // Information Management View --------------------------------------

    initInformationView();

    // --------------------------------------------------

    connect(d->openTagMngr, SIGNAL(clicked()),
            this, SLOT(slotOpenTagsManager()));

    connect(d->tagCheckView->checkableModel(), SIGNAL(checkStateChanged(Album*,Qt::CheckState)),
            this, SLOT(slotTagStateChanged(Album*,Qt::CheckState)));

    connect(d->titleEdit, SIGNAL(signalModified(QString,QString)),
            this, SLOT(slotTitleChanged()));

    connect(d->titleEdit, SIGNAL(signalValueAdded(QString,QString)),
            this, SLOT(slotTitleChanged()));

    connect(d->titleEdit, SIGNAL(signalValueDeleted(QString)),
            this, SLOT(slotTitleChanged()));

    connect(d->captionsEdit, SIGNAL(signalModified()),
            this, SLOT(slotCommentChanged()));

    connect(d->dateTimeEdit, SIGNAL(dateTimeChanged(QDateTime)),
            this, SLOT(slotDateTimeChanged(QDateTime)));

    connect(d->pickLabelSelector, SIGNAL(signalPickLabelChanged(int)),
            this, SLOT(slotPickLabelChanged(int)));

    connect(d->colorLabelSelector, SIGNAL(signalColorLabelChanged(int)),
            this, SLOT(slotColorLabelChanged(int)));

    connect(d->ratingWidget, SIGNAL(signalRatingChanged(int)),
            this, SLOT(slotRatingChanged(int)));

    connect(d->templateSelector, SIGNAL(signalTemplateSelected()),
            this, SLOT(slotTemplateSelected()));

    connect(d->tagsSearchBar, SIGNAL(signalSearchTextSettings(SearchTextSettings)),
            this, SLOT(slotTagsSearchChanged(SearchTextSettings)));

    connect(d->assignedTagsBtn, SIGNAL(toggled(bool)),
            this, SLOT(slotAssignedTagsToggled(bool)));

    connect(d->newTagEdit, SIGNAL(taggingActionActivated(TaggingAction)),
            this, SLOT(slotTaggingActionActivated(TaggingAction)));

    connect(d->applyBtn, SIGNAL(clicked()),
            this, SLOT(slotApplyAllChanges()));

    connect(d->applyToAllVersionsButton, SIGNAL(clicked()),
            this, SLOT(slotApplyChangesToAllVersions()));

    connect(d->revertBtn, SIGNAL(clicked()),
            this, SLOT(slotRevertAllChanges()));

    connect(d->moreMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotMoreMenu()));

    connect(d->metadataChangeTimer, SIGNAL(timeout()),
            this, SLOT(slotReloadForMetadataChange()));

    connect(this, SIGNAL(signalAskToApplyChanges(QList<ItemInfo>,DisjointMetadata*)),
            this, SLOT(slotAskToApplyChanges(QList<ItemInfo>,DisjointMetadata*)),
            Qt::QueuedConnection);

    // Initialize ---------------------------------------------

    d->titleEdit->textEdit()->installEventFilter(this);
    d->captionsEdit->altLangStrEdit()->textEdit()->installEventFilter(this);

    d->dateTimeEdit->installEventFilter(this);
    d->pickLabelSelector->installEventFilter(this);
    d->colorLabelSelector->installEventFilter(this);
    d->ratingWidget->installEventFilter(this);

    // TODO update, what does this filter?

    d->tagCheckView->installEventFilter(this);
    d->newTagEdit->installEventFilter(this);
    updateRecentTags();

    // Connect to attribute watch ------------------------------

    ItemAttributesWatch* const watch = ItemAttributesWatch::instance();

    connect(watch, SIGNAL(signalImageTagsChanged(qlonglong)),
            this, SLOT(slotImageTagsChanged(qlonglong)));

    connect(watch, SIGNAL(signalImagesChanged(int)),
            this, SLOT(slotImagesChanged(int)));

    connect(watch, SIGNAL(signalImageRatingChanged(qlonglong)),
            this, SLOT(slotImageRatingChanged(qlonglong)));

    connect(watch, SIGNAL(signalImageDateChanged(qlonglong)),
            this, SLOT(slotImageDateChanged(qlonglong)));

    connect(watch, SIGNAL(signalImageCaptionChanged(qlonglong)),
            this, SLOT(slotImageCaptionChanged(qlonglong)));
}

ItemDescEditTab::~ItemDescEditTab()
{
    delete d;
}

void ItemDescEditTab::readSettings(KConfigGroup& group)
{
    d->tabWidget->setCurrentIndex(group.readEntry(QLatin1String("ItemDescEdit Tab"), (int)Private::DESCRIPTIONS));
    d->titleEdit->setCurrentLanguageCode(group.readEntry(QLatin1String("ItemDescEditTab TitleLang"), QString()));
    d->captionsEdit->setCurrentLanguageCode(group.readEntry(QLatin1String("ItemDescEditTab CaptionsLang"), QString()));

    d->templateViewer->readSettings(group);

    d->tagCheckView->setConfigGroup(group);
    d->tagCheckView->setEntryPrefix(QLatin1String("ItemDescEditTab TagCheckView"));
    d->tagCheckView->loadState();
    d->tagsSearchBar->setConfigGroup(group);
    d->tagsSearchBar->setEntryPrefix(QLatin1String("ItemDescEditTab SearchBar"));
    d->tagsSearchBar->loadState();
}

void ItemDescEditTab::writeSettings(KConfigGroup& group)
{
    group.writeEntry(QLatin1String("ItemDescEdit Tab"),             d->tabWidget->currentIndex());
    group.writeEntry(QLatin1String("ItemDescEditTab TitleLang"),    d->titleEdit->currentLanguageCode());
    group.writeEntry(QLatin1String("ItemDescEditTab CaptionsLang"), d->captionsEdit->currentLanguageCode());

    d->templateViewer->writeSettings(group);

    d->tagCheckView->saveState();
    d->tagsSearchBar->saveState();
}

void ItemDescEditTab::setFocusToLastSelectedWidget()
{
    if (d->lastSelectedWidget)
    {
        d->lastSelectedWidget->setFocus();
    }

    d->lastSelectedWidget = nullptr;
}

bool ItemDescEditTab::singleSelection() const
{
    return (d->currInfos.count() == 1);
}

void ItemDescEditTab::slotChangingItems()
{
    if (!d->modified)
    {
        return;
    }

    if (d->currInfos.isEmpty())
    {
        return;
    }

    if (!ApplicationSettings::instance()->getApplySidebarChangesDirectly())
    {
        // Open dialog via queued connection out-of-scope, see bug 302311

        DisjointMetadata* const hub2 = new DisjointMetadata();
        hub2->setDataFields(d->hub.dataFields());

        Q_EMIT signalAskToApplyChanges(d->currInfos, hub2);

        reset();
    }
    else
    {
        slotApplyAllChanges();
    }
}

void ItemDescEditTab::slotAskToApplyChanges(const QList<ItemInfo>& infos, DisjointMetadata* hub)
{
    int changedFields = 0;

    if (hub->titlesChanged())
    {
        ++changedFields;
    }

    if (hub->commentsChanged())
    {
        ++changedFields;
    }

    if (hub->dateTimeChanged())
    {
        ++changedFields;
    }

    if (hub->ratingChanged())
    {
        ++changedFields;
    }

    if (hub->pickLabelChanged())
    {
        ++changedFields;
    }

    if (hub->colorLabelChanged())
    {
        ++changedFields;
    }

    if (hub->tagsChanged())
    {
        ++changedFields;
    }

    QString text;

    if (changedFields == 1)
    {
        if      (hub->commentsChanged())
        {
            text = i18np("You have edited the image caption. ",
                         "You have edited the captions of %1 images. ",
                         infos.count());
        }
        else if (hub->titlesChanged())
        {
            text = i18np("You have edited the image title. ",
                         "You have edited the titles of %1 images. ",
                         infos.count());
        }
        else if (hub->dateTimeChanged())
        {
            text = i18np("You have edited the date of the image. ",
                         "You have edited the date of %1 images. ",
                         infos.count());
        }
        else if (hub->pickLabelChanged())
        {
            text = i18np("You have edited the pick label of the image. ",
                         "You have edited the pick label of %1 images. ",
                         infos.count());
        }
        else if (hub->colorLabelChanged())
        {
            text = i18np("You have edited the color label of the image. ",
                         "You have edited the color label of %1 images. ",
                         infos.count());
        }
        else if (hub->ratingChanged())
        {
            text = i18np("You have edited the rating of the image. ",
                         "You have edited the rating of %1 images. ",
                         infos.count());
        }
        else if (hub->tagsChanged())
        {
            text = i18np("You have edited the tags of the image. ",
                         "You have edited the tags of %1 images. ",
                         infos.count());
        }

        text += i18n("Do you want to apply your changes?");
    }
    else
    {
        text = i18np("<p>You have edited the metadata of the image: </p>",
                     "<p>You have edited the metadata of %1 images: </p>",
                     infos.count());

        text += QLatin1String("<p><ul>");

        if (hub->titlesChanged())
        {
            text += i18n("<li>title</li>");
        }

        if (hub->commentsChanged())
        {
            text += i18n("<li>caption</li>");
        }

        if (hub->dateTimeChanged())
        {
            text += i18n("<li>date</li>");
        }

        if (hub->pickLabelChanged())
        {
            text += i18n("<li>pick label</li>");
        }

        if (hub->colorLabelChanged())
        {
            text += i18n("<li>color label</li>");
        }

        if (hub->ratingChanged())
        {
            text += i18n("<li>rating</li>");
        }

        if (hub->tagsChanged())
        {
            text += i18n("<li>tags</li>");
        }

        text += QLatin1String("</ul></p>");

        text += i18n("<p>Do you want to apply your changes?</p>");
    }

    QCheckBox* const alwaysCBox  = new QCheckBox(i18n("Always apply changes without confirmation"));

    QPointer<QMessageBox> msgBox = new QMessageBox(QMessageBox::Information,
                                                   i18n("Apply changes?"),
                                                   text,
                                                   QMessageBox::Yes | QMessageBox::No,
                                                   qApp->activeWindow());
    msgBox->setCheckBox(alwaysCBox);
    msgBox->setDefaultButton(QMessageBox::No);
    msgBox->setEscapeButton(QMessageBox::No);

    // Pop-up a message in desktop notification manager

    DNotificationWrapper(QString(), i18n("Apply changes?"),
                         DigikamApp::instance(), DigikamApp::instance()->windowTitle());

    int returnCode   = msgBox->exec();
    bool alwaysApply = msgBox->checkBox()->isChecked();
    delete msgBox;

    if (alwaysApply)
    {
        ApplicationSettings::instance()->setApplySidebarChangesDirectly(true);
    }

    if (returnCode == QMessageBox::No)
    {
        delete hub;
        return;
    }

    // otherwise apply:

    FileActionMngr::instance()->applyMetadata(infos, hub);
}

void ItemDescEditTab::reset()
{
    d->modified = false;
    d->hub.resetChanged();
    d->applyBtn->setEnabled(false);
    d->revertBtn->setEnabled(false);
    d->applyToAllVersionsButton->setEnabled(false);
}

void ItemDescEditTab::slotApplyAllChanges()
{
    if (!d->modified)
    {
        return;
    }

    if (d->currInfos.isEmpty())
    {
        return;
    }

    FileActionMngr::instance()->applyMetadata(d->currInfos, d->hub);
    reset();
}

void ItemDescEditTab::slotRevertAllChanges()
{
    if (!d->modified)
    {
        return;
    }

    if (d->currInfos.isEmpty())
    {
        return;
    }

    setInfos(d->currInfos);
}

void ItemDescEditTab::setItem(const ItemInfo& info)
{
    slotChangingItems();
    ItemInfoList list;

    if (!info.isNull())
    {
        list << info;
    }

    setInfos(list);
}

void ItemDescEditTab::setItems(const ItemInfoList& infos)
{
    slotChangingItems();
    setInfos(infos);
}

void ItemDescEditTab::setInfos(const ItemInfoList& infos)
{
    if (infos.isEmpty())
    {
        d->hub.reset();
        d->captionsEdit->blockSignals(true);
        d->captionsEdit->reset();
        resetCaptionEditPlaceholderText();
        d->captionsEdit->blockSignals(false);
        d->titleEdit->blockSignals(true);
        d->titleEdit->reset();
        resetTitleEditPlaceholderText();
        d->titleEdit->blockSignals(false);
        d->currInfos.clear();
        resetMetadataChangeInfo();
        setEnabled(false);
        return;
    }

    setEnabled(true);
    d->currInfos = infos;
    d->modified  = false;
    resetMetadataChangeInfo();
    d->hub.reset();
    d->applyBtn->setEnabled(false);
    d->revertBtn->setEnabled(false);

    if (d->currInfos.count() > 1000)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }

    // First, we read all tags of the items into the cache with one SQL query.
    // This is faster than each item individually.

    d->currInfos.loadTagIds();

    Q_FOREACH (const ItemInfo& info, d->currInfos)
    {
        d->hub.load(info);
    }

    updateComments();
    updatePickLabel();
    updateColorLabel();
    updateRating();
    updateDate();
    updateTemplate();
    updateTagsView();
    updateRecentTags();
    setFocusToLastSelectedWidget();

    if (d->currInfos.count() > 1000)
    {
        QApplication::restoreOverrideCursor();
    }
}

void ItemDescEditTab::slotReadFromFileMetadataToDatabase()
{
    initProgressIndicator();

    Q_EMIT signalProgressMessageChanged(i18n("Reading metadata from files. Please wait..."));

    d->ignoreItemAttributesWatch = true;
    int i                        = 0;

    ScanController::instance()->suspendCollectionScan();

    CollectionScanner scanner;

    Q_FOREACH (const ItemInfo& info, d->currInfos)
    {
        scanner.scanFile(info, CollectionScanner::CleanScan);

        Q_EMIT signalProgressValueChanged(i++/(float)d->currInfos.count());

        qApp->processEvents();
    }

    ScanController::instance()->resumeCollectionScan();
    d->ignoreItemAttributesWatch = false;

    Q_EMIT signalProgressFinished();

    // reload everything

    setInfos(d->currInfos);
}

void ItemDescEditTab::slotWriteToFileMetadataFromDatabase()
{
    initProgressIndicator();

    Q_EMIT signalProgressMessageChanged(i18n("Writing metadata to files. Please wait..."));

    int i = 0;

    Q_FOREACH (const ItemInfo& info, d->currInfos)
    {
        MetadataHub fileHub;

        // read in from database

        fileHub.load(info);

        // write out to file DMetadata

        fileHub.write(info.filePath());

        Q_EMIT signalProgressValueChanged(i++ / (float)d->currInfos.count());
        qApp->processEvents();
    }

    Q_EMIT signalProgressFinished();
}

bool ItemDescEditTab::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent* const k = static_cast<QKeyEvent*>(e);

        if ((k->key() == Qt::Key_Enter) || (k->key() == Qt::Key_Return))
        {
            if      (k->modifiers() == Qt::ControlModifier)
            {
                d->lastSelectedWidget = qobject_cast<QWidget*>(o);
                Q_EMIT signalNextItem();
                return true;
            }
            else if (k->modifiers() == Qt::ShiftModifier)
            {
                d->lastSelectedWidget = qobject_cast<QWidget*>(o);
                Q_EMIT signalPrevItem();
                return true;
            }
        }

        if (k->key() == Qt::Key_PageUp)
        {
            d->lastSelectedWidget = qobject_cast<QWidget*>(o);
            Q_EMIT signalPrevItem();
            return true;
        }

        if (k->key() == Qt::Key_PageDown)
        {
            d->lastSelectedWidget = qobject_cast<QWidget*>(o);
            Q_EMIT signalNextItem();
            return true;
        }

        if ((d->newTagEdit == o) &&
            !d->newTagEdit->completer()->popup()->isVisible())
        {
            if (k->key() == Qt::Key_Up)
            {
                d->lastSelectedWidget = qobject_cast<QWidget*>(o);
                Q_EMIT signalPrevItem();
                return true;
            }

            if (k->key() == Qt::Key_Down)
            {
                d->lastSelectedWidget = qobject_cast<QWidget*>(o);
                Q_EMIT signalNextItem();
                return true;
            }
        }
    }

    return DVBox::eventFilter(o, e);
}

void ItemDescEditTab::slotModified()
{
    d->modified = true;
    d->applyBtn->setEnabled(true);
    d->revertBtn->setEnabled(true);

    if (d->currInfos.size() == 1)
    {
        d->applyToAllVersionsButton->setEnabled(true);
    }
}

void ItemDescEditTab::setMetadataWidgetStatus(int status, QWidget* const widget)
{
    if (status == DisjointMetadataDataFields::MetadataDisjoint)
    {
        // For text widgets: Set text color to color of disabled text.

        QPalette palette = widget->palette();
        palette.setColor(QPalette::Text, palette.color(QPalette::Disabled, QPalette::Text));
        widget->setPalette(palette);

        QString msg = i18n("Selected items have different text here.");

        if      (qobject_cast<AltLangStrEdit*>(widget))
        {
            d->titleEdit->setPlaceholderText(msg);
        }
        else if (qobject_cast<CaptionEdit*>(widget))
        {
            d->captionsEdit->setPlaceholderText(msg);
        }
    }
    else
    {
        widget->setPalette(QPalette());

        if      (qobject_cast<AltLangStrEdit*>(widget))
        {
            resetTitleEditPlaceholderText();
        }
        else if (qobject_cast<CaptionEdit*>(widget))
        {
            resetCaptionEditPlaceholderText();
        }
    }
}

void ItemDescEditTab::slotMoreMenu()
{
    d->moreMenu->clear();

    if (singleSelection())
    {
        d->moreMenu->addAction(i18n("Read metadata from file to database"), this, SLOT(slotReadFromFileMetadataToDatabase()));
        QAction* const writeAction = d->moreMenu->addAction(i18n("Write metadata to each file"), this,
                                                            SLOT(slotWriteToFileMetadataFromDatabase()));

        // we do not need a "Write to file" action here because the apply button will do just that
        // if selection is a single file.
        // Adding the option will confuse users: Does the apply button not write to file?
        // Removing the option will confuse users: There is not option to write to file! (not visible in single selection)
        // Disabling will confuse users: Why is it disabled?

        writeAction->setEnabled(false);
    }
    else
    {
        // We need to make clear that this action is different from the Apply button,
        // which saves the same changes to all files. These batch operations operate on each single file.

        d->moreMenu->addAction(i18n("Read metadata from each file to database"), this, SLOT(slotReadFromFileMetadataToDatabase()));
        d->moreMenu->addAction(i18n("Write metadata to each file"), this, SLOT(slotWriteToFileMetadataFromDatabase()));
        d->moreMenu->addSeparator();
        d->moreMenu->addAction(i18n("Unify tags of selected items"), this, SLOT(slotUnifyPartiallyTags()));
    }
}

void ItemDescEditTab::slotImagesChanged(int albumId)
{
    if (d->ignoreItemAttributesWatch || d->modified)
    {
        return;
    }

    Album* const a = AlbumManager::instance()->findAlbum(albumId);

    if (d->currInfos.isEmpty() || !a || a->isRoot() || (a->type() != Album::TAG))
    {
        return;
    }

    setInfos(d->currInfos);
}

// private common code for above methods

void ItemDescEditTab::metadataChange(qlonglong imageId)
{
    if (d->ignoreItemAttributesWatch || d->modified)
    {
        // Don't lose modifications

        return;
    }

    d->metadataChangeIds << imageId;
    d->metadataChangeTimer->start();
}

void ItemDescEditTab::resetMetadataChangeInfo()
{
    d->metadataChangeTimer->stop();
    d->metadataChangeIds.clear();
}

void ItemDescEditTab::slotReloadForMetadataChange()
{
    // NOTE: What to do if d->modified? Reloading is no option.
    // It may be a little change the user wants to ignore, or a large conflict.

    if (d->currInfos.isEmpty() || d->modified)
    {
        resetMetadataChangeInfo();
        return;
    }

    if (singleSelection())
    {
        if (d->metadataChangeIds.contains(d->currInfos.first().id()))
        {
            setInfos(d->currInfos);
        }
    }
    else
    {
        // if image id is in our list, update

        Q_FOREACH (const ItemInfo& info, d->currInfos)
        {
            if (d->metadataChangeIds.contains(info.id()))
            {   // cppcheck-suppress useStlAlgorithm
                setInfos(d->currInfos);
                break;
            }
        }
    }
}

void ItemDescEditTab::slotApplyChangesToAllVersions()
{
    if (!d->modified)
    {
        return;
    }

    if (d->currInfos.isEmpty())
    {
        return;
    }

    QSet<qlonglong>                     tmpSet;
    QList<QPair<qlonglong, qlonglong> > relations;

    Q_FOREACH (const ItemInfo& info, d->currInfos)
    {
        // Collect all ids in all image's relations.

        relations.append(info.relationCloud());
    }

    if (relations.isEmpty())
    {
        slotApplyAllChanges();
        return;
    }

    for (int i = 0 ; i < relations.size() ; ++i)
    {
        // Use QSet to prevent duplicates.

        tmpSet.insert(relations.at(i).first);
        tmpSet.insert(relations.at(i).second);
    }

    FileActionMngr::instance()->applyMetadata(ItemInfoList(tmpSet.values()), d->hub);

    d->modified = false;
    d->hub.resetChanged();
    d->applyBtn->setEnabled(false);
    d->revertBtn->setEnabled(false);
    d->applyToAllVersionsButton->setEnabled(false);
}

void ItemDescEditTab::initProgressIndicator()
{
    if (!ProgressManager::instance()->findItembyId(QLatin1String("ItemDescEditTabProgress")))
    {
        FileActionProgress* const item = new FileActionProgress(QLatin1String("ItemDescEditTabProgress"));

        connect(this, SIGNAL(signalProgressMessageChanged(QString)),
                item, SLOT(slotProgressStatus(QString)));

        connect(this, SIGNAL(signalProgressValueChanged(float)),
                item, SLOT(slotProgressValue(float)));

        connect(this, SIGNAL(signalProgressFinished()),
                item, SLOT(slotCompleted()));
    }
}

} // namespace Digikam
