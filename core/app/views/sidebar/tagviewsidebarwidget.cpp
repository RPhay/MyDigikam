/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Side Bar Widget for the tag view.
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * Copyright (C) 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#include "tagviewsidebarwidget.h"

// Qt includes

#include <QButtonGroup>
#include <QRadioButton>
#include <QApplication>
#include <QStyle>
#include <QPushButton>
#include <QIcon>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "applicationsettings.h"
#include "searchtextbardb.h"
#include "tagfolderview.h"
#include "tagsmanager.h"
#include "coredb.h"
#include "coredbsearchxml.h"

namespace Digikam
{

class Q_DECL_HIDDEN TagViewSideBarWidget::Private
{
public:

    enum TagsSource
    {
        NoTags = 0,
        ExistingTags
    };

public:

    explicit Private()
      : openTagMngr             (nullptr),
        tagSearchBar            (nullptr),
        tagFolderView           (nullptr),
        btnGroup                (nullptr),
        noTagsBtn               (nullptr),
        tagsBtn                 (nullptr),
        noTagsWasChecked        (false),
        ExistingTagsWasChecked  (false)
    {
    }

public:

    QPushButton*         openTagMngr;
    SearchTextBarDb*     tagSearchBar;
    TagFolderView*       tagFolderView;
    QButtonGroup*        btnGroup;
    QRadioButton*        noTagsBtn;
    QRadioButton*        tagsBtn;

    bool                 noTagsWasChecked;
    bool                 ExistingTagsWasChecked;

    QString              noTagsSearchXml;

    static const QString configTagsSourceEntry;
};

const QString TagViewSideBarWidget::Private::configTagsSourceEntry(QLatin1String("TagsSource"));

TagViewSideBarWidget::TagViewSideBarWidget(QWidget* const parent, TagModel* const model)
    : SidebarWidget(parent),
      d            (new Private)
{
    setObjectName(QLatin1String("TagView Sidebar"));
    setProperty("Shortcut", Qt::CTRL + Qt::SHIFT + Qt::Key_F2);

    const int spacing         = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QVBoxLayout* const layout = new QVBoxLayout(this);

    d->openTagMngr = new QPushButton( i18n("Open Tag Manager"));
    d->noTagsBtn   = new QRadioButton(i18n("No Tags"), this);
    d->tagsBtn     = new QRadioButton(i18n("Existing Tags"), this);
    d->btnGroup    = new QButtonGroup(this);
    d->btnGroup->addButton(d->noTagsBtn);
    d->btnGroup->addButton(d->tagsBtn);
    d->btnGroup->setId(d->noTagsBtn, 0);
    d->btnGroup->setId(d->tagsBtn, 1);
    d->btnGroup->setExclusive(true);

    d->tagFolderView = new TagFolderView(this, model);
    d->tagFolderView->setConfigGroup(getConfigGroup());
    d->tagFolderView->setAlbumManagerCurrentAlbum(true);

    //d->tagFolderView->filteredModel()->doNotListTagsWithProperty(TagPropertyName::person());
    //d->tagFolderView->filteredModel()->setFilterBehavior(AlbumFilterModel::StrictFiltering);

    d->tagSearchBar  = new SearchTextBarDb(this, QLatin1String("ItemIconViewTagSearchBar"));
    d->tagSearchBar->setHighlightOnResult(true);
    d->tagSearchBar->setModel(model, AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);

    //d->tagSearchBar->setModel(d->tagFolderView->filteredModel(),
    //                          AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);

    d->tagSearchBar->setFilterModel(d->tagFolderView->albumFilterModel());

    layout->addWidget(d->openTagMngr);
    layout->addWidget(d->noTagsBtn);
    layout->addWidget(d->tagsBtn);
    layout->addWidget(d->tagFolderView);
    layout->addWidget(d->tagSearchBar);
    layout->setContentsMargins(0, 0, spacing, 0);

    connect(d->openTagMngr, SIGNAL(clicked()),
            this,SLOT(slotOpenTagManager()));

    connect(d->tagFolderView, SIGNAL(signalFindDuplicates(QList<TAlbum*>)),
            this, SIGNAL(signalFindDuplicates(QList<TAlbum*>)));

    connect(d->btnGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(slotToggleTagsSelection(int)));
}

TagViewSideBarWidget::~TagViewSideBarWidget()
{
    delete d;
}

void TagViewSideBarWidget::setActive(bool active)
{
    if (active)
    {
        if (d->noTagsBtn->isChecked())
        {
            setNoTagsAlbum();
        }
        else
        {
            AlbumManager::instance()->setCurrentAlbums(d->tagFolderView->selectedTags());
        }
    }
}

void TagViewSideBarWidget::doLoadState()
{
    KConfigGroup group        = getConfigGroup();
    bool noTagsBtnWasChecked  = group.readEntry(d->configTagsSourceEntry, false);
    d->noTagsBtn->setChecked(noTagsBtnWasChecked);
    d->tagsBtn->setChecked(!noTagsBtnWasChecked);
    d->noTagsWasChecked       = noTagsBtnWasChecked;
    d->ExistingTagsWasChecked = !noTagsBtnWasChecked;

    d->tagFolderView->loadState();
    d->tagFolderView->setDisabled(noTagsBtnWasChecked);
}

void TagViewSideBarWidget::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    group.writeEntry(d->configTagsSourceEntry, d->noTagsBtn->isChecked());

    d->tagFolderView->saveState();

    group.sync();
}

void TagViewSideBarWidget::applySettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();
    d->tagFolderView->setExpandNewCurrentItem(settings->getExpandNewCurrentItem());
}

void TagViewSideBarWidget::changeAlbumFromHistory(const QList<Album*>& album)
{
    if (album.first()->type() == Album::TAG)
    {
        d->tagsBtn->setChecked(true);
        d->tagFolderView->setEnabled(true);
        d->ExistingTagsWasChecked = true;
        d->noTagsWasChecked       = false;
        d->tagFolderView->setCurrentAlbums(album);
    }
    else
    {
        d->noTagsBtn->setChecked(true);
        d->tagFolderView->setDisabled(true);
        d->noTagsWasChecked       = true;
        d->ExistingTagsWasChecked = false;
    }
}

AlbumPointer<TAlbum> TagViewSideBarWidget::currentAlbum() const
{
    return AlbumPointer<TAlbum> (d->tagFolderView->currentAlbum());
}

void TagViewSideBarWidget::setNoTagsAlbum()
{
    if (d->noTagsSearchXml.isEmpty())
    {
        SearchXmlWriter writer;
        writer.setFieldOperator((SearchXml::standardFieldOperator()));
        writer.writeGroup();
        writer.writeField(QLatin1String("nottagged"), SearchXml::Equal);
        writer.finishField();
        writer.finishGroup();
        writer.finish();
        d->noTagsSearchXml = writer.xml();
    }

    QString title = SAlbum::getTemporaryTitle(DatabaseSearch::AdvancedSearch);
    SAlbum* album = AlbumManager::instance()->findSAlbum(title);

    int id;

    if (album)
    {
        id = album->id();
        CoreDbAccess().db()->updateSearch(id, DatabaseSearch::AdvancedSearch,
                                          SAlbum::getTemporaryTitle(DatabaseSearch::AdvancedSearch), d->noTagsSearchXml);
    }
    else
    {
        id = CoreDbAccess().db()->addSearch(DatabaseSearch::AdvancedSearch,
                                            SAlbum::getTemporaryTitle(DatabaseSearch::AdvancedSearch), d->noTagsSearchXml);
    }

    album = new SAlbum(i18n("No Tags Album"), id);

    if (album)
    {
        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << album);
    }
}

const QIcon TagViewSideBarWidget::getIcon()
{
    return QIcon::fromTheme(QLatin1String("tag"));
}

const QString TagViewSideBarWidget::getCaption()
{
    return i18n("Tags");
}

void TagViewSideBarWidget::setCurrentAlbum(TAlbum* album)
{
    d->tagFolderView->setCurrentAlbums(QList<Album*>() << album);
}

void TagViewSideBarWidget::slotOpenTagManager()
{
    TagsManager* const tagMngr = TagsManager::instance();
    tagMngr->show();
    tagMngr->activateWindow();
    tagMngr->raise();
}

void TagViewSideBarWidget::slotToggleTagsSelection(int radioClicked)
{
    switch (Private::TagsSource(radioClicked))
    {
        case Private::NoTags:
        {
            if (!d->noTagsWasChecked)
            {
                setNoTagsAlbum();
                d->tagFolderView->setDisabled(true);
                d->noTagsWasChecked       = d->noTagsBtn->isChecked();
                d->ExistingTagsWasChecked = d->tagsBtn->isChecked();
            }

            break;
        }
        case Private::ExistingTags:
        {
            if (!d->ExistingTagsWasChecked)
            {
                d->tagFolderView->setEnabled(true);
                setActive(true);
                d->noTagsWasChecked       = d->noTagsBtn->isChecked();
                d->ExistingTagsWasChecked = d->tagsBtn->isChecked();
            }

            break;
        }
    }
}

} // namespace Digikam
