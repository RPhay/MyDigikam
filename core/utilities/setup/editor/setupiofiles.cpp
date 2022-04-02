/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-23
 * Description : setup image editor output files settings.
 *
 * Copyright (C) 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "setupiofiles.h"

// Qt includes

#include <QCheckBox>
#include <QGroupBox>
#include <QVBoxLayout>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "dpluginloader.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupIOFiles::Private
{
public:

    explicit Private()
      : JPEGOptions             (nullptr),
        PNGOptions              (nullptr),
        TIFFOptions             (nullptr),

#ifdef HAVE_JASPER

        JPEG2000Options         (nullptr),

#endif // HAVE_JASPER

        PGFOptions              (nullptr),

#ifdef HAVE_X265

        HEIFOptions             (nullptr),

#endif // HAVE_X265

        JXLOptions              (nullptr),
        showImageSettingsDialog (nullptr)
    {
    }

    QWidget* createGroupBox(QWidget* const w) const
    {
        QGroupBox* const  box     = new QGroupBox;
        QVBoxLayout* const layout = new QVBoxLayout;
        layout->addWidget(w);
        box->setLayout(layout);

        return box;
    }

public:

    static const QString configGroupName;
    static const QString configJPEGCompressionEntry;
    static const QString configJPEGSubSamplingEntry;
    static const QString configPNGCompressionEntry;
    static const QString configTIFFCompressionEntry;
    static const QString configJPEG2000CompressionEntry;
    static const QString configJPEG2000LossLessEntry;
    static const QString configPGFCompressionEntry;
    static const QString configPGFLossLessEntry;
    static const QString configHEIFCompressionEntry;
    static const QString configHEIFLossLessEntry;
    static const QString configShowImageSettingsDialog;
    static const QString configJXLCompressionEntry;
    static const QString configJXLLossLessEntry;

    DImgLoaderSettings*  JPEGOptions;
    DImgLoaderSettings*  PNGOptions;
    DImgLoaderSettings*  TIFFOptions;

#ifdef HAVE_JASPER

    DImgLoaderSettings*  JPEG2000Options;

#endif // HAVE_JASPER

    DImgLoaderSettings*  PGFOptions;

#ifdef HAVE_X265

    DImgLoaderSettings*  HEIFOptions;

#endif // HAVE_X265

    DImgLoaderSettings*  JXLOptions;
    QCheckBox*           showImageSettingsDialog;
};

const QString SetupIOFiles::Private::configGroupName(QLatin1String("ImageViewer Settings"));
const QString SetupIOFiles::Private::configJPEGCompressionEntry(QLatin1String("JPEGCompression"));
const QString SetupIOFiles::Private::configJPEGSubSamplingEntry(QLatin1String("JPEGSubSampling"));
const QString SetupIOFiles::Private::configPNGCompressionEntry(QLatin1String("PNGCompression"));
const QString SetupIOFiles::Private::configTIFFCompressionEntry(QLatin1String("TIFFCompression"));
const QString SetupIOFiles::Private::configJPEG2000CompressionEntry(QLatin1String("JPEG2000Compression"));
const QString SetupIOFiles::Private::configJPEG2000LossLessEntry(QLatin1String("JPEG2000LossLess"));
const QString SetupIOFiles::Private::configPGFCompressionEntry(QLatin1String("PGFCompression"));
const QString SetupIOFiles::Private::configPGFLossLessEntry(QLatin1String("PGFLossLess"));
const QString SetupIOFiles::Private::configHEIFCompressionEntry(QLatin1String("HEIFCompression"));
const QString SetupIOFiles::Private::configHEIFLossLessEntry(QLatin1String("HEIFLossLess"));
const QString SetupIOFiles::Private::configShowImageSettingsDialog(QLatin1String("ShowImageSettingsDialog"));
const QString SetupIOFiles::Private::configJXLCompressionEntry(QLatin1String("JXLCompression"));
const QString SetupIOFiles::Private::configJXLLossLessEntry(QLatin1String("JXLLossLess"));

// --------------------------------------------------------

SetupIOFiles::SetupIOFiles(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    QWidget* const panel         = new QWidget;
    QVBoxLayout* const vbox      = new QVBoxLayout;
    DPluginLoader* const ploader = DPluginLoader::instance();
    
    d->JPEGOptions               = ploader->exportWidget(QLatin1String("JPEG"));
    d->JPEGOptions->setParent(this);
    d->PNGOptions                = ploader->exportWidget(QLatin1String("PNG"));
    d->PNGOptions->setParent(this);
    d->TIFFOptions               = ploader->exportWidget(QLatin1String("TIFF"));
    d->TIFFOptions->setParent(this);

#ifdef HAVE_JASPER

    d->JPEG2000Options           = ploader->exportWidget(QLatin1String("JP2"));
    d->JPEG2000Options->setParent(this);

#endif // HAVE_JASPER

    d->PGFOptions                = ploader->exportWidget(QLatin1String("PGF"));
    d->PGFOptions->setParent(this);

#ifdef HAVE_X265

    d->HEIFOptions               = ploader->exportWidget(QLatin1String("HEIF"));
    d->HEIFOptions->setParent(this);

#endif // HAVE_X265

    d->JXLOptions                = ploader->exportWidget(QLatin1String("JXL"));

    if (d->JXLOptions)
    {
        d->JXLOptions->setParent(this);
    }

    // Show Settings Dialog Option

    d->showImageSettingsDialog   = new QCheckBox(panel);
    d->showImageSettingsDialog->setText(i18n("Show Settings Dialog when Saving Image Files"));
    d->showImageSettingsDialog->setWhatsThis(i18n("<ul><li>Checked: A dialog where settings can be changed when saving image files</li>"
                                                  "<li>Unchecked: Default settings are used when saving image files</li></ul>"));

    vbox->addWidget(d->createGroupBox(d->JPEGOptions));
    vbox->addWidget(d->createGroupBox(d->PNGOptions));
    vbox->addWidget(d->createGroupBox(d->TIFFOptions));

#ifdef HAVE_JASPER

    vbox->addWidget(d->createGroupBox(d->JPEG2000Options));

#endif // HAVE_JASPER

    vbox->addWidget(d->createGroupBox(d->PGFOptions));

#ifdef HAVE_X265

    vbox->addWidget(d->createGroupBox(d->HEIFOptions));

#endif // HAVE_265

    if (d->JXLOptions)
    {
        vbox->addWidget(d->createGroupBox(d->JXLOptions));
    }

    vbox->addWidget(d->createGroupBox(d->showImageSettingsDialog));
    vbox->addStretch();

    panel->setLayout(vbox);
    setWidget(panel);
    setWidgetResizable(true);

    // --------------------------------------------------------

    readSettings();
}

SetupIOFiles::~SetupIOFiles()
{
    delete d;
}

void SetupIOFiles::applySettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    group.writeEntry(d->configJPEGCompressionEntry,     d->JPEGOptions->settings()[QLatin1String("quality")].toInt());
    group.writeEntry(d->configJPEGSubSamplingEntry,     d->JPEGOptions->settings()[QLatin1String("subsampling")].toInt());

    // ---

    group.writeEntry(d->configPNGCompressionEntry,      d->PNGOptions->settings()[QLatin1String("quality")].toInt());

    // ---

    group.writeEntry(d->configTIFFCompressionEntry,     d->TIFFOptions->settings()[QLatin1String("compress")].toBool());

#ifdef HAVE_JASPER

    group.writeEntry(d->configJPEG2000CompressionEntry, d->JPEG2000Options->settings()[QLatin1String("quality")].toInt());
    group.writeEntry(d->configJPEG2000LossLessEntry,    d->JPEG2000Options->settings()[QLatin1String("lossless")].toBool());

#endif // HAVE_JASPER

    group.writeEntry(d->configPGFCompressionEntry,      d->PGFOptions->settings()[QLatin1String("quality")].toInt());
    group.writeEntry(d->configPGFLossLessEntry,         d->PGFOptions->settings()[QLatin1String("lossless")].toBool());

#ifdef HAVE_X265

    group.writeEntry(d->configHEIFCompressionEntry,     d->HEIFOptions->settings()[QLatin1String("quality")].toInt());
    group.writeEntry(d->configHEIFLossLessEntry,        d->HEIFOptions->settings()[QLatin1String("lossless")].toBool());

#endif // HAVE_X265

    
    if (d->JXLOptions)
    {
        group.writeEntry(d->configJXLCompressionEntry,  d->JXLOptions->settings()[QLatin1String("quality")].toInt());
        group.writeEntry(d->configJXLLossLessEntry,     d->JXLOptions->settings()[QLatin1String("lossless")].toBool());
    }

    group.writeEntry(d->configShowImageSettingsDialog,  d->showImageSettingsDialog->isChecked());
    config->sync();
}

void SetupIOFiles::readSettings()
{
    DImgLoaderPrms set;

    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
   
    set.clear();
    set.insert(QLatin1String("quality"),     group.readEntry(d->configJPEGCompressionEntry,    75));
    set.insert(QLatin1String("subsampling"), group.readEntry(d->configJPEGSubSamplingEntry,    1));  // Medium sub-sampling
    d->JPEGOptions->setSettings(set);

    // ---

    set.clear();
    set.insert(QLatin1String("quality"),  group.readEntry(d->configPNGCompressionEntry,        9));
    d->PNGOptions->setSettings(set);

    // ---
    
    set.clear();
    set.insert(QLatin1String("compress"),  group.readEntry(d->configTIFFCompressionEntry,      false));
    d->TIFFOptions->setSettings(set);

#ifdef HAVE_JASPER

    set.clear();
    set.insert(QLatin1String("quality"),  group.readEntry(d->configJPEG2000CompressionEntry,   75));
    set.insert(QLatin1String("lossless"), group.readEntry(d->configJPEG2000LossLessEntry,      true));
    d->JPEG2000Options->setSettings(set);

#endif // HAVE_JASPER

    set.clear();
    set.insert(QLatin1String("quality"),  group.readEntry(d->configPGFCompressionEntry,        3));
    set.insert(QLatin1String("lossless"), group.readEntry(d->configPGFLossLessEntry,           true));
    d->PGFOptions->setSettings(set);

#ifdef HAVE_X265

    set.clear();
    set.insert(QLatin1String("quality"),  group.readEntry(d->configHEIFCompressionEntry,       75));
    set.insert(QLatin1String("lossless"), group.readEntry(d->configHEIFLossLessEntry,          true));
    d->HEIFOptions->setSettings(set);

#endif // HAVE_X265

    if (d->JXLOptions)
    {
        set.clear();
        set.insert(QLatin1String("quality"),  group.readEntry(d->configJXLCompressionEntry,    75));
        set.insert(QLatin1String("lossless"), group.readEntry(d->configJXLLossLessEntry,       true));
        d->JXLOptions->setSettings(set);
    }

    d->showImageSettingsDialog->setChecked(group.readEntry(d->configShowImageSettingsDialog,   true));
}

} // namespace Digikam
