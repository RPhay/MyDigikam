/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-05
 * Description : Side Bar Widget for People
 *
 * SPDX-FileCopyrightText: 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2010-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2014      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "peoplesidebarwidget.h"

// Qt includes

#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QPushButton>
#include <QIcon>
#include <QVBoxLayout>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "digikam_debug.h"
#include "searchtextbardb.h"
#include "tagfolderview.h"
#include "facescanwidget.h"
#include "dnotificationwidget.h"
#include "applicationsettings.h"
#include "dexpanderbox.h"
#include "facesengine.h"

namespace Digikam
{

class Q_DECL_HIDDEN PeopleSideBarWidget::Private
{
public:

    Private() = default;

    QPushButton*              rescanButton              = nullptr;
    SearchModificationHelper* searchModificationHelper  = nullptr;
    FaceScanWidget*           settingsWdg               = nullptr;
    TagFolderView*            tagFolderView             = nullptr;
    SearchTextBarDb*          tagSearchBar              = nullptr;
    PeopleSideBarWidget*      parentInstance            = nullptr;
    DLabelExpander*           faceScanExpander          = nullptr;

    int                       ref                       = 1;
};

PeopleSideBarWidget::Private* PeopleSideBarWidget::d = nullptr;

PeopleSideBarWidget::PeopleSideBarWidget(QWidget* const parent,
                                         TagModel* const model,
                                         SearchModificationHelper* const searchModificationHelper)
    : SidebarWidget(parent)
{
    if (!d)
    {
        d                 = new Private();
        d->parentInstance = this;
    }
    else
    {
        ++(d->ref);
    }

    setObjectName(QLatin1String("People Sidebar"));
    setProperty("Shortcut", QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F9));
    d->searchModificationHelper   = searchModificationHelper;

    const int spacing             = layoutSpacing();

    model->setColumnHeader(this->getCaption());

    QVBoxLayout* const vlay = new QVBoxLayout(this);
    d->tagFolderView        = new TagFolderView(this, model);
    d->tagFolderView->setConfigGroup(getConfigGroup());
    d->tagFolderView->setAlbumManagerCurrentAlbum(true);
    d->tagFolderView->setShowDeleteFaceTagsAction(true);

    d->tagFolderView->filteredModel()->listOnlyTagsWithProperty(TagPropertyName::person());
    d->tagFolderView->filteredModel()->setFilterBehavior(AlbumFilterModel::StrictFiltering);

    d->tagSearchBar = new SearchTextBarDb(this, QLatin1String("ItemIconViewPeopleSearchBar"));
    d->tagSearchBar->setHighlightOnResult(true);
    d->tagSearchBar->setModel(d->tagFolderView->filteredModel(),
                              AbstractAlbumModel::AlbumIdRole, AbstractAlbumModel::AlbumTitleRole);
    d->tagSearchBar->setFilterModel(d->tagFolderView->albumFilterModel());

    d->faceScanExpander            = new DLabelExpander(this);
    d->faceScanExpander->setText(i18n("Scan collection for faces"));
    d->faceScanExpander->setIcon(QIcon::fromTheme(QLatin1String("edit-find")));
    d->faceScanExpander->setObjectName(QLatin1String("FaceScanWidgetExpanded"));

    QWidget* const faceScanWdg     = new QWidget(d->faceScanExpander);
    QVBoxLayout* const faceScanLay = new QVBoxLayout(faceScanWdg);

    d->settingsWdg  = new FaceScanWidget(faceScanWdg);
    d->rescanButton = new QPushButton;
    d->rescanButton->setText(i18n("Scan collection for faces"));
    d->rescanButton->setIcon(QIcon::fromTheme(QLatin1String("edit-find")));
    d->rescanButton->setWhatsThis(i18nc("@info", "Use this button to scan the selected albums for faces"));

    faceScanLay->addWidget(d->settingsWdg);
    faceScanLay->addWidget(d->rescanButton);
    faceScanLay->setContentsMargins(0, spacing, 0, 0);

    d->faceScanExpander->setLineVisible(true);
    d->faceScanExpander->setWidget(faceScanWdg);
    d->faceScanExpander->setExpandByDefault(true);
    d->faceScanExpander->layout()->setContentsMargins(0, 0, 0, spacing);

    vlay->addWidget(d->tagFolderView, 10);
    vlay->addWidget(d->tagSearchBar);
    vlay->addWidget(d->faceScanExpander);
    vlay->setContentsMargins(0, spacing, spacing, 0);

    connect(d->tagFolderView, SIGNAL(signalFindDuplicates(QList<TAlbum*>)),
            this, SIGNAL(signalFindDuplicates(QList<TAlbum*>)));

    connect(d->rescanButton, SIGNAL(pressed()),
            this, SLOT(slotScanForFaces()) );
}

PeopleSideBarWidget::~PeopleSideBarWidget()
{
    --(d->ref);

    if (0 == d->ref)
    {
        delete d;
        d = nullptr;
    }
}

void PeopleSideBarWidget::slotInit()
{
    loadState();
}

void PeopleSideBarWidget::setActive(bool active)
{
    Q_EMIT requestFaceMode(active);

    if (active)
    {
        d->tagFolderView->setCurrentAlbums(QList<Album*>() << d->tagFolderView->currentAlbum());

        if (!ApplicationSettings::instance()->getHelpBoxNotificationSeen())
        {
            QString msg = i18n("Welcome to Face Management in digiKam. "
                               "If this is your first time using this feature, please consider "
                               "using the Help Box in the Bottom Left Side Panel.");

            Q_EMIT signalNotificationError(msg, DNotificationWidget::Information);

            ApplicationSettings::instance()->setHelpBoxNotificationSeen(true);
        }
    }
}

void PeopleSideBarWidget::doLoadState()
{
    KConfigGroup group = getConfigGroup();

    d->faceScanExpander->setExpanded(group.readEntry(d->faceScanExpander->objectName(),
                                                     d->faceScanExpander->isExpandByDefault()));
    d->tagFolderView->loadState();
    d->settingsWdg->loadState();
}

void PeopleSideBarWidget::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    group.writeEntry(d->faceScanExpander->objectName(), d->faceScanExpander->isExpanded());

    d->tagFolderView->saveState();
    d->settingsWdg->saveState();
}

void PeopleSideBarWidget::applySettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();
    d->tagFolderView->setExpandNewCurrentItem(settings->getExpandNewCurrentItem());
}

void PeopleSideBarWidget::changeAlbumFromHistory(const QList<Album*>& album)
{
    d->tagFolderView->setCurrentAlbums(album);
}

void PeopleSideBarWidget::slotScanForFaces()
{
    FaceScanSettings faceScanSettings = d->settingsWdg->settings();

    if (!d->settingsWdg->settingsConflicted())
    {

        doFaceScan(faceScanSettings);

    }
    else
    {
        Q_EMIT signalNotificationError(i18n("Face recognition is aborted, because "
                                            "there are no identities to recognize. "
                                            "Please add new identities."),
                                       DNotificationWidget::Information);
    }
}

void PeopleSideBarWidget::slotScanComplete()
{
    d->settingsWdg->setEnabled(true);
    d->rescanButton->setEnabled(true);
}

const QIcon PeopleSideBarWidget::getIcon()
{
    return QIcon::fromTheme(QLatin1String("edit-image-face-show"));
}

const QString PeopleSideBarWidget::getCaption()
{
    return i18nc("Browse images sorted by depicted people", "People");
}

// this should probably be a singleton somewhere else, but here works well
// for controlling the FaceScanWidget

void PeopleSideBarWidget::doFaceScan(const FaceScanSettings& faceScanSettings)
{
    FacesEngine* const facesDetector = new FacesEngine(faceScanSettings);
    facesDetector->start();

    // FacesDetector* const facesDetector = new FacesDetector(faceScanSettings);
    // facesDetector->start();

    connect(facesDetector, SIGNAL(signalComplete()),
            d->parentInstance, SLOT(slotScanComplete()));

    connect(facesDetector, SIGNAL(signalCanceled()),
            d->parentInstance, SLOT(slotScanComplete()));

    connect(facesDetector, SIGNAL(signalScanNotification(QString,int)),
            d->parentInstance, SIGNAL(signalNotificationError(QString,int)));

    d->settingsWdg->setEnabled(false);
    d->rescanButton->setEnabled(false);
}

} // namespace Digikam

#include "moc_peoplesidebarwidget.cpp"
