/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-14
 * Description : remove metadata batch tool.
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "removemetadata.h"

// Qt includes

#include <QGridLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QWidget>
#include <QLabel>
#include <QFile>
#include <QScopedPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dimg.h"
#include "dmetadata.h"

namespace DigikamBqmRemoveMetadataPlugin
{

class Q_DECL_HIDDEN RemoveMetadata::Private
{
public:

    enum RemoveAction
    {
        ALL = 0,
        GPS,
        DATE,
        EXIF,
        VIDEO,
        DUBLIN,
        DIGIKAM,
        HISTORY,
        XPKEYWORDS
    };

public:

    explicit Private()
      : removeExif      (nullptr),
        removeIptc      (nullptr),
        removeXmp       (nullptr),
        exifComboBox    (nullptr),
        iptcComboBox    (nullptr),
        xmpComboBox     (nullptr),
        changeSettings  (true)
    {
    }

    QCheckBox* removeExif;
    QCheckBox* removeIptc;
    QCheckBox* removeXmp;

    QComboBox* exifComboBox;
    QComboBox* iptcComboBox;
    QComboBox* xmpComboBox;

    bool       changeSettings;
};

RemoveMetadata::RemoveMetadata(QObject* const parent)
    : BatchTool(QLatin1String("RemoveMetadata"), MetadataTool, parent),
      d        (new Private)
{
}

RemoveMetadata::~RemoveMetadata()
{
    delete d;
}

void RemoveMetadata::registerSettingsWidget()
{
    QWidget* const panel     = new QWidget;
    QGridLayout* const grid  = new QGridLayout(panel);

    d->removeExif            = new QCheckBox(i18nc("@title", "Exif:"), panel);
    d->exifComboBox          = new QComboBox(panel);
    d->exifComboBox->addItem(i18nc("@item: exif namespace", "Completely"), Private::ALL);
    d->exifComboBox->addItem(i18nc("@item: exif namespace", "Date"),       Private::DATE);
    d->exifComboBox->addItem(i18nc("@item: exif namespace", "GPS"),        Private::GPS);
    d->exifComboBox->addItem(i18nc("@item: exif namespace", "XPKeywords"), Private::XPKEYWORDS);

    d->removeIptc            = new QCheckBox(i18nc("@title", "Iptc:"), panel);
    d->iptcComboBox          = new QComboBox(panel);
    d->iptcComboBox->addItem(i18nc("@item: iptc namespace", "Completely"), Private::ALL);
    d->iptcComboBox->addItem(i18nc("@item: iptc namespace", "Date"),       Private::DATE);

    d->removeXmp             = new QCheckBox(i18nc("@title", "Xmp:"), panel);
    d->xmpComboBox           = new QComboBox(panel);
    d->xmpComboBox->addItem(i18nc("@item: xmp namespace", "Completely"),            Private::ALL);
    d->xmpComboBox->addItem(i18nc("@item: xmp namespace", "Date"),                  Private::DATE);
    d->xmpComboBox->addItem(i18nc("@item: xmp namespace", "DigiKam"),               Private::DIGIKAM);
    d->xmpComboBox->addItem(i18nc("@item: xmp namespace", "DigiKam image history"), Private::HISTORY);
    d->xmpComboBox->addItem(i18nc("@item: xmp namespace", "Dublin Core"),           Private::DUBLIN);
    d->xmpComboBox->addItem(i18nc("@item: xmp namespace", "Exif"),                  Private::EXIF);
    d->xmpComboBox->addItem(i18nc("@item: xmp namespace", "Video"),                 Private::VIDEO);

    grid->addWidget(d->removeExif,   0, 0, 1, 1);
    grid->addWidget(d->exifComboBox, 0, 1, 1, 2);
    grid->addWidget(d->removeIptc,   1, 0, 1, 1);
    grid->addWidget(d->iptcComboBox, 1, 1, 1, 2);
    grid->addWidget(d->removeXmp,    2, 0, 1, 1);
    grid->addWidget(d->xmpComboBox,  2, 1, 1, 2);
    grid->setColumnStretch(2, 10);
    grid->setRowStretch(3, 10);

    m_settingsWidget = panel;

    connect(d->removeExif, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->removeIptc, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->removeXmp, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->exifComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->iptcComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->xmpComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings RemoveMetadata::defaultSettings()
{
    BatchToolSettings settings;

    settings.insert(QLatin1String("RemoveExif"), false);
    settings.insert(QLatin1String("RemoveIptc"), false);
    settings.insert(QLatin1String("RemoveXmp"),  false);
    settings.insert(QLatin1String("ExifData"),   Private::ALL);
    settings.insert(QLatin1String("IptcData"),   Private::ALL);
    settings.insert(QLatin1String("XmpData"),    Private::ALL);

    return settings;
}

void RemoveMetadata::slotAssignSettings2Widget()
{
    d->changeSettings = false;

    d->removeExif->setChecked(settings()[QLatin1String("RemoveExif")].toBool());
    d->removeIptc->setChecked(settings()[QLatin1String("RemoveIptc")].toBool());
    d->removeXmp->setChecked(settings()[QLatin1String("RemoveXmp")].toBool());

    int exifData = settings()[QLatin1String("ExifData")].toInt();
    d->exifComboBox->setCurrentIndex(d->exifComboBox->findData(exifData));

    int iptcData = settings()[QLatin1String("IptcData")].toInt();
    d->iptcComboBox->setCurrentIndex(d->iptcComboBox->findData(iptcData));

    int xmpData  = settings()[QLatin1String("XmpData")].toInt();
    d->xmpComboBox->setCurrentIndex(d->xmpComboBox->findData(xmpData));

    d->exifComboBox->setEnabled(d->removeExif->isChecked());
    d->iptcComboBox->setEnabled(d->removeIptc->isChecked());
    d->xmpComboBox->setEnabled(d->removeXmp->isChecked());

    d->changeSettings = true;
}

void RemoveMetadata::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;

        settings.insert(QLatin1String("RemoveExif"), d->removeExif->isChecked());
        settings.insert(QLatin1String("RemoveIptc"), d->removeIptc->isChecked());
        settings.insert(QLatin1String("RemoveXmp"),  d->removeXmp->isChecked());
        settings.insert(QLatin1String("ExifData"),   d->exifComboBox->currentData().toInt());
        settings.insert(QLatin1String("IptcData"),   d->iptcComboBox->currentData().toInt());
        settings.insert(QLatin1String("XmpData" ),   d->xmpComboBox->currentData().toInt());

        BatchTool::slotSettingsChanged(settings);
    }
}

bool RemoveMetadata::toolOperations()
{
    if (!isLastChainedTool())
    {
        setErrorDescription(i18nc("@info", "Remove Metadata: Not the last tool in the list."));
        return false;
    }

    bool ret = true;
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (image().isNull())
    {
        QFile::remove(outputUrl().toLocalFile());
        ret = QFile::copy(inputUrl().toLocalFile(), outputUrl().toLocalFile());

        if (!ret || !meta->load(outputUrl().toLocalFile()))
        {
            return ret;
        }
    }
    else
    {
        ret = savefromDImg();
        meta->setData(image().getMetadata());
    }

    bool removeExif = settings()[QLatin1String("RemoveExif")].toBool();
    bool removeIptc = settings()[QLatin1String("RemoveIptc")].toBool();
    bool removeXmp  = settings()[QLatin1String("RemoveXmp")].toBool();

    int exifData    = settings()[QLatin1String("ExifData")].toInt();
    int iptcData    = settings()[QLatin1String("IptcData")].toInt();
    int xmpData     = settings()[QLatin1String("XmpData")].toInt();

    if (removeExif)
    {
        if      (exifData == Private::ALL)
        {
            meta->clearExif();
        }
        else if (exifData == Private::DATE)
        {
            meta->removeExifTag("Exif.Image.DateTime");
            meta->removeExifTag("Exif.Image.PreviewDateTime");
            meta->removeExifTag("Exif.Photo.DateTimeOriginal");
            meta->removeExifTag("Exif.Photo.DateTimeDigitized");
        }
        else if (exifData == Private::GPS)
        {
            meta->removeExifTags(QStringList() << QLatin1String("GPSInfo"));
        }
        else if (exifData == Private::XPKEYWORDS)
        {
            meta->removeExifTag("Exif.Image.XPKeywords");
        }
    }

    if (removeIptc)
    {
        if      (iptcData == Private::ALL)
        {
            meta->clearIptc();
        }
        else if (iptcData == Private::DATE)
        {
            meta->removeIptcTag("Iptc.Application2.DateCreated");
            meta->removeIptcTag("Iptc.Application2.TimeCreated");
        }
    }

    if (removeXmp)
    {
        if      (xmpData == Private::ALL)
        {
            meta->clearXmp();
        }
        else if (xmpData == Private::DATE)
        {
            meta->removeXmpTag("Xmp.photoshop.DateCreated");
            meta->removeXmpTag("Xmp.exif.DateTimeOriginal");
            meta->removeXmpTag("Xmp.xmp.MetadataDate");
            meta->removeXmpTag("Xmp.xmp.CreateDate");
            meta->removeXmpTag("Xmp.xmp.ModifyDate");
            meta->removeXmpTag("Xmp.tiff.DateTime");
            meta->removeXmpTag("Xmp.video.DateTimeDigitized");
            meta->removeXmpTag("Xmp.video.DateTimeOriginal");
            meta->removeXmpTag("Xmp.video.ModificationDate");
            meta->removeXmpTag("Xmp.video.DateUTC");
        }
        else if (xmpData == Private::HISTORY)
        {
            meta->removeXmpTag("Xmp.digiKam.ImageHistory");
        }
        else if (xmpData == Private::DIGIKAM)
        {
            meta->removeXmpTags(QStringList() << QLatin1String("digiKam"));
        }
        else if (xmpData == Private::DUBLIN)
        {
            meta->removeXmpTags(QStringList() << QLatin1String("dc"));
        }
        else if (xmpData == Private::EXIF)
        {
            meta->removeXmpTags(QStringList() << QLatin1String("exif"));
        }
        else if (xmpData == Private::VIDEO)
        {
            meta->removeXmpTags(QStringList() << QLatin1String("video"));
        }
    }

    if (ret && (removeExif || removeIptc || removeXmp))
    {
        ret = meta->save(outputUrl().toLocalFile());
    }

    return ret;
}

} // namespace DigikamBqmRemoveMetadataPlugin
