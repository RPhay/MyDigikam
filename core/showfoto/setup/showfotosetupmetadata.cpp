/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-18
 * Description : setup Metadata tab.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "showfotosetupmetadata.h"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QApplication>
#include <QStyle>
#include <QStandardPaths>
#include <QFontDatabase>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "metaengine.h"
#include "metadatapanel.h"
#include "metaenginesettings.h"
#include "dactivelabel.h"
#include "exiftoolbinary.h"
#include "dbinarysearch.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN SetupMetadata::Private
{
public:

    explicit Private()
      : exifRotateBox        (nullptr),
        exifSetOrientationBox(nullptr),
        tab                  (nullptr),
        tagsCfgPanel         (nullptr),
        exifToolBinWidget    (nullptr)
    {
    }

    QCheckBox*              exifRotateBox;
    QCheckBox*              exifSetOrientationBox;

    QTabWidget*             tab;

    Digikam::MetadataPanel* tagsCfgPanel;
    Digikam::DBinarySearch* exifToolBinWidget;
    Digikam::ExifToolBinary exifToolBin;
};

SetupMetadata::SetupMetadata(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    d->tab                        = new QTabWidget(viewport());
    setWidget(d->tab);
    setWidgetResizable(true);

    const int spacing             = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QWidget* const panel          = new QWidget(d->tab);
    QVBoxLayout* const mainLayout = new QVBoxLayout(panel);

    // --------------------------------------------------------

    QGroupBox* const   rotationAdvGroup  = new QGroupBox(panel);
    QGridLayout* const rotationAdvLayout = new QGridLayout(rotationAdvGroup);

    QLabel* const rotationAdvExpl  = new QLabel(i18nc("@label", "Rotate actions"));
    QLabel* const rotationAdvIcon  = new QLabel;
    rotationAdvIcon->setPixmap(QIcon::fromTheme(QLatin1String("configure")).pixmap(32));

    d->exifRotateBox         = new QCheckBox(rotationAdvGroup);
    d->exifRotateBox->setText(i18nc("@option:check", "Show images/thumbnails &rotated according to orientation tag."));
    d->exifSetOrientationBox = new QCheckBox(rotationAdvGroup);
    d->exifSetOrientationBox->setText(i18nc("@option:check", "Set orientation tag to normal after rotate/flip."));

    rotationAdvLayout->addWidget(rotationAdvIcon,          0, 0, 1, 1);
    rotationAdvLayout->addWidget(rotationAdvExpl,          0, 1, 1, 1);
    rotationAdvLayout->addWidget(d->exifRotateBox,         1, 0, 1, 3);
    rotationAdvLayout->addWidget(d->exifSetOrientationBox, 2, 0, 1, 3);
    rotationAdvLayout->setColumnStretch(2, 10);
    rotationAdvGroup->setLayout(rotationAdvLayout);

    // --------------------------------------------------------

    QFrame* const box       = new QFrame(panel);
    QGridLayout* const grid = new QGridLayout(box);
    box->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    Digikam::DActiveLabel* const exiv2LogoLabel = new Digikam::DActiveLabel(QUrl(QLatin1String("https://www.exiv2.org")),
                                                                            QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                                                                   QLatin1String("digikam/data/logo-exiv2.png")),
                                                                            box);
    exiv2LogoLabel->setWhatsThis(i18nc("@info:whatsthis", "Visit Exiv2 project website"));

    QLabel* const explanation = new QLabel(box);
    explanation->setOpenExternalLinks(true);
    explanation->setWordWrap(true);
    QString txt;

    txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/Exif'>Exif</a> - %1</p>")
               .arg(i18nc("@info", "a standard used by most digital cameras today to store technical "
                          "information (like aperture and shutter speed) about an image.")));

    txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/IPTC_Information_Interchange_Model'>IPTC</a> - %1</p>")
               .arg(i18nc("@info", "an older standard used in digital photography to store "
                          "photographer information in images.")));

    if (MetaEngine::supportXmp())
    {
        txt.append(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/Extensible_Metadata_Platform'>XMP</a> - %1</p>")
                   .arg(i18nc("@info", "a new standard used in digital photography, designed to replace IPTC.")));
    }

    explanation->setText(txt);
    explanation->setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));

    grid->addWidget(exiv2LogoLabel, 0, 0, 1, 1);
    grid->addWidget(explanation,    0, 1, 1, 2);
    grid->setColumnStretch(1, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(0);

    // --------------------------------------------------------

    mainLayout->setContentsMargins(QMargins());
    mainLayout->setSpacing(spacing);
    mainLayout->addWidget(rotationAdvGroup);
    mainLayout->addSpacing(spacing);
    mainLayout->addWidget(box);
    mainLayout->addStretch();

    d->tab->insertTab(Behavior, panel, i18nc("@title:tab", "Behavior"));

    // --------------------------------------------------------

    d->tagsCfgPanel = new Digikam::MetadataPanel(d->tab);

    // --------------------------------------------------------

    QWidget* const exifToolPanel      = new QWidget(d->tab);
    QVBoxLayout* const exifToolLayout = new QVBoxLayout;
    QLabel* const exifToolBinLabel    = new QLabel(i18nc("@info",
                                                   "ExifTool is an open-source software program for reading, writing, "
                                                   "and manipulating multimedia files. It is platform independent "
                                                   "available as a command-line application. ExifTool is commonly "
                                                   "incorporated into different types of digital workflows and supports "
                                                   "many types of metadata including Exif, IPTC, XMP, JFIF, GeoTIFF, ICC Profile, "
                                                   "Photoshop IRB, as well as the manufacturer-specific metadata formats of "
                                                   "many digital cameras.\n\n"
                                                   "Here you can configure location where ExifTool binary is located. "
                                                   "Application will try to find this binary automatically if they are "
                                                   "already installed on your computer."),
                                                   exifToolPanel);
    exifToolBinLabel->setWordWrap(true);

    d->exifToolBinWidget              = new Digikam::DBinarySearch(exifToolPanel);
    d->exifToolBinWidget->addBinary(d->exifToolBin);

    foreach (const QString& path, MetaEngineSettings::instance()->settings().defaultExifToolSearchPaths())
    {
        d->exifToolBinWidget->addDirectory(path);
    }

    d->exifToolBinWidget->allBinariesFound();

    exifToolLayout->addWidget(exifToolBinLabel);
    exifToolLayout->addWidget(d->exifToolBinWidget);
    exifToolLayout->addStretch();
    exifToolPanel->setLayout(exifToolLayout);

    d->tab->insertTab(ExifTool, exifToolPanel, i18nc("@title:tab", "ExifTool"));

    // --------------------------------------------------------

    readSettings();
}

SetupMetadata::~SetupMetadata()
{
    delete d;
}

void SetupMetadata::applySettings()
{
    Digikam::MetaEngineSettings* const mSettings = Digikam::MetaEngineSettings::instance();

    if (!mSettings)
    {
        return;
    }

    Digikam::MetaEngineSettingsContainer set;

    set.exifRotate         = d->exifRotateBox->isChecked();
    set.exifSetOrientation = d->exifSetOrientationBox->isChecked();
    set.exifToolPath       = d->exifToolBin.directory();
    mSettings->setSettings(set);

    d->tagsCfgPanel->applySettings();
}

void SetupMetadata::readSettings()
{
    Digikam::MetaEngineSettings* const mSettings = Digikam::MetaEngineSettings::instance();

    if (!mSettings)
    {
        return;
    }

    Digikam::MetaEngineSettingsContainer set     = mSettings->settings();

    d->exifRotateBox->setChecked(set.exifRotate);
    d->exifSetOrientationBox->setChecked(set.exifSetOrientation);
    d->exifToolBin.setup(set.exifToolPath);
}

void SetupMetadata::setActiveTab(MetadataTab tab)
{
    d->tab->setCurrentIndex(tab);
}

} // namespace ShowFoto
