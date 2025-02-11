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

// Local includes

#include "dnnmodelmanager.h"

namespace Digikam
{

AutotagsScanWidget::AutotagsScanWidget(SettingsDisplayMode displayMode, QWidget* const parent)
    : QTabWidget       (parent),
      StateSavingObject(this),
      d                (new Private)
{
    d->displayMode = displayMode;
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
    prm.readFromConfig(group);

    int tagScanMode             = d->scanMode->findData(prm.scanMode);
    d->scanMode->setCurrentIndex(tagScanMode);

    int tagTagMode              = d->tagMode->findData(prm.tagMode);
    d->tagMode->setCurrentIndex(tagTagMode);

    int objectDetectModel       = d->objectDetectModel->findData(prm.objectDetectModel);

    if (objectDetectModel < 0)
    {
        objectDetectModel = 0;
    }

    d->objectDetectModel->setCurrentIndex(objectDetectModel);

    d->accuracyInput->setValue(prm.uiConfidenceThreshold);

    d->useFullCpuButton->setChecked(prm.useFullCpu);

    d->trSelectorList->clearLanguages();

    for (const QString& lg : std::as_const(prm.languages))
    {
        d->trSelectorList->addLanguage(lg);
    }
}

void AutotagsScanWidget::doSaveState()
{
    KConfigGroup group       = getConfigGroup();
    d->albumSelectors->saveState();

    AutotagsScanSettings prm = settings();
    prm.writeToConfig(group);
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
                         "<p>The settings below configure the deep-learning AI model used for analyzing image "
                         "contents to identify objects. The AI neural network will automatically generate "
                         "descriptive tags for the image contents and store the results in the database.</p>"));
    title->setWordWrap(true);

    DHBox* const hbox12         = new DHBox(d->settingsTab);
    new QLabel(i18nc("@label: autotags scanning mode", "Scan Mode:"), hbox12);
    QWidget* const space8       = new QWidget(hbox12);
    hbox12->setStretchFactor(space8, 10);

    d->scanMode                 = new QComboBox(hbox12);
    d->scanMode->addItem(i18nc("@item: all items to scan with autotags",       "All"),               AutotagsScanSettings::ScanMode::AllItems);
    d->scanMode->addItem(i18nc("@item: autotags scan only non assigned items", "Non-Assigned Only"), AutotagsScanSettings::ScanMode::NonAssignedItems);
    d->scanMode->setToolTip(i18nc("@info:tooltip",
                                  "<p><b>All</b>: re-scan all items for tags.</p>"
                                  "<p><b>Non-Assigned Only</b>: scan only the items with no assigned autotags.</p>"));

    DHBox* const hbox15         = new DHBox(d->settingsTab);
    new QLabel (i18n("Auto-Tagging Mode:"), hbox15);
    QWidget* const space15      = new QWidget(hbox15);
    hbox15->setStretchFactor(space15, 10);

    d->tagMode                  = new QComboBox(hbox15);
    d->tagMode->addItem(i18nc("@item: remplace existing autotags entries in database", "Replace Existing"), AutotagsScanSettings::TagMode::Replace);
    d->tagMode->addItem(i18nc("@item: update the autotags entries in database",        "Update"),           AutotagsScanSettings::TagMode::Update);
    d->tagMode->setToolTip(i18nc("@info:tooltip",
                                 "<p><b>Replace Existing</b>: clear existing autotags and replace with the results of the scan.</p>"
                                 "<p><b>Update</b>: add any new autotags found to the existing tags.</p>"));

    DHBox* const hbox13         = new DHBox(d->settingsTab);
    new QLabel(i18nc("@label: deep learning model selection to perform autotags", "Selection Model:"), hbox13);
    QWidget* const space9       = new QWidget(hbox13);
    hbox13->setStretchFactor(space9, 10);

    DNNModelManager* const dnnModelManager        = DNNModelManager::instance();
    QList<QPair<QString, QStringList> > modelList = dnnModelManager->getModelList(DNNModelUsage::DNNUsageObjectDetection);
    d->objectDetectModel                          = new QComboBox(hbox13);

    if (modelList.isEmpty())
    {
        d->objectDetectModel->addItem(i18n("No models available"), QStringLiteral("none"));
        this->parentWidget()->setEnabled(false);
    }
    else
    {
        for (const auto& model : modelList)
        {
            d->objectDetectModel->addItem(model.second[0], model.first);
            d->objectDetectModel->setToolTip(d->objectDetectModel->toolTip() + model.second[1]);
        }
    }

    DHBox* const hbox14         = new DHBox(d->settingsTab);
    new QLabel(i18n("Detection Accuracy:"), hbox14);
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
        hbox16->hide();
        hbox12->hide();
        setTabVisible(0, false);
        setCurrentIndex(1);
    }

    if (SettingsDisplayMode::Maintenance == d->displayMode)
    {
        hbox16->hide();
        setTabVisible(0, false);
        setCurrentIndex(1);
    }

    if (SettingsDisplayMode::Normal == d->displayMode)
    {
        title->hide();
    }

    // --- Signals -----------------------------------------------------------------------------------

    connect(d->albumSelectors, SIGNAL(signalSelectionChanged()),
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

    if (SettingsDisplayMode::Normal == d->displayMode)
    {
        result.albums             = d->albumSelectors->selectedAlbumsAndTags();
        result.wholeAlbums        = d->albumSelectors->wholeAlbumsChecked();
    }

    if (SettingsDisplayMode::BQM != d->displayMode)
    {
        result.scanMode           = (AutotagsScanSettings::ScanMode)d->scanMode->itemData(d->scanMode->currentIndex()).toInt();
    }

    result.useFullCpu             = d->useFullCpuButton->isChecked();
    result.tagMode                = (AutotagsScanSettings::TagMode)d->tagMode->itemData(d->tagMode->currentIndex()).toInt();
    result.objectDetectModel      = d->objectDetectModel->itemData(d->objectDetectModel->currentIndex()).toString();
    result.uiConfidenceThreshold  = d->accuracyInput->value();
    result.languages              = d->trSelectorList->languagesList();

    return result;
}

void AutotagsScanWidget::settings(const AutotagsScanSettings& newSettings)
{
    d->scanMode->setCurrentIndex(d->scanMode->findData(newSettings.scanMode));
    d->tagMode->setCurrentIndex(d->tagMode->findData(newSettings.tagMode));
    d->objectDetectModel->setCurrentIndex(d->objectDetectModel->findData(newSettings.objectDetectModel));
    d->useFullCpuButton->setChecked(newSettings.useFullCpu);

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
