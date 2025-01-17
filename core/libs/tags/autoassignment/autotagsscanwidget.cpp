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

AutotagsScanWidget::AutotagsScanWidget(SettingsDisplayMode _displayMode, QWidget* const parent)
    : QTabWidget       (parent),
      StateSavingObject(this),
      d                (new Private)
{
    d->displayMode = _displayMode;
    setObjectName(d->configName);
    setupUi();
}

AutotagsScanWidget::~AutotagsScanWidget()
{
    delete d;
}

void AutotagsScanWidget::doLoadState()
{
    KConfigGroup group = getConfigGroup();
    d->albumSelectors->loadState();

    AutotagsScanSettings prm;

    int tagScanMode             = d->scanMode->findData(group.readEntry(d->configScanMode, (int)prm.scanMode));
    d->scanMode->setCurrentIndex(tagScanMode);

    int tagTagMode              = d->tagMode->findData(group.readEntry(d->configTagMode, (int)prm.tagMode));
    d->tagMode->setCurrentIndex(tagTagMode);

    int objectDetectModel       = d->objectDetectModel->findData(group.readEntry(d->configObjectDetectModel,   (int)prm.objectDetectModel));
    d->objectDetectModel->setCurrentIndex(objectDetectModel);

    d->accuracyInput->setValue(group.readEntry(d->configObjectDetectAccuracy, prm.uiConfidenceThreshold));

    d->useFullCpuButton->setChecked(group.readEntry(d->configObjectDetectAccuracy, prm.useFullCpu));

    d->trSelectorList->clearLanguages();
    const auto lgs     = group.readEntry(d->configLanguages,                                    prm.languages);

    for (const QString& lg : lgs)
    {
        d->trSelectorList->addLanguage(lg);
    }
}

void AutotagsScanWidget::doSaveState()
{
    KConfigGroup group       = getConfigGroup();
    AutotagsScanSettings prm = settings();

    d->albumSelectors->saveState();

    group.writeEntry(d->configScanMode,             (int)prm.scanMode);
    group.writeEntry(d->configTagMode,              (int)prm.tagMode);
    group.writeEntry(d->configObjectDetectModel,    (int)prm.objectDetectModel);
    group.writeEntry(d->configObjectDetectAccuracy, (int)prm.uiConfidenceThreshold);
    group.writeEntry(d->configLanguages,            prm.languages);
}

void AutotagsScanWidget::setupUi()
{
    // --- Album tab --------------------------------------------------------------------------------------

    d->albumSelectors                   = new AlbumSelectors(QString(), d->configName,
                                                             this, AlbumSelectors::AlbumType::All, true);
    addTab(d->albumSelectors, i18nc("@title:tab", "Search in"));

    // --- Settings tab -----------------------------------------------------------------------------------

    d->settingsTab                      = new QWidget(this);
    QVBoxLayout* const settingsLayout   = new QVBoxLayout(d->settingsTab);

    QLabel* const title         = new QLabel(d->settingsTab);
    title->setText(i18nc("@label",
                         "<p><b>This tool automatically assigns tags to images by analyzing the image using "
                         "a deep-learning neural network AI model.</b></p>"
                         "<p>The settings below determine the deep-learning AI model to use while parsing image "
                         "contents to determine objects in the image. The AI neural network used in background "
                         "will automatically generate tags describing the contents and store the results in "
                         "the database.</p>"));
    title->setWordWrap(true);

    DHBox* const hbox12         = new DHBox(d->settingsTab);
    new QLabel (i18n("Auto-tagging mode: "), hbox12);
    QWidget* const space8       = new QWidget(hbox12);
    hbox12->setStretchFactor(space8, 10);

    d->scanMode                 = new QComboBox(hbox12);
    d->scanMode->addItem(i18n("Scan all"), AutotagsScanSettings::ScanMode::AllItems);
    d->scanMode->addItem(i18n("Scan non-assigned only"),  AutotagsScanSettings::ScanMode::NonAssignedItems);
    d->scanMode->setToolTip(i18nc("@info:tooltip",
        "<p><b>Scan all</b>: re-scan all items for tags.</p>"
        "<p><b>Scan non-assigned only</b>: scan only the items with no assigned autotags.</p>"));

    DHBox* const hbox15         = new DHBox(d->settingsTab);
    new QLabel (i18n("Auto-tagging tag mode: "), hbox15);
    QWidget* const space15      = new QWidget(hbox15);
    hbox15->setStretchFactor(space15, 10);

    d->tagMode                  = new QComboBox(hbox15);
    d->tagMode->addItem(i18n("Replace existing autotags"), AutotagsScanSettings::TagMode::Replace);
    d->tagMode->addItem(i18n("Update autotags"),  AutotagsScanSettings::TagMode::Update);
    d->tagMode->setToolTip(i18nc("@info:tooltip",
        "<p><b>Replace existing autotags</b>: clear existing autotags and replace with the results of the scan.</p>"
        "<p><b>Update autotags</b>: add any new autotags found to the existing tags.</p>"));

    DHBox* const hbox13         = new DHBox(d->settingsTab);
    new QLabel(i18n("Selection model: "), hbox13);
    QWidget* const space9       = new QWidget(hbox13);
    hbox13->setStretchFactor(space9, 10);

    d->objectDetectModel        = new QComboBox(hbox13);
    d->objectDetectModel->addItem(i18n("YOLOv11 Nano"),   AutotagsScanSettings::ObjectDetectionModel::YOLOV11NANO);
    d->objectDetectModel->addItem(i18n("YOLOv11 XLarge"), AutotagsScanSettings::ObjectDetectionModel::YOLOV11XLARGE);
    d->objectDetectModel->addItem(i18n("ResNet-152"),      AutotagsScanSettings::ObjectDetectionModel::RESNET152);
    d->objectDetectModel->setToolTip(i18nc("@info:tooltip",
        "<p><b>YOLOv11 Nano</b>: small, lightweight neural network offering exceptional speed, but may miss identifying more objects in images. "
        "YOLO can detect multiple objects in an image. It is trained to recognize 80 different objects using the COCO dataset.</p>"
        "<p><b>YOLOv11 XLarge</b>: large, robust neural network offering good accuracy. It will detect more objects in images than YOLOv11 Nano, "
        "but is slower. YOLO can detect multiple objects in an image. It is trained to recognize 80 different objects using the COCO dataset.</p>"
        "<p><b>ResNet-152</b>: large and powerful convoluted neural network. It will detect a single object in an image with high accuracy. "
        "ResNet-152 was trained to recognize 1,000 different objects using the ImageNet dataset.</p>"));

    DHBox* const hbox14         = new DHBox(d->settingsTab);
    new QLabel(i18n("Object detection accuracy: "), hbox14);
    d->accuracyInput            = new DIntNumInput(hbox14);
    d->accuracyInput->setDefaultValue(7);
    d->accuracyInput->setRange(1, 10, 1);
    d->accuracyInput->setToolTip(i18nc("@info:tooltip",
                                                "Adjust sensitivity versus specificity: the higher the value, the more accurately objects will\n"
                                                "be recognized, but fewer objects will be recognized.\n"));

    DHBox* const hbox16         = new DHBox(d->settingsTab);
    d->useFullCpuButton         = new QCheckBox(hbox16);
    d->useFullCpuButton->setText(i18nc("@option:check", "Work on all processor cores"));
    d->useFullCpuButton->setToolTip(i18nc("@info:tooltip",
                                          "Object detection and auto-tagging are time-consuming tasks.\n"
                                          "You can choose if you wish to employ all processor cores\n"
                                          "on your system, or work in the background only on one core."));

    settingsLayout->addWidget(title);
    settingsLayout->addWidget(hbox12);
    settingsLayout->addWidget(hbox15);
    settingsLayout->addWidget(hbox14);
    settingsLayout->addWidget(hbox13);
    settingsLayout->addWidget(hbox16);

    addTab(d->settingsTab, i18nc("@title:tab", "Settings"));

    // --- Translate tab -----------------------------------------------------------------------------------

    d->trSelectorList           = new LocalizeSelectorList(d->settingsTab);
    d->trSelectorList->setTitle(i18nc("@label", "Translate Tags to:"));

    addTab(d->trSelectorList, i18nc("@title:tab", "Translate"));

    // --- Configure UI -----------------------------------------------------------------------------------

    if (SettingsDisplayMode::BQM == d->displayMode)
    {
        hbox12->hide();
        this->setTabVisible(0, false);
        this->setCurrentIndex(1);
    }

    if (SettingsDisplayMode::Maintenance == d->displayMode)
    {
        this->setTabVisible(0, false);
        this->setCurrentIndex(1);
    }

    if (SettingsDisplayMode::Normal == d->displayMode)
    {
        title->hide();
    }

    // --- Signals -----------------------------------------------------------------------------------

    connect(d->albumSelectors, SIGNAL(signalSelectionChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->objectDetectModel, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->scanMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->tagMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->trSelectorList, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    connect(d->accuracyInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotSettingsChanged()));
}

AutotagsScanSettings AutotagsScanWidget::settings() const
{

    AutotagsScanSettings result;

    if (SettingsDisplayMode::Normal == d->displayMode )
    {
        result.albums                 = d->albumSelectors->selectedAlbumsAndTags();
        result.wholeAlbums            = d->albumSelectors->wholeAlbumsChecked();
    }

    if (SettingsDisplayMode::BQM != d->displayMode )
    {
        result.scanMode               = (AutotagsScanSettings::ScanMode)d->scanMode->itemData(d->scanMode->currentIndex()).toInt();
    }

    result.tagMode                = (AutotagsScanSettings::TagMode)d->tagMode->itemData(d->tagMode->currentIndex()).toInt();
    result.objectDetectModel      = (AutotagsScanSettings::ObjectDetectionModel)d->objectDetectModel->itemData(d->objectDetectModel->currentIndex()).toInt();
    result.uiConfidenceThreshold  = d->accuracyInput->value();
    result.languages              = d->trSelectorList->languagesList();

    return result;
}

void AutotagsScanWidget::settings(const AutotagsScanSettings& newSettings)
{
    // d->albumSelectors->setAlbumSelected(newSettings.albums);
    // d->albumSelectors->set

    // result.albums                 = d->albumSelectors->selectedAlbumsAndTags();
    // result.wholeAlbums            = d->albumSelectors->wholeAlbumsChecked();

    d->scanMode->setCurrentIndex(d->scanMode->findData(newSettings.scanMode));
    d->tagMode->setCurrentIndex(d->tagMode->findData(newSettings.tagMode));
    d->objectDetectModel->setCurrentIndex(d->objectDetectModel->findData(newSettings.objectDetectModel));

    if (d->accuracyInput->value() != newSettings.uiConfidenceThreshold)
    {
        d->accuracyInput->setValue(newSettings.uiConfidenceThreshold);
    }

    d->trSelectorList->clearLanguages();

    for (const QString& lg : newSettings.languages)
    {
        d->trSelectorList->addLanguage(lg);
    }
}

void AutotagsScanWidget::slotSettingsChanged()
{
    Q_EMIT signalSettingsChanged();
}

} // namespace Digikam

#include "moc_autotagsscanwidget.cpp"
