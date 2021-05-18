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
#include "exiftoolconfpanel.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoSetupMetadata::Private
{
public:

    explicit Private()
      : exifRotateBox        (nullptr),
        exifSetOrientationBox(nullptr),
        tab                  (nullptr),
        tagsCfgPanel         (nullptr),
        exifToolView         (nullptr)
    {
    }

    QCheckBox*                  exifRotateBox;
    QCheckBox*                  exifSetOrientationBox;

    QTabWidget*                 tab;

    Digikam::MetadataPanel*     tagsCfgPanel;
    Digikam::ExifToolConfPanel* exifToolView;
};

ShowfotoSetupMetadata::ShowfotoSetupMetadata(QWidget* const parent)
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

    if (Digikam::MetaEngine::supportXmp())
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

    d->exifToolView = new Digikam::ExifToolConfPanel(d->tab);
    d->tab->insertTab(ExifTool, d->exifToolView, i18nc("@title:tab", "ExifTool"));

    // --------------------------------------------------------

    readSettings();
}

ShowfotoSetupMetadata::~ShowfotoSetupMetadata()
{
    delete d;
}

void ShowfotoSetupMetadata::applySettings()
{
    Digikam::MetaEngineSettings* const mSettings = Digikam::MetaEngineSettings::instance();

    if (!mSettings)
    {
        return;
    }

    Digikam::MetaEngineSettingsContainer set;

    set.exifRotate         = d->exifRotateBox->isChecked();
    set.exifSetOrientation = d->exifSetOrientationBox->isChecked();
    set.exifToolPath       = d->exifToolView->exifToolDirectory();
    mSettings->setSettings(set);

    d->tagsCfgPanel->applySettings();
}

void ShowfotoSetupMetadata::readSettings()
{
    Digikam::MetaEngineSettings* const mSettings = Digikam::MetaEngineSettings::instance();

    if (!mSettings)
    {
        return;
    }

    Digikam::MetaEngineSettingsContainer set     = mSettings->settings();

    d->exifRotateBox->setChecked(set.exifRotate);
    d->exifSetOrientationBox->setChecked(set.exifSetOrientation);
    d->exifToolView->setExifToolDirectory(set.exifToolPath);
}

void ShowfotoSetupMetadata::setActiveTab(MetadataTab tab)
{
    d->tab->setCurrentIndex(tab);
}

} // namespace ShowFoto
