/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-23
 * Description : mics configuration setup tab
 *
 * SPDX-FileCopyrightText: 2004      by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2005-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupmisc_p.h"

namespace Digikam
{

SetupMisc::SetupMisc(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->tab            = new QTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);

    const int spacing = layoutSpacing();

    // -- Application Behavior Options --------------------------------------------------------

    QWidget* const behaviourPanel = new QWidget(d->tab);
    QGridLayout* const layout     = new QGridLayout(behaviourPanel);

    DHBox* const albumDateSourceHbox  = new DHBox(behaviourPanel);
    d->albumDateSourceLabel           = new QLabel(i18n("Get album date source:"), albumDateSourceHbox);
    d->albumDateSource                = new QComboBox(albumDateSourceHbox);
    d->albumDateSource->addItem(i18nc("method to get the album date", "From The Newest Item Date"), MetaEngineSettingsContainer::NewestItemDate);
    d->albumDateSource->addItem(i18nc("method to get the album date", "From The Oldest Item Date"), MetaEngineSettingsContainer::OldestItemDate);
    d->albumDateSource->addItem(i18nc("method to get the album date", "From The Average Date"),     MetaEngineSettingsContainer::AverageDate);
    d->albumDateSource->addItem(i18nc("method to get the album date", "From The Folder Date"),      MetaEngineSettingsContainer::FolderDate);
    d->albumDateSource->addItem(i18nc("method to get the album date", "No Change Of Date"),         MetaEngineSettingsContainer::IgnoreDate);
    d->albumDateSource->setToolTip(i18n("The album date is adjusted depending on the option set "
                                        "when new items are found during scanning."));

    DHBox* const stringComparisonHbox = new DHBox(behaviourPanel);
    d->stringComparisonTypeLabel      = new QLabel(i18n("String comparison type:"), stringComparisonHbox);
    d->stringComparisonType           = new QComboBox(stringComparisonHbox);
    d->stringComparisonType->addItem(i18nc("method to compare strings", "Natural"), ApplicationSettings::Natural);
    d->stringComparisonType->addItem(i18nc("method to compare strings", "Normal"),  ApplicationSettings::Normal);
    d->stringComparisonType->setToolTip(i18n("<qt>Sets the way in which strings are compared inside digiKam. "
                                             "This eg. influences the sorting of the tree views.<br/>"
                                             "<b>Natural</b> tries to compare strings in a way that regards some normal conventions "
                                             "and will eg. result in sorting numbers naturally even if they have a different number of digits.<br/>"
                                             "<b>Normal</b> uses a more technical approach. "
                                             "Use this style if you eg. want to entitle albums with ISO dates (201006 or 20090523) "
                                             "and the albums should be sorted according to these dates.</qt>"));

    // --------------------------------------------------------

    d->showTrashDeleteDialogCheck     = new QCheckBox(i18n("Confirm when moving items to the &trash"), behaviourPanel);
    d->showPermanentDeleteDialogCheck = new QCheckBox(i18n("Confirm when permanently deleting items"), behaviourPanel);
    d->sidebarApplyDirectlyCheck      = new QCheckBox(i18n("Do not confirm when applying changes in the &right sidebar"), behaviourPanel);
    d->scanAtStart                    = new QCheckBox(i18n("&Scan for new items at startup (makes startup slower)"), behaviourPanel);
    d->scanAtStart->setToolTip(i18n("Set this option to force digiKam to scan all collections for new items to\n"
                                    "register new elements in database. The scan is performed in the background through\n"
                                    "the progress manager available in the statusbar\n when digiKam main interface\n"
                                    "is loaded. If your computer is fast enough, this will have no effect on usability\n"
                                    "of digiKam while scanning. If your collections are huge or if you use a remote database,\n"
                                    "this can introduce low latency, and it is recommended to disable this option and to plan\n"
                                    "a manual scan through the maintenance tool at the right moment."));

    d->useFastScan                    = new QCheckBox(i18n("Fast scan (detects new, deleted and renamed items)"), behaviourPanel);
    d->useFastScan->setToolTip(i18n("Set this option to significantly speed up the scan. New items, deleted and also\n"
                                    "renamed items are found. In order to find items that have been changed, this\n"
                                    "option must be deactivated."));

    d->useFastScan->setEnabled(false);

    d->detectFaces                    = new QCheckBox(i18n("Detect faces in newly added images"), behaviourPanel);

    d->cleanAtStart                   = new QCheckBox(i18n("Remove obsolete core database objects (makes startup slower)"), behaviourPanel);
    d->cleanAtStart->setToolTip(i18n("Set this option to force digiKam to clean up the core database from obsolete item entries.\n"
                                     "Entries are only deleted if the connected image/video/audio file was already removed, i.e.\n"
                                     "the database object wastes space.\n"
                                     "This option does not clean up other databases as the thumbnails or recognition db.\n"
                                     "For clean up routines for other databases, please use the maintenance."));

    d->selectFirstAlbumItemCheck              = new QCheckBox(i18n("Initially select the first item in the album"), behaviourPanel);
    d->expandNewCurrentItemCheck              = new QCheckBox(i18n("Expand current tree item with a single mouse click"), behaviourPanel);
    d->scrollItemToCenterCheck                = new QCheckBox(i18n("Scroll current item to center of thumbbar"), behaviourPanel);
    d->showOnlyPersonTagsInPeopleSidebarCheck = new QCheckBox(i18n("Show only face tags for assigning names in people sidebar"), behaviourPanel);


    // ---------------------------------------------------------

    DHBox* const minSimilarityBoundHbox       = new DHBox(behaviourPanel);
    d->minSimilarityBoundLabel                = new QLabel(i18n("Lower bound for minimum similarity:"), minSimilarityBoundHbox);
    d->minimumSimilarityBound                 = new QSpinBox(minSimilarityBoundHbox);
    d->minimumSimilarityBound->setSuffix(QLatin1String("%"));
    d->minimumSimilarityBound->setRange(1, 100);
    d->minimumSimilarityBound->setSingleStep(1);
    d->minimumSimilarityBound->setValue(40);
    d->minimumSimilarityBound->setToolTip(i18n("Select here the lower bound of "
                                               "the minimum similarity threshold "
                                               "for fuzzy and duplicates searches. "
                                               "The default value is 40. Selecting "
                                               "a lower value than 40 can make the search <b>really</b> slow."));
    d->minSimilarityBoundLabel->setBuddy(d->minimumSimilarityBound);

    // ---------------------------------------------------------

    QGroupBox* const upOptionsGroup = new QGroupBox(i18n("Updates"), behaviourPanel);
    QVBoxLayout* const gLayout5     = new QVBoxLayout();

    DHBox* const updateHbox = new DHBox(upOptionsGroup);
    d->updateTypeLabel      = new QLabel(i18n("Check for new version:"), updateHbox);
    d->updateType           = new QComboBox(updateHbox);
    d->updateType->addItem(i18n("Only For Stable Releases"), 0);
    d->updateType->addItem(i18n("Weekly Pre-Releases"),      1);
    d->updateType->setToolTip(i18n("Set this option to configure which kind of new versions must be check for updates.\n"
                                   "\"Stable\" releases are official versions safe to use in production.\n"
                                   "\"Pre-releases\" are proposed weekly to tests quickly new features\n"
                                   "and are not recommended to use in production as bugs can remain."));

    d->updateWithDebug = new QCheckBox(i18n("Use Version With Debug Symbols"), upOptionsGroup);
    d->updateWithDebug->setWhatsThis(i18n("If this option is enabled, a version including debug symbols will be used for updates.\n"
                                          "This version is more heavy but can help developers to trace dysfunctions in debugger."));

    DHBox* const updateHbox2     = new DHBox(upOptionsGroup);
    QLabel* const lastCheckLabel = new QLabel(updateHbox2);
    lastCheckLabel->setText(i18n("Last check: %1", OnlineVersionChecker::lastCheckDate()));
    QPushButton* const updateNow = new QPushButton(i18n("Check now..."), updateHbox2);

    connect(updateNow, &QPushButton::pressed,
            this, [this, parent]()
        {
            if (!checkSettings())
            {
                return;
            }

            applySettings();

            if (parent)
            {
                parent->close();
            }

            Setup::onlineVersionCheck();
        }
    );

    gLayout5->addWidget(updateHbox);
    gLayout5->addWidget(d->updateWithDebug);
    gLayout5->addWidget(updateHbox2);
    upOptionsGroup->setLayout(gLayout5);

    // ---------------------------------------------------------

    layout->setContentsMargins(spacing, spacing, spacing, spacing);
    layout->setSpacing(spacing);
    layout->addWidget(d->scanAtStart,                             0, 0, 1, 4);
    layout->addWidget(d->useFastScan,                             1, 3, 1, 1);
    layout->addWidget(d->detectFaces,                             2, 0, 1, 4);
    layout->addWidget(d->cleanAtStart,                            3, 0, 1, 4);
    layout->addWidget(new DLineWidget(Qt::Horizontal),            4, 0, 1, 4);
    layout->addWidget(d->showTrashDeleteDialogCheck,              5, 0, 1, 4);
    layout->addWidget(d->showPermanentDeleteDialogCheck,          6, 0, 1, 4);
    layout->addWidget(d->sidebarApplyDirectlyCheck,               7, 0, 1, 4);
    layout->addWidget(d->showOnlyPersonTagsInPeopleSidebarCheck,  8, 0, 1, 4);
    layout->addWidget(d->selectFirstAlbumItemCheck,               9, 0, 1, 4);
    layout->addWidget(d->expandNewCurrentItemCheck,              10, 0, 1, 4);
    layout->addWidget(d->scrollItemToCenterCheck,                11, 0, 1, 4);
    layout->addWidget(albumDateSourceHbox,                       12, 0, 1, 4);
    layout->addWidget(stringComparisonHbox,                      13, 0, 1, 4);
    layout->addWidget(minSimilarityBoundHbox,                    14, 0, 1, 4);
    layout->addWidget(upOptionsGroup,                            15, 0, 1, 4);
    layout->setColumnStretch(3, 10);
    layout->setRowStretch(16, 10);

    // --------------------------------------------------------

    connect(d->scanAtStart, SIGNAL(toggled(bool)),
            d->useFastScan, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    d->tab->insertTab(Behaviour, behaviourPanel, i18nc("@title:tab", "Behaviour"));

    // -- Application Appearance Options --------------------------------------------------------

    QWidget* const appearancePanel = new QWidget(d->tab);
    QVBoxLayout* const layout2     = new QVBoxLayout(appearancePanel);

    d->showSplashCheck             = new QCheckBox(i18n("&Show splash screen at startup"), appearancePanel);
    d->useNativeFileDialogCheck    = new QCheckBox(i18n("Use native file dialogs from system"), appearancePanel);
    d->drawFramesToGroupedCheck    = new QCheckBox(i18n("Draw frames around grouped items"), appearancePanel);

    DHBox* const tabStyleHbox = new DHBox(appearancePanel);
    d->sidebarTypeLabel       = new QLabel(i18n("Sidebar tab title:"), tabStyleHbox);
    d->sidebarType            = new QComboBox(tabStyleHbox);
    d->sidebarType->addItem(i18n("Only For Active Tab"), 0);
    d->sidebarType->addItem(i18n("For All Tabs"),        1);
    d->sidebarType->setToolTip(i18n("Set this option to configure how sidebar tab titles are visible. "
                                    "Use \"Only For Active Tab\" option if you use a small screen resolution as with a laptop computer."));

    DHBox* const appStyleHbox = new DHBox(appearancePanel);
    d->applicationStyleLabel  = new QLabel(i18n("Widget style:"), appStyleHbox);
    d->applicationStyle       = new QComboBox(appStyleHbox);
    d->applicationStyle->setToolTip(i18n("Set this option to choose the default window decoration and looks."));
    const auto styles         = QStyleFactory::keys();

    for (const QString& style : styles)
    {
        QString sitem = style;
        sitem[0]      = sitem[0].toUpper();

        if (sitem != QLatin1String("Macintosh"))        // See bug #475572
        {
            d->applicationStyle->addItem(sitem, sitem.toLower());
        }
    }

#ifndef HAVE_APPSTYLE_SUPPORT

    // See Bug #365262

    appStyleHbox->setVisible(false);

#endif

    DHBox* const iconThemeHbox = new DHBox(appearancePanel);
    d->applicationIconLabel    = new QLabel(i18n("Icon theme (changes after restart):"), iconThemeHbox);
    d->applicationIcon         = new QComboBox(iconThemeHbox);
    d->applicationIcon->setToolTip(i18n("Set this option to choose the default icon theme."));

    QMap<QString, QString> iconThemes;
    QMap<QString, QString> themeWhiteList;
    themeWhiteList.insert(QLatin1String("adwaita"),         i18nc("icon theme", "Adwaita"));
    themeWhiteList.insert(QLatin1String("breeze"),          i18nc("icon theme", "Breeze"));
    themeWhiteList.insert(QLatin1String("breeze-dark"),     i18nc("icon theme", "Breeze Dark"));
    themeWhiteList.insert(QLatin1String("faenza"),          i18nc("icon theme", "Faenza"));
    themeWhiteList.insert(QLatin1String("faenza-ambiance"), i18nc("icon theme", "Ambiance"));
    themeWhiteList.insert(QLatin1String("humanity"),        i18nc("icon theme", "Humanity"));
    themeWhiteList.insert(QLatin1String("oxygen"),          i18nc("icon theme", "Oxygen"));
    const auto pathes = QIcon::themeSearchPaths();

    for (const QString& path : pathes)
    {
        QDirIterator it(path, QDir::Dirs       |
                              QDir::NoSymLinks |
                              QDir::NoDotAndDotDot);

        while (it.hasNext())
        {
            if (QFile::exists(it.next() + QLatin1String("/index.theme")))
            {
                QString iconKey = it.fileInfo().fileName().toLower();

                if (themeWhiteList.contains(iconKey))
                {
                    iconThemes[themeWhiteList.value(iconKey)] = it.fileInfo().fileName();
                }
            }
        }
    }

    QMap<QString, QString>::const_iterator it = iconThemes.constBegin();
    d->applicationIcon->addItem(i18n("Use Icon Theme From System"), QString());

    for ( ; it != iconThemes.constEnd() ; ++it)
    {
        d->applicationIcon->addItem(it.key(), it.value());
    }

    d->applicationFont = new DFontSelect(i18n("Application font:"), appearancePanel);
    d->applicationFont->setToolTip(i18n("Select here the font used to display text in whole application."));

    // --------------------------------------------------------

    layout2->setContentsMargins(spacing, spacing, spacing, spacing);
    layout2->setSpacing(spacing);
    layout2->addWidget(d->showSplashCheck);
    layout2->addWidget(d->useNativeFileDialogCheck);
    layout2->addWidget(d->drawFramesToGroupedCheck);
    layout2->addWidget(tabStyleHbox);
    layout2->addWidget(appStyleHbox);
    layout2->addWidget(iconThemeHbox);
    layout2->addWidget(d->applicationFont);
    layout2->addStretch();

    d->tab->insertTab(Appearance, appearancePanel, i18nc("@title:tab", "Appearance"));

    // -- Grouping Options -------------------------------------

    QWidget* const groupingPanel = new QWidget(d->tab);
    QGridLayout* const grid      = new QGridLayout(groupingPanel);

    QLabel* const description    = new QLabel(i18n("Perform the following operations on all group members:"), groupingPanel);
    description->setToolTip(i18n("When images are grouped the following operations<br/>"
                                 "are performed only on the displayed item (No)<br/>"
                                 "or on all the hidden items in the group as well (Yes).<br/>"
                                 "If Ask is selected, there will be a prompt every<br/>"
                                 "time this operation is executed."));

    QLabel* const noLabel        = new QLabel(i18nc("@label: grouped image ops", "No"),  groupingPanel);
    QLabel* const yesLabel       = new QLabel(i18nc("@label: grouped image ops", "Yes"), groupingPanel);
    QLabel* const askLabel       = new QLabel(i18nc("@label: grouped image ops", "Ask"), groupingPanel);

    QHash<int, QLabel*> labels;

    for (int i = 0 ; i != UnspecifiedOps ; ++i)
    {
        labels.insert(i, new QLabel(ApplicationSettings::operationTypeTitle(
                                    (OperationType)i), groupingPanel));
        QString explanation = ApplicationSettings::operationTypeExplanation(
                              (OperationType)i);

        if (!explanation.isEmpty())
        {
            labels.value(i)->setToolTip(explanation);
        }

        d->groupingButtons.insert(i, new QButtonGroup(groupingPanel));
        d->groupingButtons.value(i)->addButton(new QRadioButton(groupingPanel), 0);
        d->groupingButtons.value(i)->addButton(new QRadioButton(groupingPanel), 1);
        d->groupingButtons.value(i)->addButton(new QRadioButton(groupingPanel), 2);
    }

    grid->addWidget(description, 0, 0, 1, 4);
    grid->addWidget(noLabel,     1, 1, 1, 1);
    grid->addWidget(yesLabel,    1, 2, 1, 1);
    grid->addWidget(askLabel,    1, 3, 1, 1);

    for (int i = 0 ; i != UnspecifiedOps ; ++i)
    {
        grid->addWidget(labels.value(i),                        i+2, 0, 1, 1);
        grid->addWidget(d->groupingButtons.value(i)->button(0), i+2, 1, 1, 1);
        grid->addWidget(d->groupingButtons.value(i)->button(1), i+2, 2, 1, 1);
        grid->addWidget(d->groupingButtons.value(i)->button(2), i+2, 3, 1, 1);
    }

    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);
    grid->setColumnStretch(0, 10);
    grid->setColumnMinimumWidth(1, 30);
    grid->setColumnMinimumWidth(2, 30);
    grid->setColumnMinimumWidth(3, 30);
    grid->setRowStretch(20, 10);

    d->tab->insertTab(Grouping, groupingPanel, i18nc("@title:tab", "Grouping"));

    // -- Spell Check and localize Options --------------------------------------

#ifdef HAVE_SONNET

    d->spellCheckWidget = new SpellCheckConfig(d->tab);

    d->tab->insertTab(SpellCheck, d->spellCheckWidget, i18nc("@title:tab", "Spellcheck"));

#endif

    d->localizeWidget = new LocalizeConfig(d->tab);

    d->tab->insertTab(Localize, d->localizeWidget, i18nc("@title:tab", "Localize"));

    // -- System Options --------------------------------------------------------

    d->systemSettingsWidget = new SystemSettingsWidget(d->tab);

    d->tab->insertTab(System, d->systemSettingsWidget, i18nc("@title:tab", "System"));

    // --------------------------------------------------------

    readSettings();
    adjustSize();
}

SetupMisc::~SetupMisc()
{
    delete d;
}

void SetupMisc::setActiveTab(MiscTab tab)
{
    d->tab->setCurrentIndex(tab);
}

SetupMisc::MiscTab SetupMisc::activeTab() const
{
    return (MiscTab)d->tab->currentIndex();
}

bool SetupMisc::checkSettings()
{
    // If check for new version use weekly pre-releases, warn end-user.

    if ((d->updateType->currentIndex()                    == 1) &&
        (ApplicationSettings::instance()->getUpdateType() == 0))
    {
        d->tab->setCurrentIndex(0);

        int result = QMessageBox::warning(this, qApp->applicationName(),
                                          i18n("Check for new version option will verify the pre-releases.\n"
                                               "\"Pre-releases\" are proposed weekly to tests quickly new features.\n"
                                               "It's not recommended to use pre-release in production as bugs can remain,\n"
                                               "unless you know what you are doing.\n"
                                               "Do you want to continue?"),
                                                QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes)
        {
            return true;
        }

        return false;
    }

    return true;
}

void SetupMisc::applySettings()
{
    d->systemSettingsWidget->saveSettings();

    ApplicationSettings* const settings      = ApplicationSettings::instance();
    MetaEngineSettingsContainer metaSettings = MetaEngineSettings::instance()->settings();

    settings->setShowSplashScreen(d->showSplashCheck->isChecked());
    settings->setShowTrashDeleteDialog(d->showTrashDeleteDialogCheck->isChecked());
    settings->setShowPermanentDeleteDialog(d->showPermanentDeleteDialogCheck->isChecked());
    settings->setMinimumSimilarityBound(d->minimumSimilarityBound->value());
    settings->setApplySidebarChangesDirectly(d->sidebarApplyDirectlyCheck->isChecked());
    settings->setScanAtStart(d->scanAtStart->isChecked());
    settings->setDetectFacesInNewImages(d->detectFaces->isChecked());
    settings->setCleanAtStart(d->cleanAtStart->isChecked());
    settings->setUseNativeFileDialog(d->useNativeFileDialogCheck->isChecked());
    settings->setDrawFramesToGrouped(d->drawFramesToGroupedCheck->isChecked());
    settings->setSelectFirstAlbumItem(d->selectFirstAlbumItemCheck->isChecked());
    settings->setExpandNewCurrentItem(d->expandNewCurrentItemCheck->isChecked());
    settings->setScrollItemToCenter(d->scrollItemToCenterCheck->isChecked());
    settings->setShowOnlyPersonTagsInPeopleSidebar(d->showOnlyPersonTagsInPeopleSidebarCheck->isChecked());
    settings->setSidebarTitleStyle(d->sidebarType->currentIndex() == 0 ? DMultiTabBar::ActiveIconText : DMultiTabBar::AllIconsText);
    settings->setUpdateType(d->updateType->currentIndex());
    settings->setUpdateWithDebug(d->updateWithDebug->isChecked());
    settings->setStringComparisonType((ApplicationSettings::StringComparisonType)
                                      d->stringComparisonType->itemData(d->stringComparisonType->currentIndex()).toInt());

    metaSettings.useFastScan = d->scanAtStart->isChecked() ? d->useFastScan->isChecked() : false;
    metaSettings.albumDateFrom = (MetaEngineSettingsContainer::AlbumDateSource)
                                 d->albumDateSource->itemData(d->albumDateSource->currentIndex()).toInt();
    MetaEngineSettings::instance()->setSettings(metaSettings);

    for (int i = 0 ; i != UnspecifiedOps ; ++i)
    {
        settings->setGroupingOperateOnAll((OperationType)i,
                                          (ApplicationSettings::ApplyToEntireGroup)d->groupingButtons.value(i)->checkedId());
    }

#ifdef HAVE_APPSTYLE_SUPPORT

    if (settings->getApplicationStyle().compare(d->applicationStyle->currentText(), Qt::CaseInsensitive) != 0)
    {
        settings->setApplicationStyle(d->applicationStyle->currentText());
        ThemeManager::instance()->updateThemeMenu();
    }

#endif

    settings->setIconTheme(d->applicationIcon->currentData().toString());
    settings->setApplicationFont(d->applicationFont->font());
    settings->saveSettings();

#ifdef HAVE_SONNET

    d->spellCheckWidget->applySettings();

#endif

    d->localizeWidget->applySettings();
}

void SetupMisc::readSettings()
{
    d->systemSettingsWidget->readSettings();

    ApplicationSettings* const settings = ApplicationSettings::instance();

    d->showSplashCheck->setChecked(settings->getShowSplashScreen());
    d->showTrashDeleteDialogCheck->setChecked(settings->getShowTrashDeleteDialog());
    d->showPermanentDeleteDialogCheck->setChecked(settings->getShowPermanentDeleteDialog());
    d->minimumSimilarityBound->setValue(settings->getMinimumSimilarityBound());
    d->sidebarApplyDirectlyCheck->setChecked(settings->getApplySidebarChangesDirectly());
    d->sidebarApplyDirectlyCheck->setChecked(settings->getApplySidebarChangesDirectly());
    d->scanAtStart->setChecked(settings->getScanAtStart());
    d->detectFaces->setChecked(settings->getDetectFacesInNewImages());
    d->cleanAtStart->setChecked(settings->getCleanAtStart());
    d->useNativeFileDialogCheck->setChecked(settings->getUseNativeFileDialog());
    d->drawFramesToGroupedCheck->setChecked(settings->getDrawFramesToGrouped());
    d->selectFirstAlbumItemCheck->setChecked(settings->getSelectFirstAlbumItem());
    d->expandNewCurrentItemCheck->setChecked(settings->getExpandNewCurrentItem());
    d->scrollItemToCenterCheck->setChecked(settings->getScrollItemToCenter());
    d->showOnlyPersonTagsInPeopleSidebarCheck->setChecked(settings->showOnlyPersonTagsInPeopleSidebar());
    d->sidebarType->setCurrentIndex(settings->getSidebarTitleStyle() == DMultiTabBar::ActiveIconText ? 0 : 1);
    d->updateType->setCurrentIndex(settings->getUpdateType());
    d->updateWithDebug->setChecked(settings->getUpdateWithDebug());
    d->stringComparisonType->setCurrentIndex(settings->getStringComparisonType());

    MetaEngineSettingsContainer metaSettings = MetaEngineSettings::instance()->settings();
    d->albumDateSource->setCurrentIndex((int)metaSettings.albumDateFrom);
    d->useFastScan->setChecked(metaSettings.useFastScan);

    for (int i = 0 ; i != UnspecifiedOps ; ++i)
    {
        d->groupingButtons.value(i)->button((int)settings->getGroupingOperateOnAll((OperationType)i))->setChecked(true);
    }

#ifdef HAVE_APPSTYLE_SUPPORT

    d->applicationStyle->setCurrentIndex(d->applicationStyle->findData(settings->getApplicationStyle().toLower()));

#endif

    d->applicationIcon->setCurrentIndex(d->applicationIcon->findData(settings->getIconTheme()));
    d->applicationFont->setFont(settings->getApplicationFont());

    // NOTE: Spellcheck and Localize read settings is done in widget constructor.
}

} // namespace Digikam

#include "moc_setupmisc.cpp"
