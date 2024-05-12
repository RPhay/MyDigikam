/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-30
 * Description : maintenance dialog
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "maintenancedlg_p.h"

namespace Digikam
{

MaintenanceDlg::MaintenanceDlg(QWidget* const parent)
    : QDialog(parent),
      d      (new Private)
{
    setWindowFlags((windowFlags() & ~Qt::Dialog) |
                   Qt::Window                    |
                   Qt::WindowCloseButtonHint     |
                   Qt::WindowMinMaxButtonsHint);

    setWindowTitle(i18nc("@title:window", "Maintenance"));
    setModal(true);

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    d->buttons->button(QDialogButtonBox::Cancel)->setDefault(true);

    QScrollArea* const main    = new QScrollArea(this);
    QWidget* const page        = new QWidget(main->viewport());
    main->setWidget(page);
    main->setWidgetResizable(true);

    QGridLayout* const grid    = new QGridLayout(page);

    d->logo                    = new QLabel(page);
    d->logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(48,48)));
    d->title                   = new QLabel(i18n("<qt><b>Select Maintenance Operations to Process</b></qt>"), page);
    d->expanderBox             = new DExpanderBox(page);

    // --------------------------------------------------------------------------------------

    DVBox* const options       = new DVBox;
    d->albumSelectors          = new AlbumSelectors(i18nc("@label", "Process items from:"),
                                                    d->configGroupName, options, AlbumSelectors::AlbumType::All, true);
    d->useLastSettings         = new QCheckBox(i18nc("@option:check", "Use the last saved active tools and settings"), options);
    d->useMutiCoreCPU          = new QCheckBox(i18nc("@option:check", "Work on all processor cores (when it's possible)"), options);
    d->expanderBox->insertItem(Private::Options, options, QIcon::fromTheme(QLatin1String("configure")), i18n("Common Options"), QLatin1String("Options"), true);

    // --------------------------------------------------------------------------------------

    d->expanderBox->insertItem(Private::NewItems, new QLabel(i18n("<qt>No option<br/>"
                               "<i>Note: only Albums Collection are processed by this tool.</i></qt>")),
                               QIcon::fromTheme(QLatin1String("view-refresh")), i18n("Scan for new items"), QLatin1String("NewItems"), false);
    d->expanderBox->setCheckBoxVisible(Private::NewItems, true);

    // --------------------------------------------------------------------------------------

    d->vbox3                   = new DVBox;
    new QLabel(i18n("<qt><i>Note: If activated, the Core DB is always cleaned. You can select additional databases for cleaning.<br/>"
                    " If you select one of the options below, the process may take much time and can freeze digiKam temporarily<br/>"
                    " in order to make sure that no database corruption occurs.</i></qt>"), d->vbox3);
    d->cleanThumbsDb           = new QCheckBox(i18n("Also clean up the thumbnail database."), d->vbox3);
    d->cleanFacesDb            = new QCheckBox(i18n("Also clean up the faces database."), d->vbox3);
    d->cleanSimilarityDb       = new QCheckBox(i18n("Also clean up the similarity database."), d->vbox3);
    d->shrinkDatabases         = new QCheckBox(i18n("Extended clean up and shrink all databases."), d->vbox3);
    d->shrinkDatabases->setToolTip(i18n("This option leads to the vacuuming (shrinking) of the databases. "
                                        "Vacuuming is supported both for SQLite and MySQL."));
    d->expanderBox->insertItem(Private::DbCleanup, d->vbox3,
                               QIcon::fromTheme(QLatin1String("run-build")),
                               i18n("Perform database cleaning"), QLatin1String("DbCleanup"), false);
    d->expanderBox->setCheckBoxVisible(Private::DbCleanup, true);

    // --------------------------------------------------------------------------------------

    d->scanThumbs              = new QCheckBox(i18n("Scan for changed or non-cataloged items (faster)"));
    d->expanderBox->insertItem(Private::Thumbnails, d->scanThumbs, QIcon::fromTheme(QLatin1String("view-process-all")),
                               i18n("Rebuild Thumbnails"), QLatin1String("Thumbnails"), false);
    d->expanderBox->setCheckBoxVisible(Private::Thumbnails, true);

    // --------------------------------------------------------------------------------------

    d->scanFingerPrints        = new QCheckBox(i18n("Scan for changed or non-cataloged items (faster)"));
    d->expanderBox->insertItem(Private::FingerPrints, d->scanFingerPrints, QIcon::fromTheme(QLatin1String("run-build")),
                               i18n("Rebuild Finger-prints"), QLatin1String("Fingerprints"), false);
    d->expanderBox->setCheckBoxVisible(Private::FingerPrints, true);

    // --------------------------------------------------------------------------------------

    const ApplicationSettings* const settings = ApplicationSettings::instance();

    d->duplicatesBox                          = new DVBox;
    d->similarityRangeBox                     = new DHBox(d->duplicatesBox);
    new QLabel(i18n("Similarity range (in percents): "), d->similarityRangeBox);
    QWidget* const space                      = new QWidget(d->similarityRangeBox);
    d->similarityRangeBox->setStretchFactor(space, 10);

    d->similarityRange                        = new DIntRangeBox(d->similarityRangeBox);
    d->similarityRange->setSuffix(QLatin1String("%"));

    if (settings)
    {
        d->similarityRange->setRange(settings->getMinimumSimilarityBound(), 100);
        d->similarityRange->setInterval(settings->getDuplicatesSearchLastMinSimilarity(),
                                        settings->getDuplicatesSearchLastMaxSimilarity());
    }
    else
    {
        d->similarityRange->setRange(40, 100);
        d->similarityRange->setInterval(90, 100);
    }

    d->dupeRestrictionBox      = new DHBox(d->duplicatesBox);
    new QLabel(i18n("Restriction on duplicates:"), d->dupeRestrictionBox);
    QWidget* const space4      = new QWidget(d->dupeRestrictionBox);
    d->dupeRestrictionBox->setStretchFactor(space4, 10);
    d->searchResultRestriction = new QComboBox(d->dupeRestrictionBox);
    d->searchResultRestriction->addItem(i18n("No restriction"),                       HaarIface::DuplicatesSearchRestrictions::None);
    d->searchResultRestriction->addItem(i18n("Restrict to album of reference image"), HaarIface::DuplicatesSearchRestrictions::SameAlbum);
    d->searchResultRestriction->addItem(i18n("Exclude album of reference image"),     HaarIface::DuplicatesSearchRestrictions::DifferentAlbum);

    // Load the last choice from application settings.

    HaarIface::DuplicatesSearchRestrictions restrictions = HaarIface::DuplicatesSearchRestrictions::None;

    if (settings)
    {
        restrictions = (HaarIface::DuplicatesSearchRestrictions) settings->getDuplicatesSearchRestrictions();
    }

    d->searchResultRestriction->setCurrentIndex(d->searchResultRestriction->findData(restrictions));

    d->expanderBox->insertItem(Private::Duplicates, d->duplicatesBox, QIcon::fromTheme(QLatin1String("tools-wizard")),
                               i18n("Find Duplicate Items"), QLatin1String("Duplicates"), false);
    d->expanderBox->setCheckBoxVisible(Private::Duplicates, true);

    // --------------------------------------------------------------------------------------

    d->vbox4               = new DVBox;
    DHBox* const hbox3     = new DHBox(d->vbox4);
    new QLabel(i18n("Faces data management: "), hbox3);
    QWidget* const space3  = new QWidget(hbox3);
    hbox3->setStretchFactor(space3, 10);
    d->faceScannedHandling = new QComboBox(hbox3);
    d->faceScannedHandling->addItem(i18nc("@label:listbox", "Skip images already scanned"),           FaceScanSettings::Skip);
    d->faceScannedHandling->addItem(i18nc("@label:listbox", "Scan again and merge results"),          FaceScanSettings::Merge);
    d->faceScannedHandling->addItem(i18nc("@label:listbox", "Clear unconfirmed results and rescan"),  FaceScanSettings::Rescan);
    d->faceScannedHandling->addItem(i18nc("@label:listbox", "Clear all previous results and rescan"), FaceScanSettings::ClearAll);

    d->retrainAllFaces    = new QCheckBox(d->vbox4);
    d->retrainAllFaces->setText(i18nc("@option:check", "Clear and rebuild all training data"));
    d->retrainAllFaces->setToolTip(i18nc("@info:tooltip",
                                         "This will clear all training data for recognition "
                                         "and rebuild it from all available faces."));
    d->expanderBox->insertItem(Private::FaceManagement, d->vbox4, QIcon::fromTheme(QLatin1String("edit-image-face-detect")),
                               i18n("Detect and recognize Faces"), QLatin1String("FaceManagement"), false);
    d->expanderBox->setCheckBoxVisible(Private::FaceManagement, true);

    // --------------------------------------------------------------------------------------

    d->vbox5               = new DVBox;
    QLabel* const title    = new QLabel(d->vbox5);
    title->setText(i18nc("@label",
                        "<p><b>This tool allows to assign automatically tags to images by contents analysis using "
                        "deep-learning neural network.</b></p>"
                        "<p>The settings below determines the deep-learning model to use while parsing image "
                        "contents to determine the subjects of the photography. The neural network used in background "
                        "will generate automatically a serie of tags describing the contents and store the results in "
                        "the database.</p>"));
    title->setWordWrap(true);

    DHBox* const hbox12    = new DHBox(d->vbox5);
    new QLabel (i18n("Auto-tagging mode: "), hbox12);
    QWidget* const space8  = new QWidget(hbox12);
    hbox12->setStretchFactor(space8, 10);

    d->autotaggingScanMode = new QComboBox(hbox12);
    d->autotaggingScanMode->addItem(i18n("Clean all and re-assign"),  AutotagsAssignment::AllItems);
    d->autotaggingScanMode->addItem(i18n("Scan non-assigned only"),   AutotagsAssignment::NonAssignedItems);
    d->autotaggingScanMode->setToolTip(i18nc("@info:tooltip",
        "<p><b>Clean all and re-assign</b>: clean all tags already assigned and re-scan all items from scratch.</p>"
        "<p><b>Scan non-assigned only</b>: scan only the items with no assigned tag.</p>"));

    DHBox* const hbox13    = new DHBox(d->vbox5);
    new QLabel(i18n("Selection model: "), hbox13);
    QWidget* const space9  = new QWidget(hbox13);
    hbox13->setStretchFactor(space9, 10);

    d->modelSelectionMode  = new QComboBox(hbox13);
    d->modelSelectionMode->addItem(i18n("YOLOv5 Nano"),   DetectorModel::YOLOV5NANO);
    d->modelSelectionMode->addItem(i18n("YOLOv5 XLarge"), DetectorModel::YOLOV5XLARGE);
    d->modelSelectionMode->addItem(i18n("ResNet50"),      DetectorModel::RESNET50);
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

    d->trSelectorList      = new LocalizeSelectorList(d->vbox5);
    d->trSelectorList->setTitle(i18nc("@label", "Translate Tags to:"));

    d->expanderBox->insertItem(Private::AutotagsAssignment, d->vbox5,
                               QIcon::fromTheme(QLatin1String("tag")), i18n("Auto-tags Assignment"),
                               QLatin1String("AutotagsAssignment"), false);
    d->expanderBox->setCheckBoxVisible(Private::AutotagsAssignment, true);

    // --------------------------------------------------------------------------------------

    d->vbox               = new DVBox;
    DHBox* const hbox11   = new DHBox(d->vbox);
    new QLabel(i18n("Scan Mode: "), hbox11);
    QWidget* const space7 = new QWidget(hbox11);
    hbox11->setStretchFactor(space7, 10);

    d->qualityScanMode    = new QComboBox(hbox11);
    d->qualityScanMode->addItem(i18n("Clean all and re-scan"),  ImageQualitySorter::AllItems);
    d->qualityScanMode->addItem(i18n("Scan non-assigned only"), ImageQualitySorter::NonAssignedItems);

    d->qualitySelector    = new ImageQualityConfSelector(d->vbox);

    d->expanderBox->insertItem(Private::ImageQualitySorter, d->vbox, QIcon::fromTheme(QLatin1String("flag-green")),
                               i18n("Image Quality Sorter"), QLatin1String("ImageQualitySorter"), false);
    d->expanderBox->setCheckBoxVisible(Private::ImageQualitySorter, true);

    // --------------------------------------------------------------------------------------

    d->vbox2              = new DVBox;
    DHBox* const hbox21   = new DHBox(d->vbox2);
    new QLabel(i18n("Sync Direction: "), hbox21);
    QWidget* const space5 = new QWidget(hbox21);
    hbox21->setStretchFactor(space5, 10);
    d->syncDirection      = new QComboBox(hbox21);
    d->syncDirection->addItem(i18n("From database to image metadata"), MetadataSynchronizer::WriteFromDatabaseToFile);
    d->syncDirection->addItem(i18n("From image metadata to database"), MetadataSynchronizer::ReadFromFileToDatabase);

    DHBox* const hbox22   = new DHBox(d->vbox2);
    new QLabel(i18n("Check metadata setup panel for details: "), hbox22);
    QWidget* const space6 = new QWidget(hbox22);
    hbox22->setStretchFactor(space6, 10);
    d->metadataSetup      = new QPushButton(i18n("Settings..."), hbox22);
    d->expanderBox->insertItem(Private::MetadataSync, d->vbox2, QIcon::fromTheme(QLatin1String("run-build-file")),
                               i18n("Sync Metadata and Database"), QLatin1String("MetadataSync"), false);
    d->expanderBox->setCheckBoxVisible(Private::MetadataSync, true);

    d->expanderBox->insertStretch(Private::Stretch);

    // --------------------------------------------------------------------------------------

    grid->addWidget(d->logo,        0, 0, 1, 1);
    grid->addWidget(d->title,       0, 1, 1, 1);
    grid->addWidget(d->expanderBox, 5, 0, 3, 2);
    grid->setSpacing(layoutSpacing());
    grid->setContentsMargins(QMargins());
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(5, 10);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(main);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    // --------------------------------------------------------------------------------------

    connect(d->buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotOk()));

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(reject()));

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

    connect(d->expanderBox, SIGNAL(signalItemToggled(int,bool)),
            this, SLOT(slotItemToggled(int,bool)));

    connect(d->useLastSettings, SIGNAL(toggled(bool)),
            this, SLOT(slotUseLastSettings(bool)));

    connect(d->metadataSetup, SIGNAL(clicked()),
            this, SLOT(slotMetadataSetup()));

    connect(d->qualitySelector, SIGNAL(signalQualitySetup()),
            this, SLOT(slotQualitySetup()));

    connect(d->retrainAllFaces, &QCheckBox::toggled,
            this, [hbox3](bool on)
        {
            hbox3->setEnabled(!on);
        }
    );

    // --------------------------------------------------------------------------------------

    readSettings();
}

MaintenanceDlg::~MaintenanceDlg()
{
    delete d;
}

void MaintenanceDlg::slotOk()
{
    writeSettings();
    accept();
}

void MaintenanceDlg::slotItemToggled(int index, bool b)
{
    switch (index)
    {
        case Private::Thumbnails:
        {
            d->scanThumbs->setEnabled(b);
            break;
        }

        case Private::FingerPrints:
        {
            d->scanFingerPrints->setEnabled(b);
            break;
        }

        case Private::Duplicates:
        {
            d->duplicatesBox->setEnabled(b);
            break;
        }

        case Private::FaceManagement:
        {
            d->vbox4->setEnabled(b);
            break;
        }

        case Private::ImageQualitySorter:
        {
            d->vbox->setEnabled(b);
            break;
        }

        case Private::AutotagsAssignment:
        {
            d->vbox5->setEnabled(b);
            break;
        }

        case Private::MetadataSync:
        {
            d->vbox2->setEnabled(b);
            break;
        }

        case Private::DbCleanup:
        {
            d->vbox3->setEnabled(b);
            break;
        }

        default :  // NewItems
        {
            break;
        }
    }
}

void MaintenanceDlg::slotMetadataSetup()
{
    Setup::execSinglePage(this, Setup::MetadataPage);
}

void MaintenanceDlg::slotQualitySetup()
{
    Setup::execSinglePage(this, Setup::ImageQualityPage);
}

void MaintenanceDlg::slotHelp()
{
    openOnlineDocumentation(QLatin1String("maintenance_tools"));
}

} // namespace Digikam

#include "moc_maintenancedlg.cpp"
