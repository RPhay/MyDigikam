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
#include "recognitionmodelchange.h"
#include "peoplesidebarwidget.h"

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

    ApplicationSettings::instance()->setFaceDetectionAccuracy(d->detectAccuracyInput->value() / 100.0);
    ApplicationSettings::instance()->setFaceDetectionModel(static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt()));
    ApplicationSettings::instance()->setFaceDetectionSize(static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt()));
    ApplicationSettings::instance()->setFaceRecognitionAccuracy(d->recognizeAccuracyInput->value() / 100.0);
    ApplicationSettings::instance()->setFaceRecognitionModel(static_cast<FaceScanSettings::FaceRecognitionModel>(d->recognizeModelBox->currentData().toInt()));

    group.writeEntry(entryName(d->configUseFullCpu), d->useFullCpuButton->isChecked());
}

void FaceScanWidget::setupUi()
{
    // --- Workflow tab ------------------------------------------------------------------------------------

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
    optionLayout->addStretch();

#ifdef ENABLE_DETECT_AND_RECOGNIZE

    QStyleOptionButton buttonOption;
    buttonOption.initFrom(d->detectAndRecognizeButton);
    int indent = style()->subElementRect(QStyle::SE_RadioButtonIndicator, &buttonOption, d->detectAndRecognizeButton).width();
    optionLayout->setColumnMinimumWidth(0, indent);

#endif

    d->workflowWidget->setLayout(optionLayout);
    addTab(d->workflowWidget, i18nc("@title:tab", "Workflow"));

    // --- Album tab --------------------------------------------------------------------------------------

    d->albumSelectors                   = new AlbumSelectors(QString(), d->configName,
                                                             this, AlbumSelectors::AlbumType::All, true);
    addTab(d->albumSelectors, i18nc("@title:tab", "Search in"));

    // --- Settings tab -----------------------------------------------------------------------------------

    d->settingsTab                      = new QWidget(this);
    QVBoxLayout* const settingsLayout   = new QVBoxLayout(d->settingsTab);

    DExpanderBox* const expBox          = new DExpanderBox(d->settingsTab);

    // --- detection settings

    QWidget* const detectWidget         = new QWidget(expBox);
    QGridLayout* const detectGrid       = new QGridLayout(detectWidget);

    QLabel* const detectAccuracyLabel   = new QLabel(i18nc("@label Face Detection Accuracy",
                                                           "Accuracy:"), d->settingsTab);
    detectAccuracyLabel->setAlignment(Qt::AlignLeft);

    d->detectAccuracyInput              = new DIntNumInput(d->settingsTab);
    d->detectAccuracyInput->setDefaultValue(70);
    d->detectAccuracyInput->setRange(0, 100, 10);
    d->detectAccuracyInput->setToolTip(i18nc("@info:tooltip",
                                             "Adjust sensitivity versus specificity: the higher the value, "
                                             "the more accurately faces will\n"
                                             "be detected, but less faces will be detected."));

    QLabel* const detectModelLabel      = new QLabel(i18nc("@label AI model used for face detection",
                                                           "Model:"), d->settingsTab);
    detectModelLabel->setAlignment(Qt::AlignLeft);

    d->detectModelBox                   = new SqueezedComboBox(d->settingsTab);


    d->detectModelBox->addSqueezedItem(i18nc("@label:listbox", "YuNet"),    FaceScanSettings::FaceDetectionModel::YuNet);
    d->detectModelBox->addSqueezedItem(i18nc("@label:listbox", "YOLOv3"),   FaceScanSettings::FaceDetectionModel::YOLOv3);
    d->detectModelBox->addSqueezedItem(i18nc("@label:listbox", "SSD"),      FaceScanSettings::FaceDetectionModel::SSDMOBILENET);
    d->detectModelBox->setEditable(false);
    d->detectModelBox->setToolTip(i18nc("@info:tooltip",
                                        "Detection model used to find faces. YuNet is the default model.\n"
                                        "It is faster and more configurable than YOLOv3.\n"
                                        "Note: YuNet is only available if digiKam is compiled with OpenCV 4.6.0 or later."));

#if OPENCV_TEST_VERSION(4,6,0)

    auto* const detModel = qobject_cast<QStandardItemModel*>(d->detectModelBox->model());

    if (detModel)
    {
        auto* const detItem = detModel->item(0);

        if (detItem)
        {
            detItem->setFlags(detItem->flags() & ~Qt::ItemIsEnabled);
        }
    }

#endif

    QLabel* const detectSizeLabel       = new QLabel(i18nc("@label face size for detection",
                                                           "Face size:"), d->settingsTab);
    detectSizeLabel->setAlignment(Qt::AlignLeft);

    d->detectSizeBox                    = new SqueezedComboBox(d->settingsTab);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Extra Small"),   FaceScanSettings::FaceDetectionSize::ExtraSmall);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Small"),         FaceScanSettings::FaceDetectionSize::Small);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Medium"),        FaceScanSettings::FaceDetectionSize::Medium);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Large"),         FaceScanSettings::FaceDetectionSize::Large);
    d->detectSizeBox->addSqueezedItem(i18nc("@label:listbox", "Extra Large"),   FaceScanSettings::FaceDetectionSize::ExtraLarge);
    d->detectSizeBox->setEditable(false);
    d->detectSizeBox->setToolTip(i18nc("@info:tooltip",
                                       "<p>Selecting <b>Extra Small</b> means the model will detect small background faces in addition "
                                       "to medium and larger faces. Selecting <b>Small Faces</b> increases the sensitivity of the model "
                                       "by tuning the model parameters. It will also increase the probability of false-positives, "
                                       "and it will increase detection time.</p>"
                                       "<p>Selecting <b>Large</b> or <b>Extra Large</b> will eliminate small background faces from being detected. "
                                       "It is much faster and will reduce the number of false-positives, but will not detect small faces "
                                       "in the the background or faces in large group photos.</p>"
                                       "<p>If you want to detect all faces in one pass, select <b>Extra Small</b> faces and set the detection "
                                       "accuracy to about 45%. Unfortunately, this will give a significant number of false-positives. "
                                       "It's recommended to set a detection accuracy of about 55% and face size of <b>Small</b> or <b>Medium</b> "
                                       "for normal use.</p><p>This setting applies only to YuNet.</p>"));

    detectGrid->addWidget(detectAccuracyLabel,      0, 0, 1, 3);
    detectGrid->addWidget(d->detectAccuracyInput,   1, 0, 1, 3);
    detectGrid->addWidget(detectModelLabel,         2, 0, 1, 1);
    detectGrid->addWidget(d->detectModelBox,        2, 2, 1, 1);
    detectGrid->addWidget(detectSizeLabel,          3, 0, 1, 1);
    detectGrid->addWidget(d->detectSizeBox,         3, 2, 1, 1);

    expBox->addItem(detectWidget, i18n("Face Detection Settings"),
                    QLatin1String("FaceDetectionSettings"), true);

    // --- recognition settings

    QWidget* const recognizeWidget       = new QWidget(expBox);
    QGridLayout* const recognizeGrid     = new QGridLayout(recognizeWidget);

    QLabel* const recognizeAccuracyLabel = new QLabel(i18nc("@label Face Recognition Accuracy",
                                                            "Accuracy:"), d->settingsTab);
    recognizeAccuracyLabel->setAlignment(Qt::AlignLeft);

    d->recognizeAccuracyInput            = new DIntNumInput(d->settingsTab);
    d->recognizeAccuracyInput->setDefaultValue(70);
    d->recognizeAccuracyInput->setRange(0, 100, 10);
    d->recognizeAccuracyInput->setToolTip(i18nc("@info:tooltip",
                                                "Adjust sensitivity versus specificity: the higher the value, the more accurately faces will\n"
                                                "be recognized, but less faces will be recognized\n"
                                                "Note: only faces that are very similar to pre-tagged faces are recognized."));

    QLabel* const recognizeModelLabel    = new QLabel(i18nc("@label AI model used for face recognition",
                                                            "Model:"), d->settingsTab);
    recognizeModelLabel->setAlignment(Qt::AlignLeft);

    d->recognizeModelBox                 = new SqueezedComboBox(d->settingsTab);
    d->recognizeModelBox->addSqueezedItem(i18nc("@label:listbox", "SFace"),     FaceScanSettings::FaceRecognitionModel::SFace);
    d->recognizeModelBox->addSqueezedItem(i18nc("@label:listbox", "OpenFace"),  FaceScanSettings::FaceRecognitionModel::OpenFace);
    d->recognizeModelBox->setEditable(false);
    d->recognizeModelBox->setToolTip(i18nc("@info:tooltip",
                                           "SFace is the default detection model. It is faster and more accurate.\n"
                                           "OpenFace can be used for older libraries.\n"
                                           "Note: SFace is only available if digiKam is compiled with OpenCV 4.6.0 or later."));
#if OPENCV_TEST_VERSION(4,6,0)

    auto* const recModel = qobject_cast<QStandardItemModel*>(d->recognizeModelBox->model());

    if (recModel)
    {
        auto* const recItem = recModel->item(0);

        if (recItem)
        {
            recItem->setFlags(recItem->flags() & ~Qt::ItemIsEnabled);
        }
    }

#endif

    recognizeGrid->addWidget(recognizeAccuracyLabel,    0, 0, 1, 3);
    recognizeGrid->addWidget(d->recognizeAccuracyInput, 1, 0, 1, 3);
    recognizeGrid->addWidget(recognizeModelLabel,       2, 0, 1, 3);
    recognizeGrid->addWidget(d->recognizeModelBox,      2, 2, 1, 1);

    // ---

    d->useFullCpuButton                 = new QCheckBox(d->settingsTab);
    d->useFullCpuButton->setText(i18nc("@option:check", "Work on all processor cores"));
    d->useFullCpuButton->setToolTip(i18nc("@info:tooltip",
                                          "Face detection and recognition are time-consuming tasks.\n"
                                          "You can choose if you wish to employ all processor cores\n"
                                          "on your system, or work in the background only on one core."));

    expBox->addItem(recognizeWidget, i18n("Face Recognition Settings"),
                    QLatin1String("FaceRecognitionSettings"), false);

    expBox->addStretch();

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

    connect(d->recognizeModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FaceScanWidget::slotRecognizeModelChanged);

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

    if (FaceScanSettings::FaceDetectionModel::YuNet ==
        static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt()))
    {
        d->detectSizeBox->setEnabled(true);
    }
    else
    {
        d->detectSizeBox->setEnabled(false);
    }


}

void FaceScanWidget::slotDetectSizeChanged()
{
    ApplicationSettings::instance()->setFaceDetectionSize(static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt()));
}

void FaceScanWidget::slotRecognizeModelChanged()
{
    // save the model values if we have to revert
    FaceScanSettings::FaceRecognitionModel oldModel = ApplicationSettings::instance()->getFaceRecognitionModel();
    FaceScanSettings::FaceRecognitionModel newModel = static_cast<FaceScanSettings::FaceRecognitionModel>(d->recognizeModelBox->currentData().toInt());
    ChangeFaceRecognitionModelDlg* dlg              = new ChangeFaceRecognitionModelDlg(this, newModel);

    // show the upgrade warning dialog box
    if (d->recognizeModelBox->isVisible() && QDialog::Accepted == dlg->exec())
    {

        // upgrade was approved.  Save new value
        ApplicationSettings* const appSettings = ApplicationSettings::instance();
        appSettings->setFaceRecognitionModel(newModel);
        appSettings->saveSettings();

        // start retraining and update pipeline here
        FaceScanSettings settings;

        settings.wholeAlbums            = true;
        settings.useFullCpu             = d->useFullCpuButton->isChecked();
        settings.detectModel            = ApplicationSettings::instance()->getFaceDetectionModel();
        settings.detectSize             = ApplicationSettings::instance()->getFaceDetectionSize();
        settings.detectAccuracy         = ApplicationSettings::instance()->getFaceDetectionAccuracy();
        settings.recognizeModel         = newModel;
        settings.recognizeAccuracy      = ApplicationSettings::instance()->getFaceRecognitionAccuracy();
        settings.task                   = FaceScanSettings::ScanTask::RetrainAll;

        PeopleSideBarWidget::doFaceScan(settings);

    }
    else
    {

        // reselect the old model value in the drop-down
        d->recognizeModelBox->setCurrentIndex(d->recognizeModelBox->findData(oldModel));

    }

    // clean up the dialog
    delete dlg;
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
