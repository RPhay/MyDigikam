/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-01-06
 * Description : Widget to choose options for autotags scanning
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "autotagsscanwidget_p.h"

namespace Digikam
{

AutoTagsScanWidget::AutoTagsScanWidget(QWidget* const parent)
    : QTabWidget       (parent),
      StateSavingObject(this),
      d                (new Private)
{
    setObjectName(d->configName);
    setupUi();
}

AutoTagsScanWidget::~AutoTagsScanWidget()
{
    delete d;
}

void AutoTagsScanWidget::doLoadState()
{
    KConfigGroup group = getConfigGroup();
    d->albumSelectors->loadState();

    AutoTagsScanSettings prm;

    int tagScanMode    = d->autotaggingScanMode->findData(group.readEntry(d->configAutotaggingScanMode, (int)prm.mode));
    d->autotaggingScanMode->setCurrentIndex(tagScanMode);
    int tagSelection   = d->modelSelectionMode->findData(group.readEntry(d->configModelSelectionMode,   (int)prm.modelType));
    d->modelSelectionMode->setCurrentIndex(tagSelection);
    d->trSelectorList->clearLanguages();

    const auto lgs     = group.readEntry(d->configAutotagsLanguages,                                    prm.langs);

    for (const QString& lg : lgs)
    {
        d->trSelectorList->addLanguage(lg);
    }
}

void AutoTagsScanWidget::doSaveState()
{
    KConfigGroup group       = getConfigGroup();
    AutoTagsScanSettings prm = settings();

    d->albumSelectors->saveState();

    group.writeEntry(d->configAutotaggingScanMode, (int)prm.mode);
    group.writeEntry(d->configModelSelectionMode,  (int)prm.modelType);
    group.writeEntry(d->configAutotagsLanguages,   prm.langs);
}

void AutoTagsScanWidget::setupUi()
{
    // --- Album tab --------------------------------------------------------------------------------------

    d->albumSelectors                   = new AlbumSelectors(QString(), d->configName,
                                                             this, AlbumSelectors::AlbumType::All, true);
    addTab(d->albumSelectors, i18nc("@title:tab", "Search in"));

    // --- Settings tab -----------------------------------------------------------------------------------

    d->settingsTab                      = new QWidget(this);
    QVBoxLayout* const settingsLayout   = new QVBoxLayout(d->settingsTab);

    QLabel* const title    = new QLabel(d->settingsTab);
    title->setText(i18nc("@label",
                         "<p><b>This tool allows to assign automatically tags to images by contents analysis using "
                         "deep-learning neural network.</b></p>"
                         "<p>The settings below determines the deep-learning model to use while parsing image "
                         "contents to determine the subjects of the photography. The neural network used in background "
                         "will generate automatically a serie of tags describing the contents and store the results in "
                         "the database.</p>"));
    title->setWordWrap(true);

    DHBox* const hbox12    = new DHBox(d->settingsTab);
    new QLabel (i18n("Auto-tagging mode: "), hbox12);
    QWidget* const space8  = new QWidget(hbox12);
    hbox12->setStretchFactor(space8, 10);

    d->autotaggingScanMode = new QComboBox(hbox12);
    d->autotaggingScanMode->addItem(i18n("Clean all and re-assign"), AutoTagsScanSettings::AllItems);
    d->autotaggingScanMode->addItem(i18n("Scan non-assigned only"),  AutoTagsScanSettings::NonAssignedItems);
    d->autotaggingScanMode->setToolTip(i18nc("@info:tooltip",
        "<p><b>Clean all and re-assign</b>: clean all tags already assigned and re-scan all items from scratch.</p>"
        "<p><b>Scan non-assigned only</b>: scan only the items with no assigned tag.</p>"));

    DHBox* const hbox13    = new DHBox(d->settingsTab);
    new QLabel(i18n("Selection model: "), hbox13);
    QWidget* const space9  = new QWidget(hbox13);
    hbox13->setStretchFactor(space9, 10);

    d->modelSelectionMode  = new QComboBox(hbox13);
    d->modelSelectionMode->addItem(i18n("YOLOv5 Nano"),   AutoTagsScanSettings::YOLOV5NANO);
    d->modelSelectionMode->addItem(i18n("YOLOv5 XLarge"), AutoTagsScanSettings::YOLOV5XLARGE);
    d->modelSelectionMode->addItem(i18n("ResNet50"),      AutoTagsScanSettings::RESNET50);
    d->modelSelectionMode->setToolTip(i18nc("@info:tooltip",
        "<p><b>YOLOv5 Nano</b>: this model is a neural network which offers exceptional speed and efficiency. It enables you to swiftly "
        "evaluate the integration of smaller-scale object detection scenarios. It's designed for objects detections, capable of recognizing "
        "and extracting the location of objects within an image. The limitation on the number of recognizable objects is set to 80.</p>"
        "<p><b>YOLOv5 XLarge</b>: as the previous one, this model is a neural network dedicated for more complex object detection requirements and "
        "showcases remarkable capabilities. Despite the additional complexity introducing more time-latency and "
        "computer resources, it must be used for larger-scale object detection scenarios as it provides more accurate predictions at the expense of speed.</p>"
        "<p><b>ResNet50</b>: this model is a specific type of convolutional neural network formed by stacking residual blocks "
        "commonly used to power computer vision applications as object detections. This king of design allows the training of very deep networks without "
        "encountering the vanishing gradient problem. Unlike YOLO, ResNet50 is primarily focused on image classification and does not provide object localization. "
        "It can recognize objects from a vast set of more than 1,000 classes, covering a wide range of objects, animals, and scenes.</p>"));

    settingsLayout->addWidget(title);
    settingsLayout->addWidget(hbox12);
    settingsLayout->addWidget(hbox13);

    addTab(d->settingsTab, i18nc("@title:tab", "Settings"));

    // --- Translate tab -----------------------------------------------------------------------------------

    d->trSelectorList      = new LocalizeSelectorList(d->settingsTab);
    d->trSelectorList->setTitle(i18nc("@label", "Translate Tags to:"));

    addTab(d->trSelectorList, i18nc("@title:tab", "Translate"));
}

AutoTagsScanSettings AutoTagsScanWidget::settings() const
{

    AutoTagsScanSettings settings;
/*
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

    settings.albums                 = d->albumSelectors->selectedAlbumsAndTags();
    settings.wholeAlbums            = d->albumSelectors->wholeAlbumsChecked();

    if (d->settingsConflicted)
    {
        int numberOfIdentities      = FaceDbAccess().db()->getNumberOfIdentities();
        d->settingsConflicted       = (numberOfIdentities == 0);
    }

    settings.detectAccuracy         = d->detectAccuracyInput->value();
    settings.detectModel            = static_cast<FaceScanSettings::FaceDetectionModel>(d->detectModelBox->currentData().toInt());
    settings.detectSize             = static_cast<FaceScanSettings::FaceDetectionSize>(d->detectSizeBox->currentData().toInt());
    settings.recognizeAccuracy      = d->recognizeAccuracyInput->value();
    settings.recognizeModel         = static_cast<FaceScanSettings::FaceRecognitionModel>(d->recognizeModelBox->currentData().toInt());

    settings.useFullCpu             = d->useFullCpuButton->isChecked();
*/
    return settings;
}

} // namespace Digikam

#include "moc_autotagsscanwidget.cpp"
