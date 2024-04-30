/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-04-02
 * Description : setup Misc tab.
 *
 * SPDX-FileCopyrightText: 2005-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText:      2008 by Arnd Baecker <arnd dot baecker at web dot de>
 * SPDX-FileCopyrightText:      2014 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "showfotosetupmisc.h"

// Qt includes

#include <QCheckBox>
#include <QColor>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QStyleFactory>
#include <QApplication>
#include <QDirIterator>
#include <QStyle>
#include <QComboBox>
#include <QFile>
#include <QTabWidget>
#include <QMessageBox>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "dlayoutbox.h"
#include "dfontselect.h"
#include "showfotosettings.h"
#include "systemsettingswidget.h"
#include "onlineversionchecker.h"
#include "showfotosetup.h"
#include "localizeconfig.h"

#ifdef HAVE_SONNET
#   include "spellcheckconfig.h"
#endif

using namespace Digikam;

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoSetupMisc::Private
{
public:

    Private() = default;

    QTabWidget*           tab                   = nullptr;

    QLabel*               updateTypeLabel       = nullptr;
    QLabel*               sidebarTypeLabel      = nullptr;
    QLabel*               applicationStyleLabel = nullptr;
    QLabel*               applicationIconLabel  = nullptr;

    QCheckBox*            showSplash            = nullptr;
    QCheckBox*            nativeFileDialog      = nullptr;
    QCheckBox*            itemCenter            = nullptr;
    QCheckBox*            showMimeOverImage     = nullptr;
    QCheckBox*            showCoordinates       = nullptr;
    QCheckBox*            sortReverse           = nullptr;

    QComboBox*            updateType            = nullptr;
    QCheckBox*            updateWithDebug       = nullptr;

    QComboBox*            sidebarType           = nullptr;
    QComboBox*            sortOrderComboBox     = nullptr;
    QComboBox*            applicationStyle      = nullptr;
    QComboBox*            applicationIcon       = nullptr;
    DFontSelect*          applicationFont       = nullptr;

    SystemSettingsWidget* systemSettingsWidget  = nullptr;

#ifdef HAVE_SONNET

    SpellCheckConfig*     spellCheckWidget      = nullptr;

#endif

    LocalizeConfig*       localizeWidget        = nullptr;
    ShowfotoSettings*     settings              = ShowfotoSettings::instance();
};

// --------------------------------------------------------

ShowfotoSetupMisc::ShowfotoSetupMisc(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->tab            = new QTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);

    const int spacing = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    // -- Application Behavior Options --------------------------------------------------------

    QWidget* const behaviourPanel = new QWidget(d->tab);
    QVBoxLayout* const layout     = new QVBoxLayout(behaviourPanel);

    // -- Sort Order Options ----------------------------------

    QGroupBox* const sortOptionsGroup = new QGroupBox(i18n("Thumbbar, Folder-View, and Stack-View Items Sort Order"), behaviourPanel);
    QVBoxLayout* const gLayout4       = new QVBoxLayout();

    DHBox* const sortBox = new DHBox(sortOptionsGroup);
    new QLabel(i18n("Sort images by:"), sortBox);
    d->sortOrderComboBox = new QComboBox(sortBox);
    d->sortOrderComboBox->insertItem(SortByDate,     i18nc("sort images by date", "Date"));
    d->sortOrderComboBox->insertItem(SortByName,     i18nc("sort images by name", "Name"));
    d->sortOrderComboBox->insertItem(SortByFileSize, i18nc("sort images by size", "File Size"));
    d->sortOrderComboBox->setWhatsThis(i18n("Here, select whether newly-loaded "
                                            "images are sorted by their date, name, or size on disk."));

    d->sortReverse = new QCheckBox(i18n("Reverse ordering"), sortOptionsGroup);
    d->sortReverse->setWhatsThis(i18n("If this option is enabled, newly-loaded "
                                      "images will be sorted in descending order."));

    gLayout4->addWidget(sortBox);
    gLayout4->addWidget(d->sortReverse);
    sortOptionsGroup->setLayout(gLayout4);

    // Thumbnails Options -------------------------------------

    QGroupBox* const thOptionsGroup = new QGroupBox(i18n("Thumbbar Items"), behaviourPanel);
    QVBoxLayout* const gLayout3     = new QVBoxLayout();

    d->showMimeOverImage = new QCheckBox(i18n("&Show image Format"),          thOptionsGroup);
    d->showMimeOverImage->setWhatsThis(i18n("Set this option to show image format over image thumbnail."));
    d->showCoordinates   = new QCheckBox(i18n("&Show Geolocation Indicator"), thOptionsGroup);
    d->showCoordinates->setWhatsThis(i18n("Set this option to indicate if image has geolocation information."));
    d->itemCenter        = new QCheckBox(i18n("Scroll current item to center of thumbbar"), thOptionsGroup);

    gLayout3->addWidget(d->showMimeOverImage);
    gLayout3->addWidget(d->showCoordinates);
    gLayout3->addWidget(d->itemCenter);
    thOptionsGroup->setLayout(gLayout3);

    // Update Options ----------------------------------------

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
            if (parent)
            {
                parent->close();
            }

            ShowfotoSetup::onlineVersionCheck();
        }
    );

    gLayout5->addWidget(updateHbox);
    gLayout5->addWidget(d->updateWithDebug);
    gLayout5->addWidget(updateHbox2);
    upOptionsGroup->setLayout(gLayout5);

    // ---------------------------------------------------------

    layout->setContentsMargins(spacing, spacing, spacing, spacing);
    layout->setSpacing(spacing);
    layout->addWidget(sortOptionsGroup);
    layout->addWidget(thOptionsGroup);
    layout->addWidget(upOptionsGroup);
    layout->addStretch();

    d->tab->insertTab(Behaviour, behaviourPanel, i18nc("@title:tab", "Behaviour"));

    // -- Application Appearance Options --------------------------------------------------------

    QWidget* const appearancePanel = new QWidget(d->tab);
    QVBoxLayout* const layout2     = new QVBoxLayout(appearancePanel);

    d->showSplash       = new QCheckBox(i18n("&Show splash screen at startup"),            appearancePanel);
    d->nativeFileDialog = new QCheckBox(i18n("Use native file dialogs from the system"),   appearancePanel);

    DHBox* const tabStyleHbox = new DHBox(appearancePanel);
    d->sidebarTypeLabel       = new QLabel(i18n("Sidebar tab title:"), tabStyleHbox);
    d->sidebarType            = new QComboBox(tabStyleHbox);
    d->sidebarType->addItem(i18n("Only For Active Tab"), 0);
    d->sidebarType->addItem(i18n("For All Tabs"),        1);
    d->sidebarType->setToolTip(i18n("Set this option to configure how sidebars tab title are visible."));

    DHBox* const appStyleHbox = new DHBox(appearancePanel);
    d->applicationStyleLabel  = new QLabel(i18n("Widget style:"), appStyleHbox);
    d->applicationStyle       = new QComboBox(appStyleHbox);
    d->applicationStyle->setToolTip(i18n("Set this option to choose the default window decoration and looks."));

    Q_FOREACH (const QString& style, QStyleFactory::keys())
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

    Q_FOREACH (const QString& path, QIcon::themeSearchPaths())
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
    layout2->addWidget(d->showSplash);
    layout2->addWidget(d->nativeFileDialog);
    layout2->addWidget(tabStyleHbox);
    layout2->addWidget(appStyleHbox);
    layout2->addWidget(iconThemeHbox);
    layout2->addWidget(d->applicationFont);
    layout2->addStretch();

    d->tab->insertTab(Appearance, appearancePanel, i18nc("@title:tab", "Appearance"));

    // -- Spell Check and Localize Options --------------------------------------

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
}

ShowfotoSetupMisc::~ShowfotoSetupMisc()
{
    delete d;
}

void ShowfotoSetupMisc::setActiveTab(MiscTab tab)
{
    d->tab->setCurrentIndex(tab);
}

ShowfotoSetupMisc::MiscTab ShowfotoSetupMisc::activeTab() const
{
    return (MiscTab)d->tab->currentIndex();
}

bool ShowfotoSetupMisc::checkSettings()
{
    // If check for new version use weekly pre-releases, warn end-user.

    if (
        (d->updateType->currentIndex()                 == 1) &&
        (ShowfotoSettings::instance()->getUpdateType() == 0)
       )
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

void ShowfotoSetupMisc::readSettings()
{
    d->systemSettingsWidget->readSettings();

    d->showSplash->setChecked(d->settings->getShowSplash());
    d->nativeFileDialog->setChecked(d->settings->getNativeFileDialog());
    d->itemCenter->setChecked(d->settings->getItemCenter());
    d->showMimeOverImage->setChecked(d->settings->getShowFormatOverThumbnail());
    d->showCoordinates->setChecked(d->settings->getShowCoordinates());
    d->sidebarType->setCurrentIndex(d->settings->getRightSideBarStyle());
    d->updateType->setCurrentIndex(d->settings->getUpdateType());
    d->updateWithDebug->setChecked(d->settings->getUpdateWithDebug());
    d->sortOrderComboBox->setCurrentIndex(d->settings->getSortRole());
    d->sortReverse->setChecked(d->settings->getReverseSort());

#ifdef HAVE_APPSTYLE_SUPPORT

    d->applicationStyle->setCurrentIndex(d->applicationStyle->findData(d->settings->getApplicationStyle().toLower()));

#endif

    d->applicationIcon->setCurrentIndex(d->applicationIcon->findData(d->settings->getIconTheme()));
    d->applicationFont->setFont(d->settings->getApplicationFont());

    // NOTE: Spellcheck and Localize read settings is done in widget constructor.
}

void ShowfotoSetupMisc::applySettings()
{
    d->systemSettingsWidget->saveSettings();

    d->settings->setShowSplash(d->showSplash->isChecked());
    d->settings->setNativeFileDialog(d->nativeFileDialog->isChecked());
    d->settings->setItemCenter(d->itemCenter->isChecked());
    d->settings->setShowFormatOverThumbnail(d->showMimeOverImage->isChecked());
    d->settings->setShowCoordinates(d->showCoordinates->isChecked());
    d->settings->setRightSideBarStyle(d->sidebarType->currentIndex());
    d->settings->setUpdateType(d->updateType->currentIndex());
    d->settings->setUpdateWithDebug(d->updateWithDebug->isChecked());
    d->settings->setSortRole(d->sortOrderComboBox->currentIndex());
    d->settings->setReverseSort(d->sortReverse->isChecked());

#ifdef HAVE_APPSTYLE_SUPPORT

    d->settings->setApplicationStyle(d->applicationStyle->currentText());

#endif

    d->settings->setIconTheme(d->applicationIcon->currentData().toString());
    d->settings->setApplicationFont(d->applicationFont->font());
    d->settings->syncConfig();

#ifdef HAVE_SONNET

    d->spellCheckWidget->applySettings();

#endif

    d->localizeWidget->applySettings();
}

} // namespace ShowFoto

#include "moc_showfotosetupmisc.cpp"
