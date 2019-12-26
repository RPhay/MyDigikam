/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Dialog to choose options for face scanning
 *
 * Copyright (C) 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// NOTE: Uncomment this line to enable detect and recognize option
// Currently this option is hiden, since it's not handled properly and provides confusing functionality => Fix it later
//#define ENABLE_DETECT_AND_RECOGNIZE

#include "facescandialog_p.h"

namespace Digikam
{

FaceScanDialog::FaceScanDialog(QWidget* const parent)
    : QDialog(parent),
      StateSavingObject(this),
      d(new Private)
{
    setWindowFlags((windowFlags() & ~Qt::Dialog) |
                   Qt::Window                    |
                   Qt::WindowCloseButtonHint     |
                   Qt::WindowMinMaxButtonsHint);

    setWindowTitle(i18nc("@title:window", "Scanning faces"));
    setModal(true);

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Ok)->setDefault(true);
    d->buttons->button(QDialogButtonBox::Ok)->setText(i18nc("@action:button", "Scan"));

    setupUi();
    setupConnections();

    setObjectName(d->configName);
    loadState();
}

FaceScanDialog::~FaceScanDialog()
{
    delete d;
}

void FaceScanDialog::doLoadState()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << getConfigGroup().name();
    KConfigGroup group = getConfigGroup();
    QString mainTask   = group.readEntry(entryName(d->configMainTask),
                                         d->configValueDetect);

    if (mainTask == d->configValueRecognizedMarkedFaces)
    {
        d->reRecognizeButton->setChecked(true);
    }
    else if (mainTask == d->configValueDetectAndRecognize)
    {
#ifdef ENABLE_DETECT_AND_RECOGNIZE
        d->detectAndRecognizeButton->setChecked(true);
#endif
        d->detectButton->setChecked(true);
    }
    else
    {
        d->detectButton->setChecked(true);
    }

    FaceScanSettings::AlreadyScannedHandling handling;
    QString skipHandling = group.readEntry(entryName(d->configAlreadyScannedHandling),
                                           QString::fromLatin1("Skip"));

    if (skipHandling == QLatin1String("Rescan"))
    {
        handling = FaceScanSettings::Rescan;
    }
    else if (skipHandling == QLatin1String("Merge"))
    {
        handling = FaceScanSettings::Merge;
    }
    else // Skip
    {
        handling = FaceScanSettings::Skip;
    }

    d->alreadyScannedBox->setCurrentIndex(d->alreadyScannedBox->findData(handling));

    d->accuracyInput->setValue(ApplicationSettings::instance()->getFaceDetectionAccuracy() * 100);

    d->albumSelectors->loadState();

    d->useFullCpuButton->setChecked(group.readEntry(entryName(d->configUseFullCpu), false));

    // do not load retrainAllButton state from config, dangerous
}

void FaceScanDialog::doSaveState()
{
    qCDebug(DIGIKAM_GENERAL_LOG) << getConfigGroup().name();

    KConfigGroup group = getConfigGroup();

    QString mainTask;

    if (d->detectButton->isChecked())
    {
        mainTask = d->configValueDetect;
    }
#ifdef ENABLE_DETECT_AND_RECOGNIZE
    else if (d->detectAndRecognizeButton->isChecked())
    {
        mainTask = d->configValueDetectAndRecognize;
    }
#endif
    else // d->reRecognizeButton
    {
        mainTask = d->configValueRecognizedMarkedFaces;
    }

    group.writeEntry(entryName(d->configMainTask), mainTask);

    QString handling;

    switch ((FaceScanSettings::AlreadyScannedHandling)(d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt()))
    {
        case FaceScanSettings::Skip:
            handling = QLatin1String("Skip");
            break;

        case FaceScanSettings::Rescan:
            handling = QLatin1String("Rescan");
            break;

        case FaceScanSettings::Merge:
            handling = QLatin1String("Merge");
            break;
    }

    group.writeEntry(entryName(d->configAlreadyScannedHandling), handling);

    ApplicationSettings::instance()->setFaceDetectionAccuracy(double(d->accuracyInput->value()) / 100);
    d->albumSelectors->saveState();

    group.writeEntry(entryName(d->configUseFullCpu), d->useFullCpuButton->isChecked());
}

void FaceScanDialog::setupUi()
{
    QWidget* const mainWidget     = new QWidget;
    QGridLayout* const mainLayout = new QGridLayout;
    d->tabWidget                  = new QTabWidget;

    // ---- Introductory labels ----

    QLabel* const personIcon      = new QLabel;
    personIcon->setPixmap(QIcon::fromTheme(QLatin1String("edit-image-face-show")).pixmap(48));

    QLabel* const introduction    = new QLabel;
    introduction->setWordWrap(true);
    introduction->setText(i18nc("@info",
                                "digiKam can search for faces in your photos. "
                                "When you have identified your friends on a number of photos, "
                                "it can also recognize the people shown on your photos."));

    // ---- Main option box --------

    d->optionGroupBox               = new QGroupBox;
    QGridLayout* const optionLayout = new QGridLayout;

    d->alreadyScannedBox            = new SqueezedComboBox;
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Skip images already scanned"),          FaceScanSettings::Skip);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Scan again and merge results"),         FaceScanSettings::Merge);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Clear unconfirmed results and rescan"), FaceScanSettings::Rescan);
    d->alreadyScannedBox->setCurrentIndex(FaceScanSettings::Skip);

    d->detectButton                 = new QRadioButton(i18nc("@option:radio", "Detect faces"));
    d->detectButton->setToolTip(i18nc("@info", "Find all faces in your photos"));

#ifdef ENABLE_DETECT_AND_RECOGNIZE
    d->detectAndRecognizeButton     = new QRadioButton(i18nc("@option:radio", "Detect and recognize faces"));
    d->detectAndRecognizeButton->setToolTip(i18nc("@info", "Find all faces in your photos and try to recognize which person is depicted"));
#endif

    d->reRecognizeButton            = new QRadioButton(i18nc("@option:radio", "Recognize faces"));
    d->reRecognizeButton->setToolTip(i18nc("@info", "Try again to recognize the people depicted on marked but yet unconfirmed faces."));

    optionLayout->addWidget(d->alreadyScannedBox,          0, 0, 1, 2);
    optionLayout->addWidget(d->detectButton,               1, 0, 1, 2);

#ifdef ENABLE_DETECT_AND_RECOGNIZE
    optionLayout->addWidget(d->detectAndRecognizeButton,   2, 0, 1, 2);
#endif

    optionLayout->addWidget(d->reRecognizeButton,          2, 0, 1, 2);

#ifdef ENABLE_DETECT_AND_RECOGNIZE
    QStyleOptionButton buttonOption;
    buttonOption.initFrom(d->detectAndRecognizeButton);
    int indent = style()->subElementRect(QStyle::SE_RadioButtonIndicator, &buttonOption, d->detectAndRecognizeButton).width();
    optionLayout->setColumnMinimumWidth(0, indent);
#endif

    d->optionGroupBox->setLayout(optionLayout);

    // ------------------------

    mainLayout->addWidget(personIcon,        0, 0);
    mainLayout->addWidget(introduction,      0, 1);
    mainLayout->addWidget(d->optionGroupBox, 1, 0, 1, -1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setRowStretch(2, 1);
    mainWidget->setLayout(mainLayout);

    // ---- Album tab ---------

    d->albumSelectors = new AlbumSelectors(i18nc("@label", "Search in:"), d->configName, d->tabWidget);
    d->tabWidget->addTab(d->albumSelectors, i18nc("@title:tab", "Albums"));

    // ---- Parameters tab ----

    QWidget* const parametersTab        = new QWidget(d->tabWidget);
    QGridLayout* const parametersLayout = new QGridLayout(parametersTab);

    QLabel* const detectionLabel        = new QLabel(i18nc("@label", "Face detection and Recognition"), parametersTab);

    QLabel* const accuracyLabel         = new QLabel(i18nc("@label Two extremities of a scale", "Sensitivity     -     Specificity"), parametersTab);
    accuracyLabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    d->accuracyInput                    = new DIntNumInput(parametersTab);
    d->accuracyInput->setDefaultValue(70);
    d->accuracyInput->setRange(0, 100, 10);
    d->accuracyInput->setToolTip(i18nc("@info:tooltip",
                                       "Adjust sensitivity versus specificity: The higher the value, the more accurately faces will "
                                       "be recognized, but less faces will be recognized "
                                       "(only faces that are very similar to pretagged faces are recognized)"));

    parametersLayout->addWidget(detectionLabel,   0, 0, 1, 1);
    parametersLayout->addWidget(d->accuracyInput, 1, 0, 1, 1);
    parametersLayout->addWidget(accuracyLabel,    2, 0, 1, 1);
    parametersLayout->setColumnStretch(0, 10);
    parametersLayout->setRowStretch(3, 10);

    d->tabWidget->addTab(parametersTab, i18nc("@title:tab", "Parameters"));

    // ---- Advanced tab ------

    QWidget* const advancedTab        = new QWidget(d->tabWidget);
    QGridLayout* const advancedLayout = new QGridLayout(advancedTab);

    d->useFullCpuButton = new QCheckBox(advancedTab);
    d->useFullCpuButton->setText(i18nc("@option:check", "Work on all processor cores"));
    d->useFullCpuButton->setToolTip(i18nc("@info:tooltip",
                                          "Face detection and recognition are time-consuming tasks. "
                                          "You can choose if you wish to employ all processor cores "
                                          "on your system, or work in the background only on one core."));

    d->retrainAllButton = new QCheckBox(advancedTab);
    d->retrainAllButton->setText(i18nc("@option:check", "Clear and rebuild all training data"));
    d->retrainAllButton->setToolTip(i18nc("@info:tooltip",
                                          "This will clear all training data for recognition "
                                          "and rebuild it from all available faces."));

    advancedLayout->addWidget(d->useFullCpuButton,             0, 0);
    advancedLayout->addWidget(new DLineWidget(Qt::Horizontal), 1, 0);
    advancedLayout->addWidget(d->retrainAllButton,             2, 0);
    advancedLayout->setRowStretch(3, 10);

    d->tabWidget->addTab(advancedTab, i18nc("@title:tab", "Advanced"));

    // ------------------------

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(mainWidget);
    vbx->addWidget(d->tabWidget);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    adjustSize();
}

void FaceScanDialog::setupConnections()
{
/*
     connect(d->detectButton, SIGNAL(toggled(bool)),
            d->alreadyScannedBox, SLOT(setEnabled(bool)));
*/
#ifdef ENABLE_DETECT_AND_RECOGNIZE
    connect(d->detectAndRecognizeButton, SIGNAL(toggled(bool)),
            d->alreadyScannedBox, SLOT(setEnabled(bool)));
#endif
    connect(d->detectButton, SIGNAL(toggled(bool)),
            this, SLOT(slotPrepareForDetect(bool)));

    connect(d->reRecognizeButton, SIGNAL(toggled(bool)),
            this, SLOT(slotPrepareForRecognize(bool)));

    connect(d->retrainAllButton, SIGNAL(toggled(bool)),
            this, SLOT(retrainAllButtonToggled(bool)));

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotOk()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));
}

void FaceScanDialog::slotPrepareForDetect(bool status)
{
    d->alreadyScannedBox->setEnabled(status);

    // Set default for Tag tab as all unchecked
    d->albumSelectors->resetTAlbumSelection();
}

void FaceScanDialog::slotPrepareForRecognize(bool /*status*/)
{
    /**
     * TODO:
     * reRecognizeButton always disables d->alreadyScannedBox, while it should be
     * Find out why and fix it.
     */
    d->alreadyScannedBox->setEnabled(false);

    // First we set all tags unchecked
    d->albumSelectors->resetTAlbumSelection();

    // Set default for Tag tab so that People and its children are checked
    AlbumList tagAlbums = AlbumManager::instance()->allTAlbums();
    QString people      = i18nc("People on your photos", "People");

    for (int i = 0 ; i < tagAlbums.size() ; ++i)
    {
        Album* const album = tagAlbums[i];

        if (album->title() == people ||
           ((album->parent() != nullptr) && (album->parent()->title() == people)))
        {
            d->albumSelectors->setTagSelected(album, false);
        }
    }
}

void FaceScanDialog::slotOk()
{
    accept();
    saveState();
}

void FaceScanDialog::retrainAllButtonToggled(bool on)
{
    d->optionGroupBox->setEnabled(!on);
    d->albumSelectors->setEnabled(!on);
}

bool FaceScanDialog::settingsConflicted() const
{
    return d->settingsConflicted;
}

FaceScanSettings FaceScanDialog::settings() const
{
    FaceScanSettings settings;

    if (d->retrainAllButton->isChecked())
    {
        settings.task = FaceScanSettings::RetrainAll;
    }
    else if (d->detectButton->isChecked())
    {
        settings.task = FaceScanSettings::Detect;
    }
    else
    {
#ifdef ENABLE_DETECT_AND_RECOGNIZE
        if (d->detectAndRecognizeButton->isChecked())
        {
            settings.task = FaceScanSettings::DetectAndRecognize;
        }
        else // recognize only
#endif
        {
            settings.task = FaceScanSettings::RecognizeMarkedFaces;

            // preset settingsConflicted as True, since by default there are no tags to recognize
            d->settingsConflicted = true;
        }
    }

    settings.alreadyScannedHandling = (FaceScanSettings::AlreadyScannedHandling)
                                      d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt();

    settings.accuracy               = double(d->accuracyInput->value()) / 100;

    // TODO: why does the original code append but not assign here???
    // settings.albums << d->albumSelectors->selectedAlbumsAndTags();
    settings.albums                 = d->albumSelectors->selectedAlbumsAndTags();

    if (d->settingsConflicted)
    {
        int numberOfIdentities      = FaceDbAccess().db()->getNumberOfIdentities();
        d->settingsConflicted       = (numberOfIdentities == 0);
    }

    settings.useFullCpu             = d->useFullCpuButton->isChecked();

    return settings;
}

} // namespace Digikam
