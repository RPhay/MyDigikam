/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-05-03
 * Description : DNG convert settings for camera interface.
 *
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2016 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "dngconvertsettings.h"

// Qt includes

#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

#include "dngsettings.h"
#include "setup.h"

namespace Digikam
{

class Q_DECL_HIDDEN DNGConvertSettings::Private
{
public:

    explicit Private()
        : convertDng  (nullptr),
          dngLink     (nullptr),
          dngSettings (nullptr)
    {
    }

    QCheckBox*   convertDng;
    QLabel*      dngLink;
    DNGSettings* dngSettings;
};

DNGConvertSettings::DNGConvertSettings(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing           = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QVBoxLayout* const mainVlay = new QVBoxLayout(this);
    QHBoxLayout* const convHlay = new QHBoxLayout(this);

    d->convertDng               = new QCheckBox(i18nc("@option:check", "Convert RAW images to"), this);
    d->dngSettings              = new DNGSettings(this);

    d->dngLink                  = new QLabel(this);
    d->dngLink->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->dngLink->setOpenExternalLinks(true);
    d->dngLink->setWordWrap(false);
    d->dngLink->setText(QString::fromUtf8("<p><a href='https://en.wikipedia.org/wiki/Digital_Negative_(file_format)'>%1</a></p>")
                        .arg(i18nc("@label", "DNG container")));
    convHlay->addWidget(d->convertDng);
    convHlay->addWidget(d->dngLink);
    convHlay->addStretch(10);
    convHlay->setContentsMargins(spacing, spacing, spacing, spacing);
    convHlay->setSpacing(0);

    mainVlay->addLayout(convHlay);
    mainVlay->addWidget(d->dngSettings);
    mainVlay->setContentsMargins(QMargins());
    mainVlay->setSpacing(0);

    setWhatsThis(i18nc("@info", "Set here all options to convert RAW images to DNG container automatically "
                                "as they are downloaded."));
    d->convertDng->setWhatsThis(i18nc("@info", "Enable this option to convert RAW images to DNG container automatically."));

    setLayout(mainVlay);

    // ---------------------------------------------------------------------------------------

    connect(d->convertDng, SIGNAL(toggled(bool)),
            d->dngSettings, SLOT(setEnabled(bool)));

    connect(d->convertDng, SIGNAL(toggled(bool)),
            this, SIGNAL(signalDownloadNameChanged()));

    connect(d->dngSettings, SIGNAL(signalSetupExifTool()),
            this, SLOT(slotSetupExifTool()));
}

DNGConvertSettings::~DNGConvertSettings()
{
    delete d;
}

void DNGConvertSettings::readSettings(KConfigGroup& group)
{
    d->dngSettings->setBackupOriginalRawFile(group.readEntry(QLatin1String("BackupOriginalRawFile"), false));
    d->dngSettings->setCompressLossLess(group.readEntry(QLatin1String("CompressLossLess"),           true));
    d->dngSettings->setPreviewMode(group.readEntry(QLatin1String("PreviewMode"),                     (int)DNGWriter::MEDIUM));
    d->convertDng->setChecked(group.readEntry(QLatin1String("ConvertDng"),                           false));

    d->dngSettings->setEnabled(d->convertDng->isChecked());
}

void DNGConvertSettings::saveSettings(KConfigGroup& group)
{
    group.writeEntry(QLatin1String("BackupOriginalRawFile"), d->dngSettings->backupOriginalRawFile());
    group.writeEntry(QLatin1String("CompressLossLess"),      d->dngSettings->compressLossLess());
    group.writeEntry(QLatin1String("PreviewMode"),           d->dngSettings->previewMode());
    group.writeEntry(QLatin1String("ConvertDng"),            d->convertDng->isChecked());
}

void DNGConvertSettings::settings(DownloadSettings* const settings)
{
    settings->backupRaw   = d->dngSettings->backupOriginalRawFile();
    settings->compressDng = d->dngSettings->compressLossLess();
    settings->previewMode = d->dngSettings->previewMode();
    settings->convertDng  = d->convertDng->isChecked();
}

void DNGConvertSettings::slotSetupExifTool()
{
    if (Setup::execExifTool(nullptr))
    {
        d->dngSettings->slotSetupChanged();
    }
}

} // namespace Digikam
