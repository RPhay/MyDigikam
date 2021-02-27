/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Widget to choose options for face scanning
 *
 * Copyright (C) 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "facescanwidget_p.h"

namespace Digikam
{

FaceScanWidget::FaceScanWidget(QWidget* const parent)
    : QTabWidget       (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(d->configName);
    setupUi();
    setupConnections();
}

FaceScanWidget::~FaceScanWidget()
{
    delete d;
}

void FaceScanWidget::doLoadState()
{
    KConfigGroup group = getConfigGroup();
    QString mainTask   = group.readEntry(entryName(d->configMainTask),
                                         d->configValueDetect);

    if      (mainTask == d->configValueRecognizedMarkedFaces)
    {
        d->reRecognizeButton->setChecked(true);
    }
    else if (mainTask == d->configValueDetectAndRecognize)
    {

#ifdef ENABLE_DETECT_AND_RECOGNIZE

        d->detectAndRecognizeButton->setChecked(true);

#else

        d->detectButton->setChecked(true);

#endif

    }
    else
    {
        d->detectButton->setChecked(true);
    }

    FaceScanSettings::AlreadyScannedHandling handling;
    QString skipHandling = group.readEntry(entryName(d->configAlreadyScannedHandling),
                                           QString::fromLatin1("Skip"));

    if      (skipHandling == QLatin1String("Rescan"))
    {

#ifdef ENABLE_DETECT_AND_RECOGNIZE

        handling = FaceScanSettings::Rescan;

#else

        handling = FaceScanSettings::Skip;

#endif

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

    d->useYoloV3Button->setChecked(ApplicationSettings::instance()->getFaceDetectionYoloV3());

    d->useFullCpuButton->setChecked(group.readEntry(entryName(d->configUseFullCpu), false));
}

void FaceScanWidget::doSaveState()
{
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
        {
            handling = QLatin1String("Skip");
            break;
        }

        case FaceScanSettings::Rescan:
        {
            handling = QLatin1String("Rescan");
            break;
        }

        case FaceScanSettings::Merge:
        {
            handling = QLatin1String("Merge");
            break;
        }
    }

    group.writeEntry(entryName(d->configAlreadyScannedHandling), handling);

    ApplicationSettings::instance()->setFaceDetectionAccuracy(double(d->accuracyInput->value()) / 100);
    d->albumSelectors->saveState();

    ApplicationSettings::instance()->setFaceDetectionYoloV3(d->useYoloV3Button->isChecked());

    group.writeEntry(entryName(d->configUseFullCpu), d->useFullCpuButton->isChecked());
}

void FaceScanWidget::setupUi()
{
    // ---- Workflow tab --------

    d->workflowWidget                   = new QWidget(this);
    d->workflowWidget->setToolTip(i18nc("@info:tooltip",
                                        "digiKam can search for faces in your photos.\n"
                                        "When you have identified your friends on a number of photos,\n"
                                        "it can also recognize the people shown on your photos."));

    QVBoxLayout* const optionLayout     = new QVBoxLayout;

    QHBoxLayout* const scanOptionLayout = new QHBoxLayout;

    d->alreadyScannedBox                = new SqueezedComboBox;
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Skip images already scanned"),          FaceScanSettings::Skip);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Scan again and merge results"),         FaceScanSettings::Merge);

    QString buttonText;
    d->helpButton = new QPushButton(QIcon::fromTheme(QLatin1String("help-browser")), buttonText);
    d->helpButton->setToolTip(i18nc("@info","Help"));

    connect(d->helpButton, &QPushButton::clicked,
            this, []()
        {
            FaceManagementHelpDlg* const helpBox = new FaceManagementHelpDlg(qApp->activeWindow());
            helpBox->show();
        }
    );

    scanOptionLayout->addWidget(d->alreadyScannedBox, 9);
    scanOptionLayout->addWidget(d->helpButton, 1);

    optionLayout->addLayout(scanOptionLayout);

#ifdef ENABLE_DETECT_AND_RECOGNIZE

    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Clear unconfirmed results and rescan"), FaceScanSettings::Rescan);

#endif

    d->alreadyScannedBox->setCurrentIndex(FaceScanSettings::Skip);

    d->detectButton                   = new QRadioButton(i18nc("@option:radio", "Detect faces"));
    d->detectButton->setToolTip(i18nc("@info", "Find all faces in your photos"));

#ifdef ENABLE_DETECT_AND_RECOGNIZE

    d->detectAndRecognizeButton       = new QRadioButton(i18nc("@option:radio", "Detect and recognize faces"));
    d->detectAndRecognizeButton->setToolTip(i18nc("@info", "Find all faces in your photos and\n"
                                                           "try to recognize which person is depicted"));
#endif

    d->reRecognizeButton              = new QRadioButton(i18nc("@option:radio", "Recognize faces"));
    d->reRecognizeButton->setToolTip(i18nc("@info", "Try again to recognize the people depicted\n"
                                                    "on marked but yet unconfirmed faces."));

    optionLayout->addWidget(d->detectButton);

#ifdef ENABLE_DETECT_AND_RECOGNIZE

    optionLayout->addWidget(d->detectAndRecognizeButton);

#endif

    optionLayout->addWidget(d->reRecognizeButton);

#ifdef ENABLE_DETECT_AND_RECOGNIZE

    QStyleOptionButton buttonOption;
    buttonOption.initFrom(d->detectAndRecognizeButton);
    int indent = style()->subElementRect(QStyle::SE_RadioButtonIndicator, &buttonOption, d->detectAndRecognizeButton).width();
    optionLayout->setColumnMinimumWidth(0, indent);

#endif

    d->workflowWidget->setLayout(optionLayout);
    addTab(d->workflowWidget, i18nc("@title:tab", "Workflow"));

    // ---- Album tab ---------

    d->albumSelectors                 = new AlbumSelectors(QString(), d->configName, this);
    addTab(d->albumSelectors, i18nc("@title:tab", "Search in"));

    // ---- Settings tab ------

    QWidget* const settingsTab        = new QWidget(this);
    QVBoxLayout* const settingsLayout = new QVBoxLayout(settingsTab);

    QGroupBox* const accuracyBox      = new QGroupBox(i18nc("@label", "Face Accuracy"), settingsTab);
    QGridLayout* const accuracyGrid   = new QGridLayout(accuracyBox);

    QLabel* const sensitivityLabel    = new QLabel(i18nc("@label left extremities of a scale", "Sensitivity"), settingsTab);
    sensitivityLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QLabel* const specificityLabel    = new QLabel(i18nc("@label right extremities of a scale", "Specificity"), settingsTab);
    specificityLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);

    d->accuracyInput                  = new DIntNumInput(settingsTab);
    d->accuracyInput->setDefaultValue(70);
    d->accuracyInput->setRange(0, 100, 10);
    d->accuracyInput->setToolTip(i18nc("@info:tooltip",
                                       "Adjust sensitivity versus specificity: the higher the value, the more accurately faces will\n"
                                       "be recognized, but less faces will be recognized\n"
                                       "(only faces that are very similar to pre-tagged faces are recognized)."));

    accuracyGrid->addWidget(d->accuracyInput, 0, 0, 1, 3);
    accuracyGrid->addWidget(sensitivityLabel, 1, 0, 1, 1);
    accuracyGrid->addWidget(specificityLabel, 1, 2, 1, 1);
    accuracyGrid->setColumnStretch(1, 10);

    d->useYoloV3Button                = new QCheckBox(settingsTab);
    d->useYoloV3Button->setText(i18nc("@option:check", "Use YOLO v3 detection model"));
    d->useYoloV3Button->setToolTip(i18nc("@info:tooltip",
                                         "Face detection with YOLO v3 data model. Better results but slower."));

    d->useFullCpuButton               = new QCheckBox(settingsTab);
    d->useFullCpuButton->setText(i18nc("@option:check", "Work on all processor cores"));
    d->useFullCpuButton->setToolTip(i18nc("@info:tooltip",
                                          "Face detection and recognition are time-consuming tasks.\n"
                                          "You can choose if you wish to employ all processor cores\n"
                                          "on your system, or work in the background only on one core."));

    settingsLayout->addWidget(accuracyBox);
    settingsLayout->addWidget(d->useYoloV3Button);
    settingsLayout->addWidget(d->useFullCpuButton);

    settingsLayout->addStretch(10);

    addTab(settingsTab, i18nc("@title:tab", "Settings"));
}

void FaceScanWidget::setupConnections()
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

    connect(d->accuracyInput, &DIntNumInput::valueChanged,
            this, [this](int value)
        {
            ApplicationSettings::instance()->setFaceDetectionAccuracy(double(value) / 100);
        }
    );

    connect(d->useYoloV3Button, &QCheckBox::toggled,
            this, [this](bool yolo)
        {
            ApplicationSettings::instance()->setFaceDetectionYoloV3(yolo);
        }
    );
}

void FaceScanWidget::slotPrepareForDetect(bool status)
{
    d->alreadyScannedBox->setEnabled(status);
    d->albumSelectors->setEnabled(status);
}

void FaceScanWidget::slotPrepareForRecognize(bool /*status*/)
{
    d->alreadyScannedBox->setEnabled(false);
    d->albumSelectors->setEnabled(false);
}

bool FaceScanWidget::settingsConflicted() const
{
    return d->settingsConflicted;
}

FaceScanSettings FaceScanWidget::settings() const
{
    FaceScanSettings settings;

    d->settingsConflicted = false;

    if (d->detectButton->isChecked())
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
    settings.albums                 = d->albumSelectors->selectedAlbumsAndTags();

    if (d->settingsConflicted)
    {
        int numberOfIdentities      = FaceDbAccess().db()->getNumberOfIdentities();
        d->settingsConflicted       = (numberOfIdentities == 0);
    }

    settings.useYoloV3              = d->useYoloV3Button->isChecked();
    settings.useFullCpu             = d->useFullCpuButton->isChecked();

    return settings;
}

} // namespace Digikam
