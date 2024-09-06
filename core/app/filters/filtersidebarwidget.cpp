/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2000-12-05
 * Description : filters view for the right sidebar
 *
 * SPDX-FileCopyrightText: 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2010-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText:      2011 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText:      2014 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "filtersidebarwidget.h"

// Qt includes

#include <QActionGroup>
#include <QLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QToolButton>
#include <QMenu>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

#include "dlayoutbox.h"
#include "dexpanderbox.h"
#include "digikam_debug.h"
#include "applicationsettings.h"
#include "colorlabelfilter.h"
#include "geolocationfilter.h"
#include "picklabelfilter.h"
#include "ratingfilter.h"
#include "mimefilter.h"
#include "tagfilterview.h"
#include "searchtextbardb.h"
#include "facetags.h"

namespace Digikam
{

class Q_DECL_HIDDEN FilterSideBarWidget::Private
{
public:

    Private() = default;

    const QString                          configSearchTextFilterFieldsEntry    = QLatin1String("Search Text Filter Fields");
    const QString                          configLastShowUntaggedEntry          = QLatin1String("Show Untagged");
    const QString                          configLastShowWithoutFaceEntry       = QLatin1String("Show Without Face");
    const QString                          configMatchingConditionEntry         = QLatin1String("Matching Condition");

    QWidget*                               space                                = nullptr;
    QVBoxLayout*                           expanderVlay                         = nullptr;

    TagFilterView*                         tagFilterView                        = nullptr;
    SearchTextBarDb*                       tagFilterSearchBar                   = nullptr;
    QToolButton*                           tagOptionsBtn                        = nullptr;
    QMenu*                                 tagOptionsMenu                       = nullptr;
    TagModel*                              tagFilterModel                       = nullptr;
    QAction*                               tagOrCondAction                      = nullptr;
    QAction*                               tagAndCondAction                     = nullptr;
    ItemFilterSettings::MatchingCondition  tagMatchCond                         = ItemFilterSettings::OrCondition;

    TagFilterView*                         faceFilterView                       = nullptr;
    SearchTextBarDb*                       faceFilterSearchBar                  = nullptr;
    QToolButton*                           faceOptionsBtn                       = nullptr;
    QMenu*                                 faceOptionsMenu                      = nullptr;
    TagModel*                              faceFilterModel                      = nullptr;

    ColorLabelFilter*                      colorLabelFilter                     = nullptr;
    GeolocationFilter*                     geolocationFilter                    = nullptr;
    PickLabelFilter*                       pickLabelFilter                      = nullptr;
    RatingFilter*                          ratingFilter                         = nullptr;
    MimeFilter*                            mimeFilter                           = nullptr;
    TextFilter*                            textFilter                           = nullptr;

    QCheckBox*                             withoutTagCheckBox                   = nullptr;
    QCheckBox*                             withoutFaceCheckBox                  = nullptr;

    DExpanderBox*                          expbox                               = nullptr;
};

// ---------------------------------------------------------------------------------------------------

FilterSideBarWidget::FilterSideBarWidget(QWidget* const parent, TagModel* const tagFilterModel)
    : DVBox            (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(QLatin1String("TagFilter Sidebar"));

    d->expbox = new DExpanderBox(this);
    d->expbox->setObjectName(QLatin1String("FilterSideBarWidget Expander"));

    // --------------------------------------------------------------------------------------------------------

    d->textFilter = new TextFilter(d->expbox);
    d->expbox->addItem(d->textFilter, QIcon::fromTheme(QLatin1String("text-field")),
                       i18n("Text Filter"), QLatin1String("TextFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    d->mimeFilter = new MimeFilter(d->expbox);
    d->expbox->addItem(d->mimeFilter, QIcon::fromTheme(QLatin1String("folder-open")),
                       i18n("MIME Type Filter"), QLatin1String("TypeMimeFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    d->geolocationFilter = new GeolocationFilter(d->expbox);
    d->expbox->addItem(d->geolocationFilter, QIcon::fromTheme(QLatin1String("globe")),
                       i18n("Geolocation Filter"), QLatin1String("TypeGeolocationFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    QWidget* const box3 = new QWidget(d->expbox);
    d->tagFilterModel   = tagFilterModel;
    d->tagFilterView    = new TagFilterView(box3, tagFilterModel);
    d->tagFilterView->setObjectName(QLatin1String("ItemIconViewTagFilterView"));
/*
    d->tagFilterView->filteredModel()->doNotListTagsWithProperty(TagPropertyName::person());
    d->tagFilterView->filteredModel()->setFilterBehavior(AlbumFilterModel::StrictFiltering);
*/
    d->tagFilterSearchBar = new SearchTextBarDb(box3, QLatin1String("ItemIconViewTagFilterSearchBar"));
    d->tagFilterSearchBar->setModel(d->tagFilterView->filteredModel(),
                                    AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);
    d->tagFilterSearchBar->setFilterModel(d->tagFilterView->albumFilterModel());

    const QString notTaggedTitle = i18n("Images Without Tags");
    d->withoutTagCheckBox        = new QCheckBox(notTaggedTitle, box3);
    d->withoutTagCheckBox->setWhatsThis(i18n("Show images without a tag."));

    d->tagOptionsBtn = new QToolButton(box3);
    d->tagOptionsBtn->setToolTip( i18n("Tags Matching Condition"));
    d->tagOptionsBtn->setIcon(QIcon::fromTheme(QLatin1String("configure")));
    d->tagOptionsBtn->setPopupMode(QToolButton::InstantPopup);
    d->tagOptionsBtn->setWhatsThis(i18n("Defines in which way the selected tags are combined "
                                        "to filter the images. This also includes the '%1' check box.",
                                        notTaggedTitle));

    d->tagOptionsMenu                = new QMenu(d->tagOptionsBtn);
    QActionGroup* const tagCondGroup = new QActionGroup(d->tagOptionsMenu);
    tagCondGroup->setExclusive(true);
    d->tagOrCondAction  = d->tagOptionsMenu->addAction(i18n("OR"));
    d->tagOrCondAction->setActionGroup(tagCondGroup);
    d->tagOrCondAction->setCheckable(true);
    d->tagAndCondAction = d->tagOptionsMenu->addAction(i18n("AND"));
    d->tagAndCondAction->setActionGroup(tagCondGroup);
    d->tagAndCondAction->setCheckable(true);
    d->tagOptionsBtn->setMenu(d->tagOptionsMenu);

    QGridLayout* const lay3 = new QGridLayout(box3);
    lay3->addWidget(d->tagFilterView,      0, 0, 1, 3);
    lay3->addWidget(d->tagFilterSearchBar, 1, 0, 1, 3);
    lay3->addWidget(d->withoutTagCheckBox, 2, 0, 1, 1);
    lay3->addWidget(d->tagOptionsBtn,      2, 2, 1, 1);
    lay3->setRowStretch(0, 100);
    lay3->setColumnStretch(1, 10);
    lay3->setContentsMargins(QMargins());
    lay3->setSpacing(0);

    d->expbox->addItem(box3, QIcon::fromTheme(QLatin1String("tag-assigned")), i18n("Tags Filter"), QLatin1String("TagsFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    QWidget* const box5 = new QWidget(d->expbox);
    d->faceFilterModel  = tagFilterModel;
    d->faceFilterView   = new TagFilterView(box5, tagFilterModel);
    d->faceFilterView->setObjectName(QLatin1String("ItemIconViewFaceTagFilterView"));

    d->faceFilterView->filteredModel()->listOnlyTagsWithProperty(TagPropertyName::person());
    d->faceFilterView->filteredModel()->setFilterBehavior(AlbumFilterModel::StrictFiltering);

    d->faceFilterSearchBar = new SearchTextBarDb(box5, QLatin1String("ItemIconViewFaceTagFilterSearchBar"));
    d->faceFilterSearchBar->setModel(d->faceFilterView->filteredModel(),
                                    AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);
    d->faceFilterSearchBar->setFilterModel(d->faceFilterView->albumFilterModel());

    const QString notfaceTaggedTitle = i18n("Images Without Face tags");
    d->withoutFaceCheckBox           = new QCheckBox(notfaceTaggedTitle, box5);
    d->withoutFaceCheckBox->setWhatsThis(i18n("Show images without a face tag."));

    d->faceOptionsBtn = new QToolButton(box5);
    d->faceOptionsBtn->setToolTip( i18n("Face tags Matching Condition"));
    d->faceOptionsBtn->setIcon(QIcon::fromTheme(QLatin1String("configure")));
    d->faceOptionsBtn->setPopupMode(QToolButton::InstantPopup);
    d->faceOptionsBtn->setWhatsThis(i18n("Defines in which way the selected tags are combined "
                                        "to filter the images. This also includes the '%1' check box.",
                                        notfaceTaggedTitle));

    d->faceOptionsMenu = new QMenu(d->faceOptionsBtn);
    d->faceOptionsMenu->addAction(d->tagOrCondAction);
    d->faceOptionsMenu->addAction(d->tagAndCondAction);
    d->faceOptionsBtn->setMenu(d->faceOptionsMenu);

    QGridLayout* const lay5 = new QGridLayout(box5);
    lay5->addWidget(d->faceFilterView,      0, 0, 1, 3);
    lay5->addWidget(d->faceFilterSearchBar, 1, 0, 1, 3);
    lay5->addWidget(d->withoutFaceCheckBox, 2, 0, 1, 1);
    lay5->addWidget(d->faceOptionsBtn,      2, 2, 1, 1);
    lay5->setRowStretch(0, 100);
    lay5->setColumnStretch(1, 10);
    lay5->setContentsMargins(QMargins());
    lay5->setSpacing(0);

    d->expbox->addItem(box5, QIcon::fromTheme(QLatin1String("tag-assigned")), i18n("Face Tags Filter"), QLatin1String("FaceTagsFilter"), true);

    // --------------------------------------------------------------------------------------------------------

    QWidget* const box4 = new QWidget(d->expbox);
    d->colorLabelFilter = new ColorLabelFilter(box4);
    d->pickLabelFilter  = new PickLabelFilter(box4);
    d->ratingFilter     = new RatingFilter(box4);

    QGridLayout* const lay4 = new QGridLayout(box4);
    lay4->addWidget(d->colorLabelFilter, 0, 0, 1, 3);
    lay4->addWidget(d->pickLabelFilter,  1, 0, 1, 1);
    lay4->addWidget(d->ratingFilter,     1, 2, 1, 1);
    lay4->setColumnStretch(2, 1);
    lay4->setColumnStretch(3, 10);
    lay4->setContentsMargins(QMargins());
    lay4->setSpacing(0);

    d->expbox->addItem(box4, QIcon::fromTheme(QLatin1String("folder-favorites")), i18n("Labels Filter"), QLatin1String("LabelsFilter"), true);

    d->expanderVlay = dynamic_cast<QVBoxLayout*>(dynamic_cast<QScrollArea*>(d->expbox)->widget()->layout());
    d->space        = new QWidget();
    d->expanderVlay->addWidget(d->space);

    // --------------------------------------------------------------------------------------------------------

    connect(d->expbox, SIGNAL(signalItemExpanded(int,bool)),
            this, SLOT(slotItemExpanded(int,bool)));

    connect(d->mimeFilter, SIGNAL(activated(int)),
            this, SIGNAL(signalMimeTypeFilterChanged(int)));

    connect(d->geolocationFilter, SIGNAL(signalFilterChanged(ItemFilterSettings::GeolocationCondition)),
            this, SIGNAL(signalGeolocationFilterChanged(ItemFilterSettings::GeolocationCondition)));

    connect(d->textFilter, SIGNAL(signalSearchTextFilterSettings(SearchTextFilterSettings)),
            this, SIGNAL(signalSearchTextFilterChanged(SearchTextFilterSettings)));

    connect(d->tagFilterView, SIGNAL(checkedTagsChanged(QList<TAlbum*>,QList<TAlbum*>)),
            this, SLOT(slotCheckedTagsChanged(QList<TAlbum*>,QList<TAlbum*>)));

    connect(d->colorLabelFilter, SIGNAL(signalColorLabelSelectionChanged(QList<ColorLabel>)),
            this, SLOT(slotColorLabelFilterChanged(QList<ColorLabel>)));

    connect(d->pickLabelFilter, SIGNAL(signalPickLabelSelectionChanged(QList<PickLabel>)),
            this, SLOT(slotPickLabelFilterChanged(QList<PickLabel>)));

    connect(d->withoutTagCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotWithoutTagChanged(int)));

    connect(d->withoutFaceCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(slotWithoutTagChanged(int)));

    connect(d->tagOptionsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotTagOptionsTriggered(QAction*)));

    connect(d->faceOptionsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotTagOptionsTriggered(QAction*)));

    connect(d->tagOptionsMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotTagOptionsMenu()));

    connect(d->faceOptionsMenu, SIGNAL(aboutToShow()),
            this, SLOT(slotTagOptionsMenu()));

    connect(d->ratingFilter, SIGNAL(signalRatingFilterChanged(int,ItemFilterSettings::RatingCondition,bool)),
            this, SIGNAL(signalRatingFilterChanged(int,ItemFilterSettings::RatingCondition,bool)));
}

FilterSideBarWidget::~FilterSideBarWidget()
{
    delete d;
}

void FilterSideBarWidget::slotTagOptionsMenu()
{
    d->tagOrCondAction->setChecked(false);
    d->tagAndCondAction->setChecked(false);

    switch (d->tagMatchCond)
    {
        case ItemFilterSettings::OrCondition:
        {
            d->tagOrCondAction->setChecked(true);
            break;
        }

        case ItemFilterSettings::AndCondition:
        {
            d->tagAndCondAction->setChecked(true);
            break;
        }
    }
}

void FilterSideBarWidget::slotItemExpanded(int id, bool b)
{
    if (id == Digikam::TAGS)
    {
        d->expanderVlay->setStretchFactor(d->space, b ? 0 : 100);
    }
}

void FilterSideBarWidget::setFocusToTextFilter()
{
    d->textFilter->searchTextBar()->setFocus();
}

void FilterSideBarWidget::slotFilterMatchesForText(bool match)
{
    d->textFilter->searchTextBar()->slotSearchResult(match);
}

void FilterSideBarWidget::slotResetFilters()
{
    d->textFilter->reset();
    d->mimeFilter->setMimeFilter(MimeFilter::AllFiles);
    d->geolocationFilter->setGeolocationFilter(ItemFilterSettings::GeolocationNoFilter);
    d->tagFilterView->slotResetCheckState();
    d->withoutTagCheckBox->setChecked(false);
    d->withoutFaceCheckBox->setChecked(false);
    d->colorLabelFilter->reset();
    d->pickLabelFilter->reset();
    d->ratingFilter->reset();
    d->tagMatchCond = ItemFilterSettings::OrCondition;
}

void FilterSideBarWidget::slotTagOptionsTriggered(QAction* action)
{
    if (action)
    {
        if      (action == d->tagOrCondAction)
        {
            d->tagMatchCond = ItemFilterSettings::OrCondition;
        }
        else if (action == d->tagAndCondAction)
        {
            d->tagMatchCond = ItemFilterSettings::AndCondition;
        }
    }

    checkFilterChanges();
}

void FilterSideBarWidget::slotCheckedTagsChanged(const QList<TAlbum*>& includedTags,
                                                 const QList<TAlbum*>& excludedTags)
{
    Q_UNUSED(includedTags);
    Q_UNUSED(excludedTags);

    checkFilterChanges();
}

void FilterSideBarWidget::slotColorLabelFilterChanged(const QList<ColorLabel>& list)
{
    Q_UNUSED(list);

    checkFilterChanges();
}

void FilterSideBarWidget::slotPickLabelFilterChanged(const QList<PickLabel>& list)
{
    Q_UNUSED(list);

    checkFilterChanges();
}

void FilterSideBarWidget::slotWithoutTagChanged(int newState)
{
    Q_UNUSED(newState);

    checkFilterChanges();
}

void FilterSideBarWidget::checkFilterChanges()
{
    bool showUntagged = d->withoutTagCheckBox->isChecked();

    QList<int> includedTagIds;
    QList<int> excludedTagIds;
    QList<int> clTagIds;
    QList<int> plTagIds;

    if (!showUntagged || (d->tagMatchCond == ItemFilterSettings::OrCondition))
    {
        {
            const auto tags = d->tagFilterView->getCheckedTags();

            for (TAlbum* const tag : tags)
            {
                if (tag)
                {
                    includedTagIds << tag->id();
                }
            }
        }

        {
            const auto tags = d->tagFilterView->getPartiallyCheckedTags();

            for (TAlbum* const tag : tags)
            {
                if (tag)
                {
                    excludedTagIds << tag->id();
                }
            }
        }

        if (d->withoutFaceCheckBox->isChecked())
        {
            excludedTagIds << FaceTags::allPersonTags();
        }
    }

    {
        const auto tags = d->colorLabelFilter->getCheckedColorLabelTags();

        for (TAlbum* const tag : tags)
        {
            if (tag)
            {
                clTagIds << tag->id();
            }
        }
    }

    {
        const auto tags = d->pickLabelFilter->getCheckedPickLabelTags();

        for (TAlbum* const tag : tags)
        {
            if (tag)
            {
                plTagIds << tag->id();
            }
        }
    }

    Q_EMIT signalTagFilterChanged(includedTagIds,
                                  excludedTagIds,
                                  d->tagMatchCond,
                                  showUntagged,
                                  clTagIds,
                                  plTagIds);
}

void FilterSideBarWidget::setConfigGroup(const KConfigGroup& group)
{
    StateSavingObject::setConfigGroup(group);
    d->tagFilterView->setConfigGroup(group);
}

void FilterSideBarWidget::doLoadState()
{
    /// @todo mime type and geolocation filter states are not loaded/saved

    KConfigGroup group = getConfigGroup();

    d->expbox->readSettings(group);

    d->textFilter->setsearchTextFields((SearchTextFilterSettings::TextFilterFields)
                                       (group.readEntry(entryName(d->configSearchTextFilterFieldsEntry),
                                                                   (int)SearchTextFilterSettings::All)));


    d->ratingFilter->setRatingFilterCondition((ItemFilterSettings::RatingCondition)
                                              (ApplicationSettings::instance()->getRatingFilterCond()));

    d->tagMatchCond = (ItemFilterSettings::MatchingCondition)
                      (group.readEntry(entryName(d->configMatchingConditionEntry),
                                                  (int)ItemFilterSettings::OrCondition));

    d->tagFilterView->loadState();
    d->faceFilterView->loadState();

    if (d->tagFilterView->isRestoreCheckState())
    {
        d->withoutTagCheckBox->setChecked(group.readEntry(entryName(d->configLastShowUntaggedEntry), false));
    }

    if (d->faceFilterView->isRestoreCheckState())
    {
        d->withoutFaceCheckBox->setChecked(group.readEntry(entryName(d->configLastShowWithoutFaceEntry), false));
    }

    checkFilterChanges();
}

void FilterSideBarWidget::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    d->expbox->writeSettings(group);

    group.writeEntry(entryName(d->configSearchTextFilterFieldsEntry), (int)d->textFilter->searchTextFields());

    ApplicationSettings::instance()->setRatingFilterCond(d->ratingFilter->ratingFilterCondition());

    group.writeEntry(entryName(d->configMatchingConditionEntry), (int)d->tagMatchCond);

    d->tagFilterView->saveState();
    d->faceFilterView->saveState();

    group.writeEntry(entryName(d->configLastShowUntaggedEntry),    d->withoutTagCheckBox->isChecked());
    group.writeEntry(entryName(d->configLastShowWithoutFaceEntry), d->withoutFaceCheckBox->isChecked());
    group.sync();
}

} // namespace Digikam

#include "moc_filtersidebarwidget.cpp"
