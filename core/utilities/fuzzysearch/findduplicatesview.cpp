/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-19
 * Description : Find Duplicates View.
 *
 * SPDX-FileCopyrightText: 2016-2017 by Mario Frank <mario dot frank at uni minus potsdam dot de>
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "findduplicatesview.h"

// Qt includes

#include <QHeaderView>
#include <QLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QTimer>
#include <QElapsedTimer>
#include <QGroupBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "albummanager.h"
#include "albumselectors.h"
#include "similaritydb.h"
#include "similaritydbaccess.h"
#include "albumselectcombobox.h"
#include "albumtreeview.h"
#include "findduplicatesalbum.h"
#include "findduplicatesalbumitem.h"
#include "duplicatesfinder.h"
#include "fingerprintsgenerator.h"
#include "applicationsettings.h"
#include "haariface.h"
#include "squeezedcombobox.h"
#include "drangebox.h"

namespace Digikam
{

class Q_DECL_HIDDEN FindDuplicatesView::Private
{

public:

    Private() = default;

    QLabel*              includeAlbumsLabel      = nullptr;
    QLabel*              similarityLabel         = nullptr;
    QLabel*              restrictResultsLabel    = nullptr;
    QLabel*              albumTagRelationLabel   = nullptr;

    DIntRangeBox*        similarityRange         = nullptr;
    SqueezedComboBox*    searchResultRestriction = nullptr;
    SqueezedComboBox*    albumTagRelation        = nullptr;
    SqueezedComboBox*    refImageSelMethod       = nullptr;

    QPushButton*         findDuplicatesBtn       = nullptr;
    QPushButton*         updateFingerPrtBtn      = nullptr;
    QPushButton*         removeDuplicatesBtn     = nullptr;

    FindDuplicatesAlbum* listView                = nullptr;

    ProgressItem*        progressItem            = nullptr;

    AlbumSelectors*      albumSelectors          = nullptr;
    AlbumSelectors*      refImageAlbumSelector   = nullptr;

    ApplicationSettings* settings                = nullptr;

    bool                 active                  = false;
};

FindDuplicatesView::FindDuplicatesView(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);

    d->settings       = ApplicationSettings::instance();

    const int spacing = layoutSpacing();


    // ---------------------------------------------------------------

    d->listView           = new FindDuplicatesAlbum();
    d->listView->setSortingEnabled(false);

    d->updateFingerPrtBtn = new QPushButton(i18nc("@action", "Update fingerprints"));
    d->updateFingerPrtBtn->setIcon(QIcon::fromTheme(QLatin1String("run-build")));
    d->updateFingerPrtBtn->setWhatsThis(i18nc("@info", "Use this button to update all image fingerprints."));

    d->findDuplicatesBtn  = new QPushButton(i18nc("@action", "Find duplicates"));
    d->findDuplicatesBtn->setIcon(QIcon::fromTheme(QLatin1String("edit-find")));
    d->findDuplicatesBtn->setWhatsThis(i18nc("@info", "Use this button to scan the selected albums for "
                                            "duplicate items."));

    // ---------------------------------------------------------------

    d->albumSelectors  = new AlbumSelectors(i18nc("@label", "Search in:"), QLatin1String("Find Duplicates View"),
                                            nullptr, AlbumSelectors::AlbumType::All, true);

    // ---------------------------------------------------------------

    d->similarityRange = new DIntRangeBox();
    d->similarityRange->setSuffix(QLatin1String("%"));

    if (d->settings)
    {
        d->similarityRange->setRange(d->settings->getMinimumSimilarityBound(), 100);
        updateSimilarityRangeInterval();
    }
    else
    {
        d->similarityRange->setRange(40, 100);
        d->similarityRange->setInterval(40, 100);
    }

    d->similarityLabel         = new QLabel(i18nc("@label", "Similarity range:"));
    d->similarityLabel->setBuddy(d->similarityRange);

    d->restrictResultsLabel    = new QLabel(i18nc("@label", "Restriction:"));
    d->restrictResultsLabel->setBuddy(d->searchResultRestriction);

    d->searchResultRestriction = new SqueezedComboBox();
    d->searchResultRestriction->addSqueezedItem(i18nc("@label:listbox similarity restriction", "None"),                        HaarIface::DuplicatesSearchRestrictions::None);
    d->searchResultRestriction->addSqueezedItem(i18nc("@label:listbox similarity restriction", "Restrict to reference album"), HaarIface::DuplicatesSearchRestrictions::SameAlbum);
    d->searchResultRestriction->addSqueezedItem(i18nc("@label:listbox similarity restriction", "Exclude reference album"),     HaarIface::DuplicatesSearchRestrictions::DifferentAlbum);
    d->searchResultRestriction->setToolTip(i18nc("@info", "Use this option to restrict the duplicate search "
                                                          "with some criteria, as to limit search to the album "
                                                          "of reference image, or to exclude the album of "
                                                          "reference image of the search."));

    // Load the last choice from application settings.

    HaarIface::DuplicatesSearchRestrictions restrictions
        = d->settings ? (HaarIface::DuplicatesSearchRestrictions) d->settings->getDuplicatesSearchRestrictions()
                      : HaarIface::DuplicatesSearchRestrictions::None;

    d->searchResultRestriction->setCurrentIndex(d->searchResultRestriction->findData(restrictions));

    d->albumTagRelationLabel = new QLabel(i18nc("@label", "Restrict to:"));
    d->albumTagRelationLabel->setBuddy(d->albumTagRelation);

    /*
     * only selected tab   => search duplicates in albums or tags, use the viewed tab and ignore the other tab.
     * one of              => search duplicates being in the selected albums or having the selected tags, use checkbox selection.
     * both                => search duplicates that are both in one of the selected albums and have at least one of the selected tags.
     * albums but not tags => search only duplicates in the selected albums that do not have the selected tags.
     * tags but not albums => vice versa.
     */

    d->albumTagRelation      = new SqueezedComboBox();
    d->albumTagRelation->addSqueezedItem(i18nc("@label:listbox similarity album tag relation",
                                               "Only selected tab"),
                                         (int)HaarIface::AlbumTagRelation::NoMix);
    d->albumTagRelation->addSqueezedItem(i18nc("@label:listbox similarity album tag relation",
                                               "One of"),
                                         (int)HaarIface::AlbumTagRelation::Union);
    d->albumTagRelation->addSqueezedItem(i18nc("@label:listbox similarity album tag relation",
                                               "Both"),
                                         (int)HaarIface::AlbumTagRelation::Intersection);
    d->albumTagRelation->addSqueezedItem(i18nc("@label:listbox similarity album tag relation",
                                               "Albums but not tags"),
                                         (int)HaarIface::AlbumTagRelation::AlbumExclusive);
    d->albumTagRelation->addSqueezedItem(i18nc("@label:listbox similarity album tag relation",
                                               "Tags but not albums"),
                                         (int)HaarIface::AlbumTagRelation::TagExclusive);
    d->albumTagRelation->setCurrentIndex(ApplicationSettings::instance()->getDuplicatesAlbumTagRelation());

    const auto refSelMethod            = ApplicationSettings::instance()->getDuplicatesRefImageSelMethod();
    d->refImageSelMethod               = new SqueezedComboBox();
    d->refImageSelMethod->addSqueezedItem(i18nc("@label:listbox similarity reference selection method",
                                                "Older or Larger"),
                                          (int)HaarIface::RefImageSelMethod::OlderOrLarger);
    d->refImageSelMethod->addSqueezedItem(i18nc("@label:listbox similarity reference selection method",
                                                "Prefer selected folder as reference"),
                                          (int)HaarIface::RefImageSelMethod::PreferFolder);
    d->refImageSelMethod->addSqueezedItem(i18nc("@label:listbox similarity reference selection method",
                                                "Prefer other than selected folder as reference"),
                                          (int)HaarIface::RefImageSelMethod::ExcludeFolder);
    d->refImageSelMethod->addSqueezedItem(i18nc("@label:listbox similarity reference selection method",
                                                "Prefer Newer Creation Date"),
                                          (int)HaarIface::RefImageSelMethod::NewerCreationDate);
    d->refImageSelMethod->addSqueezedItem(i18nc("@label:listbox similarity reference selection method",
                                                "Prefer Newer Modification Date"),
                                          (int)HaarIface::RefImageSelMethod::NewerModificationDate);
    d->refImageSelMethod->setCurrentIndex(d->refImageSelMethod->findData((int)refSelMethod));
    d->refImageSelMethod->setWhatsThis(i18nc("@label:listbox similarity reference selection method",
                                             "Select method for reference image selection"));

    d->refImageAlbumSelector           = new AlbumSelectors(i18nc("@label", "Reference"),
                                                            QLatin1String("Select Reference"),
                                                            nullptr, AlbumSelectors::AlbumType::PhysAlbum, true);
    d->refImageAlbumSelector->setVisible((refSelMethod == HaarIface::RefImageSelMethod::ExcludeFolder) ||
                                         (refSelMethod == HaarIface::RefImageSelMethod::PreferFolder));

    auto* const refImageselectionGroup = new QGroupBox(i18nc("@group", "Reference Image Selection Method"));
    auto* const layout                 = new QVBoxLayout;
    layout->addWidget(d->refImageSelMethod);
    layout->addWidget(d->refImageAlbumSelector);
    refImageselectionGroup->setLayout(layout);

    QString tip = i18nc("@info",
                        "Use this option to decide about the relation of the selected albums and tags.\n"
                        "\"One of\" means that the images are either in the selected albums or tags.\n"
                        "\"Both\" means that the images are both in the selected albums and tags.\n"
                        "\"Albums but not tags\" means that images must be in the selected albums but not tags.\n"
                        "\"Tags but not albums\" means that images must be in the selected tags but not albums.\n"
                        "\"Only selected tab\" means that only the selected tab is used.");
    d->albumTagRelation->setToolTip(tip);

    // Load the last choice from application settings.

    HaarIface::AlbumTagRelation relation
        = d->settings ? (HaarIface::AlbumTagRelation) d->settings->getDuplicatesAlbumTagRelation()
                      : HaarIface::AlbumTagRelation::NoMix;

    d->albumTagRelation->setCurrentIndex(d->albumTagRelation->findData(relation));

    d->removeDuplicatesBtn = new QPushButton(i18nc("@action", "Remove Duplicates"));
    d->removeDuplicatesBtn->setIcon(QIcon::fromTheme(QLatin1String("user-trash")));
    d->removeDuplicatesBtn->setWhatsThis(i18nc("@info", "Use this button to delete all duplicate images."));

    d->removeDuplicatesBtn->setEnabled(false);

    // ---------------------------------------------------------------

    QGridLayout* const mainLayout = new QGridLayout();
    mainLayout->addWidget(d->listView,                0, 0, 1, -1);
    mainLayout->addWidget(refImageselectionGroup,     1, 0, 1, -1);
    mainLayout->addWidget(d->albumTagRelationLabel,   2, 0, 1,  2);
    mainLayout->addWidget(d->albumTagRelation,        2, 2, 1, -1);
    mainLayout->addWidget(d->albumSelectors,          3, 0, 1, -1);
    mainLayout->addWidget(d->similarityLabel,         4, 0, 1,  1);
    mainLayout->addWidget(d->similarityRange,         4, 2, 1,  1);
    mainLayout->addWidget(d->restrictResultsLabel,    5, 0, 1,  2);
    mainLayout->addWidget(d->searchResultRestriction, 5, 2, 1, -1);
    mainLayout->addWidget(d->updateFingerPrtBtn,      6, 0, 1, -1);
    mainLayout->addWidget(d->findDuplicatesBtn,       7, 0, 1, -1);
    mainLayout->addWidget(d->removeDuplicatesBtn,     8, 0, 1, -1);

    mainLayout->setRowStretch(0, 10);
    mainLayout->setColumnStretch(2, 10);
    mainLayout->setContentsMargins(spacing, spacing, spacing, spacing);
    mainLayout->setSpacing(spacing);
    setLayout(mainLayout);

    // ---------------------------------------------------------------

    connect(d->updateFingerPrtBtn, SIGNAL(clicked()),
            this, SLOT(slotUpdateFingerPrints()));

    connect(d->findDuplicatesBtn, SIGNAL(clicked()),
            this, SLOT(slotFindDuplicates()));

    connect(d->removeDuplicatesBtn, SIGNAL(clicked()),
            this, SLOT(slotRemoveDuplicates()));

    connect(d->listView, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotDuplicatesAlbumActived()));

    connect(d->albumSelectors, SIGNAL(signalSelectionChanged()),
            this, SLOT(slotCheckForValidSettings()));

    connect(AlbumManager::instance(), SIGNAL(signalAllAlbumsLoaded()),
            this, SLOT(initAlbumUpdateConnections()));

    connect(d->settings, SIGNAL(setupChanged()),
            this, SLOT(slotApplicationSettingsChanged()));

    connect(d->refImageSelMethod, QOverload<int>::of(&SqueezedComboBox::currentIndexChanged),
            this, &FindDuplicatesView::slotReferenceSelectionMethodChanged);
}

FindDuplicatesView::~FindDuplicatesView()
{
    d->albumSelectors->saveState();
    d->refImageAlbumSelector->saveState();

    delete d;
}

void FindDuplicatesView::initAlbumUpdateConnections()
{
    connect(AlbumManager::instance(), SIGNAL(signalAlbumAdded(Album*)),
            this, SLOT(slotAlbumAdded(Album*)));

    connect(AlbumManager::instance(), SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotAlbumDeleted(Album*)));

    connect(AlbumManager::instance(), SIGNAL(signalSearchUpdated(SAlbum*)),
            this, SLOT(slotSearchUpdated(SAlbum*)));

    connect(AlbumManager::instance(), SIGNAL(signalAlbumsCleared()),
            this, SLOT(slotClear()));

    connect(AlbumManager::instance(), SIGNAL(signalUpdateDuplicatesAlbums(QList<SAlbum*>,QList<qlonglong>)),
            this, SLOT(slotUpdateDuplicates(QList<SAlbum*>,QList<qlonglong>)));
}

void FindDuplicatesView::setActive(bool val)
{
    d->active = val;
    slotCheckForValidSettings();
    d->removeDuplicatesBtn->setEnabled(false);

    QTimer::singleShot(250, this, SLOT(slotSelectItemsTimer()));
}

void FindDuplicatesView::populateTreeView()
{
    if (d->listView->topLevelItemCount() > 0)
    {
        return;
    }

    QElapsedTimer waitCursorTimer;
    waitCursorTimer.start();

    bool waitCursor = false;

    const AlbumList& aList = AlbumManager::instance()->allSAlbums();

    for (AlbumList::const_iterator it = aList.constBegin() ; it != aList.constEnd() ; ++it)
    {
        SAlbum* const salbum = dynamic_cast<SAlbum*>(*it);

        if (salbum && salbum->isDuplicatesSearch() && !salbum->extraData(this))
        {
            // Adding item to listView by creating an item and passing listView as parent

            FindDuplicatesAlbumItem* const item = new FindDuplicatesAlbumItem(d->listView, salbum);
            salbum->setExtraData(this, item);
        }

        if (!waitCursor && (waitCursorTimer.elapsed() > 2000))
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            waitCursor = true;
        }
    }

    d->listView->setSortingEnabled(true);
    d->listView->resizeColumnToContents(0);
    d->listView->sortByColumn(1, Qt::DescendingOrder);

    d->albumSelectors->loadState();
    d->refImageAlbumSelector->loadState();

    if (waitCursor)
    {
        QApplication::restoreOverrideCursor();
    }
}

QList<SAlbum*> FindDuplicatesView::currentFindDuplicatesAlbums() const
{
    QList<QTreeWidgetItem*> selectedItems = d->listView->selectedItems();

    if (selectedItems.isEmpty())
    {
        QTreeWidgetItem* const item = d->listView->firstItem();

        if (item)
        {
            selectedItems << item;
        }
    }

    QList<SAlbum*> albumList;

    Q_FOREACH (QTreeWidgetItem* const item, selectedItems)
    {
        FindDuplicatesAlbumItem* const albumItem = dynamic_cast<FindDuplicatesAlbumItem*>(item);

        if (albumItem)
        {
            albumList << albumItem->album();
        }
    }

    return albumList;
}

void FindDuplicatesView::slotAlbumAdded(Album* a)
{
    if (!a || (a->type() != Album::SEARCH))
    {
        return;
    }

    SAlbum* const salbum = static_cast<SAlbum*>(a);

    if (!salbum->isDuplicatesSearch())
    {
        return;
    }

    if (!d->active)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Duplicates view is not active, returning";
        return;
    }

    if (!salbum->extraData(this))
    {
        FindDuplicatesAlbumItem* const item = new FindDuplicatesAlbumItem(d->listView, salbum);
        salbum->setExtraData(this, item);
    }

    updateSimilarityRangeInterval();
}

void FindDuplicatesView::slotAlbumDeleted(Album* a)
{
    if (!a || (a->type() != Album::SEARCH))
    {
        return;
    }

    SAlbum* const album = static_cast<SAlbum*>(a);

    FindDuplicatesAlbumItem* const item = static_cast<FindDuplicatesAlbumItem*>(album->extraData(this));

    if (item)
    {
        a->removeExtraData(this);
        delete item;
    }

    updateSimilarityRangeInterval();
}

void FindDuplicatesView::slotSearchUpdated(SAlbum* a)
{
    if (!a->isDuplicatesSearch())
    {
        return;
    }

    slotAlbumDeleted(a);
    slotAlbumAdded(a);
}

void FindDuplicatesView::slotSelectItemsTimer()
{
    if (d->listView->selectedItems().isEmpty())
    {
        d->listView->selectFirstItem();
    }

    slotDuplicatesAlbumActived();
}

void FindDuplicatesView::slotClear()
{
    for (QTreeWidgetItemIterator it(d->listView) ; *it ; ++it)
    {
        SAlbum* const salbum = static_cast<FindDuplicatesAlbumItem*>(*it)->album();

        if (salbum)
        {
            salbum->removeExtraData(this);
        }
    }

    d->listView->clear();
}

void FindDuplicatesView::enableControlWidgets(bool val)
{
    d->searchResultRestriction->setEnabled(val);
    d->removeDuplicatesBtn->setEnabled(val);
    d->updateFingerPrtBtn->setEnabled(val);
    d->findDuplicatesBtn->setEnabled(val);
    d->albumTagRelation->setEnabled(val);
    d->similarityRange->setEnabled(val);
    d->albumSelectors->setEnabled(val);
    d->refImageSelMethod->setEnabled(val);

    d->albumTagRelationLabel->setEnabled(val);
    d->restrictResultsLabel->setEnabled(val);
    d->similarityLabel->setEnabled(val);
}

void FindDuplicatesView::slotFindDuplicates()
{
    d->albumSelectors->saveState();
    d->refImageAlbumSelector->saveState();
    enableControlWidgets(false);
    slotClear();

    AlbumList albums;
    AlbumList tags;
    AlbumList referenceImageSelector;

    if (d->albumTagRelation->itemData(d->albumTagRelation->currentIndex()).toInt() == HaarIface::AlbumTagRelation::NoMix)
    {
        if      (d->albumSelectors->typeSelection() == AlbumSelectors::AlbumType::PhysAlbum)
        {
            albums = d->albumSelectors->selectedAlbums();
        }
        else if (d->albumSelectors->typeSelection() == AlbumSelectors::AlbumType::TagsAlbum)
        {
            tags  = d->albumSelectors->selectedTags();
        }
    }
    else
    {
        albums = d->albumSelectors->selectedAlbums();
        tags   = d->albumSelectors->selectedTags();
    }

    const auto referenceImageSelectionMethod = (HaarIface::RefImageSelMethod)d->refImageSelMethod->itemData(d->refImageSelMethod->currentIndex()).toInt();

    if (referenceImageSelectionMethod != HaarIface::RefImageSelMethod::OlderOrLarger)
    {
        referenceImageSelector = d->refImageAlbumSelector->selectedAlbums();
    }

    AlbumManager::instance()->clearCurrentAlbums();
    SimilarityDbAccess().db()->clearImageSimilarity();

    DuplicatesFinder* const finder = new DuplicatesFinder(albums, tags,
                                                          d->albumTagRelation->itemData(d->albumTagRelation->currentIndex()).toInt(),
                                                          d->similarityRange->minValue(), d->similarityRange->maxValue(),
                                                          d->searchResultRestriction->itemData(d->searchResultRestriction->currentIndex()).toInt(),
                                                          referenceImageSelectionMethod, referenceImageSelector);

    connect(finder, SIGNAL(signalComplete()),
            this, SLOT(slotComplete()));

    connect(finder, &DuplicatesFinder::signalComplete,
            this, [this]
       {
            d->removeDuplicatesBtn->setEnabled(true);
       }
    );

    connect(finder, &DuplicatesFinder::signalCanceled,
            this, [this]
       {
            d->removeDuplicatesBtn->setEnabled(false);
       }
    );

    finder->start();
}

void FindDuplicatesView::slotUpdateDuplicates(const QList<SAlbum*>& sAlbumsToRebuild,
                                              const QList<qlonglong>& deletedImages)
{
    d->listView->updateDuplicatesAlbumItems(sAlbumsToRebuild, deletedImages);
}

void FindDuplicatesView::slotApplicationSettingsChanged()
{
    d->similarityRange->setRange(d->settings->getMinimumSimilarityBound(), 100);
}

void FindDuplicatesView::slotReferenceSelectionMethodChanged(int index)
{
   auto method = static_cast<HaarIface::RefImageSelMethod>(d->refImageSelMethod->itemData(index).toInt());

   d->refImageAlbumSelector->setVisible(
                                        (method == HaarIface::RefImageSelMethod::ExcludeFolder) ||
                                        (method == HaarIface::RefImageSelMethod::PreferFolder)
                                       );
}

void FindDuplicatesView::slotComplete()
{
    enableControlWidgets(true);
    slotCheckForValidSettings();
    populateTreeView();

    QTimer::singleShot(250, this, SLOT(slotSelectItemsTimer()));
}

void FindDuplicatesView::slotDuplicatesAlbumActived()
{
    if (!d->active)
    {
        return;
    }

    QList<Album*> albums;

    Q_FOREACH (QTreeWidgetItem* const item, d->listView->selectedItems())
    {
        FindDuplicatesAlbumItem* const albumItem = dynamic_cast<FindDuplicatesAlbumItem*>(item);

        if (albumItem)
        {
            albums << albumItem->album();
        }
    }

    if (!albums.isEmpty())
    {
        AlbumManager::instance()->setCurrentAlbums(QList<Album*>() << albums);
    }
}

void FindDuplicatesView::slotCheckForValidSettings()
{
    bool valid = (
                  d->albumSelectors->selectedAlbums().count() ||
                  d->albumSelectors->selectedTags().count()
                 );

    d->findDuplicatesBtn->setEnabled(valid);
}

void FindDuplicatesView::slotUpdateFingerPrints()
{
    FingerPrintsGenerator* const tool = new FingerPrintsGenerator(false);
    tool->start();
}

void FindDuplicatesView::slotSetSelectedAlbums(const QList<PAlbum*>& albums)
{
    // @ODD: Why is singleton set to true? resetAlbumsAndTags already clears the selection.

    resetAlbumsAndTags();

    const AlbumSelectors::SelectionType type = albums.size() == 1 ? AlbumSelectors::SingleSelection
                                                                  : AlbumSelectors::MultipleSelection;

    Q_FOREACH (PAlbum* const album, albums)
    {
        d->albumSelectors->setAlbumSelected(album, type);
    }

    d->albumSelectors->setTypeSelection(AlbumSelectors::AlbumType::PhysAlbum);
    d->albumTagRelation->setCurrentIndex(d->albumTagRelation->findData(HaarIface::AlbumTagRelation::NoMix));
    d->searchResultRestriction->setCurrentIndex(d->searchResultRestriction->findData(HaarIface::DuplicatesSearchRestrictions::None));
    slotCheckForValidSettings();

    if (d->findDuplicatesBtn->isEnabled())
    {
        slotFindDuplicates();
    }
}

void FindDuplicatesView::slotSetSelectedAlbums(const QList<TAlbum*>& albums)
{
    resetAlbumsAndTags();

    const AlbumSelectors::SelectionType type = albums.size() == 1 ? AlbumSelectors::SingleSelection
                                                                  : AlbumSelectors::MultipleSelection;

    Q_FOREACH (TAlbum* const album, albums)
    {
        d->albumSelectors->setTagSelected(album, type);
    }

    d->albumSelectors->setTypeSelection(AlbumSelectors::AlbumType::TagsAlbum);
    d->albumTagRelation->setCurrentIndex(d->albumTagRelation->findData(HaarIface::AlbumTagRelation::NoMix));
    d->searchResultRestriction->setCurrentIndex(d->searchResultRestriction->findData(HaarIface::DuplicatesSearchRestrictions::None));
    slotCheckForValidSettings();

    if (d->findDuplicatesBtn->isEnabled())
    {
        slotFindDuplicates();
    }
}

void FindDuplicatesView::updateSimilarityRangeInterval()
{
    d->similarityRange->setInterval(d->settings->getDuplicatesSearchLastMinSimilarity(),
                                    d->settings->getDuplicatesSearchLastMaxSimilarity());
}

void FindDuplicatesView::resetAlbumsAndTags()
{
    d->albumSelectors->resetSelection();
    slotCheckForValidSettings();
}

void FindDuplicatesView::slotRemoveDuplicates()
{
    d->listView->removeDuplicates();
}

} // namespace Digikam

#include "moc_findduplicatesview.cpp"
