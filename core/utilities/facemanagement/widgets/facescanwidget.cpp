/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Widget to choose options for face scanning
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facescanwidget_p.h"
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

    FaceScanSettings::AlreadyScannedHandling handling;
    handling = (FaceScanSettings::AlreadyScannedHandling)group.readEntry(entryName(d->configAlreadyScannedHandling),
                                                                         (int)FaceScanSettings::Skip);

    /**
     * ClearAll isn't a valid value anymore so set it Rescan.
     * ClearAll is only used by ResetFacesDb in maintenance.
     */

    if (FaceScanSettings::AlreadyScannedHandling::ClearAll == handling)
    {
        handling = FaceScanSettings::AlreadyScannedHandling::Rescan;
    }

    d->alreadyScannedBox->setCurrentIndex(d->alreadyScannedBox->findData(handling));

    d->albumSelectors->loadState();

    d->detectAccuracyInput->setValue(ApplicationSettings::instance()->getFaceDetectionAccuracy());
    // d->detectModelBox->setCurrentIndex(d->detectModelBox->findData(ApplicationSettings::instance()->getFaceDetectionModel()));
    d->detectSizeBox->setCurrentIndex(d->detectSizeBox->findData(ApplicationSettings::instance()->getFaceDetectionSize()));
    d->recognizeAccuracyInput->setValue(ApplicationSettings::instance()->getFaceRecognitionAccuracy());
    // d->recognizeModelBox->setCurrentIndex(d->recognizeModelBox->findData(ApplicationSettings::instance()->getFaceRecognitionModel()));

    d->useFullCpuButton->setChecked(group.readEntry(entryName(d->configUseFullCpu), false));
}

void FaceScanWidget::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    group.writeEntry(entryName(d->configAlreadyScannedHandling),
                     d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt());

    d->albumSelectors->saveState();

    ApplicationSettings::instance()->setFaceDetectionAccuracy(d->detectAccuracyInput->value());
    // ApplicationSettings::instance()->setFaceDetectionModel(static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt()));
    ApplicationSettings::instance()->setFaceDetectionSize(static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt()));
    ApplicationSettings::instance()->setFaceRecognitionAccuracy(d->recognizeAccuracyInput->value());
    // ApplicationSettings::instance()->setFaceRecognitionModel(static_cast<FaceScanSettings::FaceRecognitionModel>(d->recognizeModelBox->currentData().toInt()));

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
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Scan new images"),           FaceScanSettings::Skip);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Scan all images"),           FaceScanSettings::Rescan);
    d->alreadyScannedBox->addSqueezedItem(i18nc("@label:listbox", "Recognize faces only"),      FaceScanSettings::RecognizeOnly);
    d->alreadyScannedBox->setToolTip(i18nc("@info:tooltip",
                                           "<p><b>Scan new images</b> will scan for faces and attempt recognition for new images found in the albums selected in the \"Search in\" tab.</p>"
                                           "<p><b>Scan all images</b> will scan for faces and attempt recognition for all images found in the albums selected in the \"Search in\" tab.</p>"
                                           "<p><b>Recognize faces only</b> will try to match unknown faces with confirmed faces without scanning images new faces.</p>"));

    QString buttonText;
    d->helpButton                       = new QPushButton(QIcon::fromTheme(QLatin1String("help-browser")), buttonText);
    d->helpButton->setToolTip(i18nc("@info", "Help"));

    connect(d->helpButton, &QPushButton::clicked,
            this, []()
        {
            openOnlineDocumentation(QLatin1String("left_sidebar"), QLatin1String("people_view"));
        }
    );

    scanOptionLayout->addWidget(d->alreadyScannedBox, 9);
    scanOptionLayout->addWidget(d->helpButton,        1);

    optionLayout->addLayout(scanOptionLayout);

    d->alreadyScannedBox->setCurrentIndex(FaceScanSettings::Skip);

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

    // --- Detection settings ---

    QWidget* const detectWidget         = new QWidget(expBox);
    QGridLayout* const detectGrid       = new QGridLayout(detectWidget);

    QLabel* const detectAccuracyLabel   = new QLabel(i18nc("@label Face Detection Accuracy",
                                                           "Accuracy:"), d->settingsTab);
    detectAccuracyLabel->setAlignment(Qt::AlignLeft);

    d->detectAccuracyInput              = new DIntNumInput(d->settingsTab);
    d->detectAccuracyInput->setDefaultValue(7);
    d->detectAccuracyInput->setRange(1, 10, 1);
    d->detectAccuracyInput->setToolTip(i18nc("@info:tooltip",
                                             "Adjust sensitivity versus specificity: the higher the value, "
                                             "the more accurately faces will\n"
                                             "be detected, but less faces will be detected."));

    /**
     * @info Code to select the detection model

    QLabel* const detectModelLabel      = new QLabel(i18nc("@label AI model used for face detection",
                                                           "Model:"), d->settingsTab);
    detectModelLabel->setAlignment(Qt::AlignLeft);

    d->detectModelBox                   = new SqueezedComboBox(d->settingsTab);

    d->detectModelBox->addSqueezedItem(i18nc("@label:listbox", "YuNet"),  FaceScanSettings::FaceDetectionModel::YuNet);
    d->detectModelBox->addSqueezedItem(i18nc("@label:listbox", "YOLOv3"), FaceScanSettings::FaceDetectionModel::YOLOv3);
    d->detectModelBox->addSqueezedItem(i18nc("@label:listbox", "SSD"),    FaceScanSettings::FaceDetectionModel::SSDMOBILENET);
    d->detectModelBox->setEditable(false);
    d->detectModelBox->setToolTip(i18nc("@info:tooltip",
                                        "Detection model used to find faces. YuNet is the default model.\n"
                                        "It is faster and more configurable than SSD and YOLOv3.\n"
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
    */

    QLabel* const detectSizeLabel       = new QLabel(i18nc("@label face size for detection",
                                                           "Face size:"), d->settingsTab);
    detectSizeLabel->setAlignment(Qt::AlignLeft);

    d->detectSizeBox                    = new SqueezedComboBox(d->settingsTab);
    d->detectSizeBox->addSqueezedItem(i18nc("@label: listbox face size", "Extra Small"), FaceScanSettings::FaceDetectionSize::ExtraSmall);
    d->detectSizeBox->addSqueezedItem(i18nc("@label: listbox face size", "Small"),       FaceScanSettings::FaceDetectionSize::Small);
    d->detectSizeBox->addSqueezedItem(i18nc("@label: listbox face size", "Medium"),      FaceScanSettings::FaceDetectionSize::Medium);
    d->detectSizeBox->addSqueezedItem(i18nc("@label: listbox face size", "Large"),       FaceScanSettings::FaceDetectionSize::Large);
    d->detectSizeBox->addSqueezedItem(i18nc("@label: listbox face size", "Extra Large"), FaceScanSettings::FaceDetectionSize::ExtraLarge);
    d->detectSizeBox->setEditable(false);
    d->detectSizeBox->setToolTip(i18nc("@info:tooltip",
                                       "<p>Selecting <b>Extra Small</b> means the model will detect small background faces in addition "
                                       "to medium and larger faces. Selecting <b>Small Faces</b> increases the sensitivity of the model "
                                       "by tuning the model parameters. It will also increase the probability of false-positives, "
                                       "and it will increase detection time.</p>"
                                       "<p>Selecting <b>Large</b> or <b>Extra Large</b> will eliminate small background faces from being detected. "
                                       "It is much faster and will reduce the number of false-positives, but will not detect small faces "
                                       "in the background or faces in large group photos.</p>"
                                       "<p>If you want to detect all faces in one pass, select <b>Extra Small</b> faces and set the detection "
                                       "accuracy to 2. Unfortunately, this will create a significant number of false-positives. "
                                       "It is recommended to set a detection accuracy of 6-8 and face size of <b>Medium</b> or <b>Large</b> "
                                       "for normal use.</p><p>This setting applies only to YuNet.</p>"));

    detectGrid->addWidget(detectAccuracyLabel,      0, 0, 1, 1);
    detectGrid->addWidget(d->detectAccuracyInput,   0, 2, 1, 1);
    // detectGrid->addWidget(detectModelLabel,         1, 0, 1, 1);
    // detectGrid->addWidget(d->detectModelBox,        1, 2, 1, 1);
    detectGrid->addWidget(detectSizeLabel,          1, 0, 1, 1);
    detectGrid->addWidget(d->detectSizeBox,         1, 2, 1, 1);

    expBox->addItem(detectWidget, i18n("Face Detection Settings"),
                    QLatin1String("FaceDetectionSettings"), true);

    // --- Recognition settings ---

    QWidget* const recognizeWidget       = new QWidget(expBox);
    QGridLayout* const recognizeGrid     = new QGridLayout(recognizeWidget);

    QLabel* const recognizeAccuracyLabel = new QLabel(i18nc("@label Face Recognition Accuracy",
                                                            "Accuracy:"), d->settingsTab);
    recognizeAccuracyLabel->setAlignment(Qt::AlignLeft);

    d->recognizeAccuracyInput            = new DIntNumInput(d->settingsTab);
    d->recognizeAccuracyInput->setDefaultValue(7);
    d->recognizeAccuracyInput->setRange(1, 10, 1);
    d->recognizeAccuracyInput->setToolTip(i18nc("@info:tooltip",
                                                "Adjust sensitivity versus specificity: the higher the value, the more accurately faces will\n"
                                                "be recognized, but less faces will be recognized.\n"
                                                "Note: only faces that are very similar to pre-tagged faces are recognized."));

    /**
     * @info Code to select the recognition model

    QLabel* const recognizeModelLabel    = new QLabel(i18nc("@label AI model used for face recognition",
                                                            "Model:"), d->settingsTab);
    recognizeModelLabel->setAlignment(Qt::AlignLeft);

    d->recognizeModelBox                 = new SqueezedComboBox(d->settingsTab);
    d->recognizeModelBox->addSqueezedItem(i18nc("@label:listbox", "SFace"),    FaceScanSettings::FaceRecognitionModel::SFace);
    d->recognizeModelBox->addSqueezedItem(i18nc("@label:listbox", "OpenFace"), FaceScanSettings::FaceRecognitionModel::OpenFace);
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
    */

    recognizeGrid->addWidget(recognizeAccuracyLabel,    0, 0, 1, 1);
    recognizeGrid->addWidget(d->recognizeAccuracyInput, 0, 2, 1, 1);
    // recognizeGrid->addWidget(recognizeModelLabel,       1, 0, 1, 3);
    // recognizeGrid->addWidget(d->recognizeModelBox,      1, 2, 1, 1);

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

    // connect(d->detectButton, SIGNAL(toggled(bool)),
    //         this, SLOT(slotPrepareForDetect(bool)));

    // connect(d->reRecognizeButton, SIGNAL(toggled(bool)),
    //         this, SLOT(slotPrepareForRecognize(bool)));

    connect(d->detectAccuracyInput, &DIntNumInput::valueChanged,
            this, &FaceScanWidget::slotDetectAccuracyChanged);

    connect(d->recognizeAccuracyInput, &DIntNumInput::valueChanged,
            this, &FaceScanWidget::slotRecognizeAccuracyChanged);

    // connect(d->detectModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //         this, &FaceScanWidget::slotDetectModelChanged);

    connect(d->detectSizeBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FaceScanWidget::slotDetectSizeChanged);

    // connect(d->recognizeModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //         this, &FaceScanWidget::slotRecognizeModelChanged);
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
    // ApplicationSettings::instance()->setFaceDetectionModel(static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt()));

    // if (FaceScanSettings::FaceDetectionModel::YuNet ==
    //     static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt()))
    // {
    //     d->detectSizeBox->setEnabled(true);
    // }

    // else
    // {
    //     d->detectSizeBox->setEnabled(false);
    // }
}

void FaceScanWidget::slotDetectAccuracyChanged()
{
    ApplicationSettings::instance()->setFaceDetectionAccuracy(d->detectAccuracyInput->value());
}

void FaceScanWidget::slotDetectSizeChanged()
{
    ApplicationSettings::instance()->setFaceDetectionSize(static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt()));
}

void FaceScanWidget::slotRecognizeModelChanged()
{
    // // Save the model values if we have to revert.

    // FaceScanSettings::FaceRecognitionModel oldModel = ApplicationSettings::instance()->getFaceRecognitionModel();
    // FaceScanSettings::FaceRecognitionModel newModel = static_cast<FaceScanSettings::FaceRecognitionModel>(d->recognizeModelBox->currentData().toInt());
    // ChangeFaceRecognitionModelDlg* const dlg        = new ChangeFaceRecognitionModelDlg(this, newModel);

    // // Show the upgrade warning dialog box.

    // if (d->recognizeModelBox->isVisible() && (QDialog::Accepted == dlg->exec()))
    // {
    //     // Upgrade was approved. Save new value.

    //     ApplicationSettings* const appSettings = ApplicationSettings::instance();
    //     appSettings->setFaceRecognitionModel(newModel);
    //     appSettings->saveSettings();

    //     // Start retraining and update pipeline here.

    //     FaceScanSettings settings;

    //     settings.wholeAlbums            = true;
    //     settings.useFullCpu             = d->useFullCpuButton->isChecked();
    //     settings.detectModel            = ApplicationSettings::instance()->getFaceDetectionModel();
    //     settings.detectSize             = ApplicationSettings::instance()->getFaceDetectionSize();
    //     settings.detectAccuracy         = ApplicationSettings::instance()->getFaceDetectionAccuracy();
    //     settings.recognizeModel         = newModel;
    //     settings.recognizeAccuracy      = ApplicationSettings::instance()->getFaceRecognitionAccuracy();
    //     settings.task                   = FaceScanSettings::ScanTask::RetrainAll;

    //     PeopleSideBarWidget::doFaceScan(settings);
    // }

    // else
    // {
    //     // Disconnect so we don't get multiple dialogs.

    //     disconnect(d->recognizeModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //                this, &FaceScanWidget::slotRecognizeModelChanged);

    //     // Reselect the old model value in the drop-down.

    //     d->recognizeModelBox->setCurrentIndex(d->recognizeModelBox->findData(oldModel));

    //     // Reconnect for future notifications.

    //     connect(d->recognizeModelBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
    //             this, &FaceScanWidget::slotRecognizeModelChanged);
    // }

    // // Clean up the dialog.

    // delete dlg;
}

void FaceScanWidget::slotRecognizeAccuracyChanged()
{
    ApplicationSettings::instance()->setFaceRecognitionAccuracy(d->recognizeAccuracyInput->value());
}

bool FaceScanWidget::settingsConflicted() const
{
    return d->settingsConflicted;
}

FaceScanSettings FaceScanWidget::settings() const
{
    FaceScanSettings settings;

    d->settingsConflicted = false;

    if (FaceScanSettings::RecognizeOnly == d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt())
    {
        settings.task = FaceScanSettings::RecognizeMarkedFaces;
        settings.alreadyScannedHandling = FaceScanSettings::AlreadyScannedHandling::Rescan;

    }

    else
    {
        settings.task = FaceScanSettings::DetectAndRecognize;
        settings.alreadyScannedHandling = (FaceScanSettings::AlreadyScannedHandling)
                                          d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt();
    }

    // settings.alreadyScannedHandling = (FaceScanSettings::AlreadyScannedHandling)
    //                                   d->alreadyScannedBox->itemData(d->alreadyScannedBox->currentIndex()).toInt();

    settings.albums                 = d->albumSelectors->selectedAlbumsAndTags();
    settings.wholeAlbums            = d->albumSelectors->wholeAlbumsChecked();

    if (d->settingsConflicted)
    {
        int numberOfIdentities      = FaceDbAccess().db()->getNumberOfIdentities();
        d->settingsConflicted       = (numberOfIdentities == 0);
    }

    settings.detectAccuracy         = d->detectAccuracyInput->value();
    // settings.detectModel            = static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt());
    settings.detectModel            = FaceScanSettings::FaceDetectionModel::YuNet;
    settings.detectSize             = static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt());
    settings.recognizeAccuracy      = d->recognizeAccuracyInput->value();
    // settings.recognizeModel         = static_cast<FaceScanSettings::FaceRecognitionModel>(d->recognizeModelBox->currentData().toInt());
    settings.recognizeModel         = FaceScanSettings::FaceRecognitionModel::SFace;

    settings.useFullCpu             = d->useFullCpuButton->isChecked();

    return settings;
}

} // namespace Digikam

#include "moc_facescanwidget.cpp"
