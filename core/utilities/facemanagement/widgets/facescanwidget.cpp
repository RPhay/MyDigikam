/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Widget to choose options for face scanning
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// NOTE: Uncomment this line to enable detect and recognize option
// Currently this option is hidden, since it's not handled properly and provides confusing functionality => Fix it later
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
    handling = (FaceScanSettings::AlreadyScannedHandling)group.readEntry(entryName(d->configAlreadyScannedHandling),
                                                                         (int)FaceScanSettings::Skip);

    d->alreadyScannedBox->setCurrentIndex(d->alreadyScannedBox->findData(handling));


    d->albumSelectors->loadState();

    d->detectAccuracyInput->setValue(ApplicationSettings::instance()->getFaceDetectionAccuracy() * 100);
    d->detectModelBox->setCurrentIndex(d->detectModelBox->findData(ApplicationSettings::instance()->getFaceDetectionModel()));
    d->detectSizeBox->setCurrentIndex(d->detectSizeBox->findData(ApplicationSettings::instance()->getFaceDetectionSize()));
    d->recognizeAccuracyInput->setValue(ApplicationSettings::instance()->getFaceRecognitionAccuracy() * 100);
    d->recognizeModelBox->setCurrentIndex(d->recognizeModelBox->findData(ApplicationSettings::instance()->getFaceRecognitionModel()));

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
    group.writeEntry(entryName(d->configAlreadyScannedHandling),
                               d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt());

    d->albumSelectors->saveState();

    ApplicationSettings::instance()->setFaceDetectionAccuracy(double(d->detectAccuracyInput->value()) / 100);
    ApplicationSettings::instance()->setFaceDetectionModel(static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt()));
    ApplicationSettings::instance()->setFaceDetectionSize(static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt()));
    ApplicationSettings::instance()->setFaceRecognitionAccuracy(double(d->recognizeAccuracyInput->value()) / 100);
    ApplicationSettings::instance()->setFaceRecognitionModel(static_cast<FaceScanSettings::FaceRecognitionModel>(d->recognizeModelBox->currentData().toInt()));

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
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Skip images already scanned"),           FaceScanSettings::Skip);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Scan again and merge results"),          FaceScanSettings::Merge);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Clear unconfirmed results and rescan"),  FaceScanSettings::Rescan);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Clear all previous results and rescan"), FaceScanSettings::ClearAll);

    QString buttonText;
    d->helpButton                       = new QPushButton(QIcon::fromTheme(QLatin1String("help-browser")), buttonText);
    d->helpButton->setToolTip(i18nc("@info", "Help"));

    connect(d->helpButton, &QPushButton::clicked,
            this, []()
        {
            openOnlineDocumentation(QLatin1String("main_window"), QLatin1String("people_view"));
        }
    );

    scanOptionLayout->addWidget(d->alreadyScannedBox, 9);
    scanOptionLayout->addWidget(d->helpButton,        1);

    optionLayout->addLayout(scanOptionLayout);

    d->alreadyScannedBox->setCurrentIndex(FaceScanSettings::Skip);

    d->detectButton                     = new QRadioButton(i18nc("@option:radio", "Detect faces"));
    d->detectButton->setToolTip(i18nc("@info", "Find all faces in your photos"));

#ifdef ENABLE_DETECT_AND_RECOGNIZE

    d->detectAndRecognizeButton         = new QRadioButton(i18nc("@option:radio", "Detect and recognize faces"));
    d->detectAndRecognizeButton->setToolTip(i18nc("@info", "Find all faces in your photos and\n"
                                                           "try to recognize which person is depicted"));
#endif

    d->reRecognizeButton                = new QRadioButton(i18nc("@option:radio", "Recognize faces"));
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

    d->albumSelectors                   = new AlbumSelectors(QString(), d->configName,
                                                             this, AlbumSelectors::AlbumType::All, true);
    addTab(d->albumSelectors, i18nc("@title:tab", "Search in"));

    // ---- Settings tab ------

    d->settingsTab                      = new QWidget(this);
    QVBoxLayout* const settingsLayout   = new QVBoxLayout(d->settingsTab);

    DExpanderBox* expBox                = new DExpanderBox(d->settingsTab);
    QWidget* const detectWidget         = new QWidget(expBox);
    QGridLayout* const detectGrid       = new QGridLayout(detectWidget);

    QWidget* const recognizeWidget      = new QWidget(expBox);
    QGridLayout* const recognizeGrid    = new QGridLayout(recognizeWidget);

    // ----- detection settings

    QLabel* const detectAccuracyLabel   = new QLabel(i18nc("@label Face Detection Accuracy", "Detection Accuracy:"), d->settingsTab);
    detectAccuracyLabel->setAlignment(Qt::AlignLeft);

    d->detectAccuracyInput              = new DIntNumInput(d->settingsTab);
    d->detectAccuracyInput->setDefaultValue(70);
    d->detectAccuracyInput->setRange(0, 100, 10);
    d->detectAccuracyInput->setToolTip(i18nc("@info:tooltip",
                                       "Adjust sensitivity versus specificity: the higher the value, the more accurately faces will\n"
                                       "be detected, but less faces will be detected."));

    QLabel* const detectModelLabel      = new QLabel(i18nc("@label AI model used for detection", "AI Model:"), d->settingsTab);
    detectModelLabel->setAlignment(Qt::AlignLeft);

    d->detectModelBox                   = new SqueezedComboBox(d->settingsTab);
    d->detectModelBox->addSqueezedItem(i18nc("@label:listbox", "YuNet"),    FaceScanSettings::FaceDetectionModel::YuNet);
    d->detectModelBox->addSqueezedItem(i18nc("@label:listbox", "YOLOv3"),   FaceScanSettings::FaceDetectionModel::YOLOv3);
    d->detectModelBox->setEditable(false);
    d->detectModelBox->setToolTip(i18nc("@info:tooltip",
                                       "AI model used to detect faces.  YuNet is the new defaut model.  It is faster and more configurable than YOLOv3."));

    QLabel* const detectSizeLabel       = new QLabel(i18nc("@label AI model used for detection", "Face size:"), d->settingsTab);
    detectSizeLabel->setAlignment(Qt::AlignLeft);

    d->detectSizeBox                    = new SqueezedComboBox(d->settingsTab);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Extra Small"),   FaceScanSettings::FaceDetectionSize::extra_small);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Small"),         FaceScanSettings::FaceDetectionSize::small);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Medium"),        FaceScanSettings::FaceDetectionSize::medium);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Large"),         FaceScanSettings::FaceDetectionSize::large);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Extra Large"),   FaceScanSettings::FaceDetectionSize::extra_large);
    d->detectSizeBox->setEditable(false);
    d->detectSizeBox->setToolTip(i18nc("@info:tooltip",
                                       "Detecting smaller faces takes more time and has a higher probability of incorrect facial detection.\n"
                                       "Detecting larger faces is faster, but may miss smaller faces in group pictures."));

    // ----- recognition settings

    QLabel* const recognizeAccuracyLabel = new QLabel(i18nc("@label Face Recognition Accuracy", "Recognition Accuracy:"), d->settingsTab);
    recognizeAccuracyLabel->setAlignment(Qt::AlignLeft);

    d->recognizeAccuracyInput           = new DIntNumInput(d->settingsTab);
    d->recognizeAccuracyInput->setDefaultValue(70);
    d->recognizeAccuracyInput->setRange(0, 100, 10);
    d->recognizeAccuracyInput->setToolTip(i18nc("@info:tooltip",
                                                "Adjust sensitivity versus specificity: the higher the value, the more accurately faces will\n"
                                                "be recognized, but less faces will be recognized\n"
                                                "(only faces that are very similar to pre-tagged faces are recognized)."));

    QLabel* const recognizeModelLabel   = new QLabel(i18nc("@label AI model used for recognition", "Recognition model:"), d->settingsTab);
    recognizeModelLabel->setAlignment(Qt::AlignLeft);

    d->recognizeModelBox                = new SqueezedComboBox(d->settingsTab);
    d->recognizeModelBox->addSqueezedItem(i18nc("@label:listbox", "SFace"),     FaceScanSettings::FaceRecognitionModel::SFace);
    d->recognizeModelBox->addSqueezedItem(i18nc("@label:listbox", "OpenFace"),  FaceScanSettings::FaceRecognitionModel::OpenFace);
    d->recognizeModelBox->setEditable(false);
    d->recognizeModelBox->setToolTip(i18nc("@info:tooltip",
                                           "SFace is the new default AI model.  It is faster ad more accurate .\n"
                                           "OpenFace can be used for older libraries."));

    // ----- build the boxes

    detectGrid->addWidget(detectAccuracyLabel,      0, 0, 1, 3);
    detectGrid->addWidget(d->detectAccuracyInput,   1, 0, 1, 3);
    detectGrid->addWidget(detectModelLabel,         2, 0, 1, 1);
    detectGrid->addWidget(d->detectModelBox,        2, 2, 1, 1);
    detectGrid->addWidget(detectSizeLabel,          3, 0, 1, 1);
    detectGrid->addWidget(d->detectSizeBox,         3, 2, 1, 1);

    recognizeGrid->addWidget(recognizeAccuracyLabel,    0, 0, 1, 3);
    recognizeGrid->addWidget(d->recognizeAccuracyInput, 1, 0, 1, 3);
    recognizeGrid->addWidget(recognizeModelLabel,       2, 0, 1, 3);
    recognizeGrid->addWidget(d->recognizeModelBox,      2, 2, 1, 1);

    d->useFullCpuButton                 = new QCheckBox(d->settingsTab);
    d->useFullCpuButton->setText(i18nc("@option:check", "Work on all processor cores"));
    d->useFullCpuButton->setToolTip(i18nc("@info:tooltip",
                                          "Face detection and recognition are time-consuming tasks.\n"
                                          "You can choose if you wish to employ all processor cores\n"
                                          "on your system, or work in the background only on one core."));

    expBox->addItem(detectWidget,    i18n("Face Detection Settings"),   QString::fromUtf8("Face Detection Settings"),   false);
    expBox->addItem(recognizeWidget, i18n("Face Recognition Settings"), QString::fromUtf8("Face Recognition Settings"), false);
    expBox->setItemExpanded(0, true);
    expBox->setItemExpanded(1, false);

    settingsLayout->addWidget(expBox);
    settingsLayout->addWidget(d->useFullCpuButton);

    addTab(d->settingsTab, i18nc("@title:tab", "Settings"));
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

    connect(d->detectAccuracyInput, &DIntNumInput::valueChanged,
            this, [this](int value)
        {
            ApplicationSettings::instance()->setFaceDetectionAccuracy(double(value) / 100.0);
        }
    );

    connect(d->detectModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FaceScanWidget::slotDetectModelChanged);

    connect(d->detectSizeBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FaceScanWidget::slotDetectSizeChanged);
}

void FaceScanWidget::slotPrepareForDetect(bool status)
{
    d->alreadyScannedBox->setEnabled(status);
}

void FaceScanWidget::slotPrepareForRecognize(bool /*status*/)
{
    d->alreadyScannedBox->setEnabled(false);
}

void FaceScanWidget::slotDetectModelChanged()
{
    ApplicationSettings::instance()->setFaceDetectionModel(static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt()));
}

void FaceScanWidget::slotDetectSizeChanged()
{
    ApplicationSettings::instance()->setFaceDetectionSize(static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt()));
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

    settings.albums                 = d->albumSelectors->selectedAlbumsAndTags();
    settings.wholeAlbums            = d->albumSelectors->wholeAlbumsChecked();

    if (d->settingsConflicted)
    {
        int numberOfIdentities      = FaceDbAccess().db()->getNumberOfIdentities();
        d->settingsConflicted       = (numberOfIdentities == 0);
    }

    settings.detectAccuracy         = d->detectAccuracyInput->value() / 100.0;
    settings.detectModel            = static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt());
    settings.detectSize             = static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt());
    settings.recognizeAccuracy      = d->recognizeAccuracyInput->value() / 100.0;
    settings.recognizeModel         = static_cast<FaceScanSettings::FaceRecognitionModel>(d->recognizeModelBox->currentData().toInt());

    settings.useFullCpu             = d->useFullCpuButton->isChecked();

    return settings;
}

} // namespace Digikam

#include "moc_facescanwidget.cpp"
