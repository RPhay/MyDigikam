/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-14
 * Description : remove metadata batch tool.
 *
 * Copyright (C) 2010-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dimg.h"
#include "dmetadata.h"

namespace DigikamBqmRemoveMetadataPlugin
{

RemoveMetadata::RemoveMetadata(QObject* const parent)
    : BatchTool(QLatin1String("RemoveMetadata"), MetadataTool, parent),
      m_removeExif(nullptr),
      m_removeIptc(nullptr),
      m_removeXmp(nullptr),
      m_exifComboBox(nullptr),
      m_iptcComboBox(nullptr),
      m_xmpComboBox(nullptr),
      m_changeSettings(true)
{
}

RemoveMetadata::~RemoveMetadata()
{
}

void RemoveMetadata::registerSettingsWidget()
{
    QWidget* const panel    = new QWidget;
    QGridLayout* const grid = new QGridLayout(panel);

    m_removeExif            = new QCheckBox(i18n("Exif:"), panel);
    m_exifComboBox          = new QComboBox(panel);
    m_exifComboBox->addItem(i18n("Completely"), RemoveAction::ALL);
    m_exifComboBox->addItem(i18n("Date"),       RemoveAction::DATE);

    m_removeIptc            = new QCheckBox(i18n("Iptc:"), panel);
    m_iptcComboBox          = new QComboBox(panel);
    m_iptcComboBox->addItem(i18n("Completely"), RemoveAction::ALL);
    m_iptcComboBox->addItem(i18n("Date"),       RemoveAction::DATE);

    m_removeXmp             = new QCheckBox(i18n("Xmp:"), panel);
    m_xmpComboBox           = new QComboBox(panel);
    m_xmpComboBox->addItem(i18n("Completely"), RemoveAction::ALL);
    m_xmpComboBox->addItem(i18n("Date"),       RemoveAction::DATE);
    m_xmpComboBox->addItem(i18n("Video"),      RemoveAction::VIDEO);

    grid->addWidget(m_removeExif,   0, 0, 1, 1);
    grid->addWidget(m_exifComboBox, 0, 1, 1, 2);
    grid->addWidget(m_removeIptc,   1, 0, 1, 1);
    grid->addWidget(m_iptcComboBox, 1, 1, 1, 2);
    grid->addWidget(m_removeXmp,    2, 0, 1, 1);
    grid->addWidget(m_xmpComboBox,  2, 1, 1, 2);
    grid->setColumnStretch(2, 10);
    grid->setRowStretch(3, 10);

    m_settingsWidget = panel;

    connect(m_removeExif, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(m_removeIptc, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(m_removeXmp, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(m_exifComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(m_iptcComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(m_xmpComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings RemoveMetadata::defaultSettings()
{
    BatchToolSettings settings;

    settings.insert(QLatin1String("RemoveExif"), false);
    settings.insert(QLatin1String("RemoveIptc"), false);
    settings.insert(QLatin1String("RemoveXmp"),  false);
    settings.insert(QLatin1String("ExifData"),   RemoveAction::ALL);
    settings.insert(QLatin1String("IptcData"),   RemoveAction::ALL);
    settings.insert(QLatin1String("XmpData"),    RemoveAction::ALL);

    m_exifComboBox->setEnabled(false);
    m_iptcComboBox->setEnabled(false);
    m_xmpComboBox->setEnabled(false);

    return settings;
}

void RemoveMetadata::slotAssignSettings2Widget()
{
    m_changeSettings = false;

    m_removeExif->setChecked(settings()[QLatin1String("RemoveExif")].toBool());
    m_removeIptc->setChecked(settings()[QLatin1String("RemoveIptc")].toBool());
    m_removeXmp->setChecked(settings()[QLatin1String("RemoveXmp")].toBool());

    int exifData = settings()[QLatin1String("ExifData")].toInt();
    m_exifComboBox->setCurrentIndex(m_exifComboBox->findData(exifData));

    int iptcData = settings()[QLatin1String("IptcData")].toInt();
    m_iptcComboBox->setCurrentIndex(m_iptcComboBox->findData(iptcData));

    int xmpData  = settings()[QLatin1String("XmpData")].toInt();
    m_xmpComboBox->setCurrentIndex(m_xmpComboBox->findData(xmpData));

    m_exifComboBox->setEnabled(m_removeExif->isChecked());
    m_iptcComboBox->setEnabled(m_removeIptc->isChecked());
    m_xmpComboBox->setEnabled(m_removeXmp->isChecked());

    m_changeSettings = true;
}

void RemoveMetadata::slotSettingsChanged()
{
    if (m_changeSettings)
    {
        BatchToolSettings settings;

        settings.insert(QLatin1String("RemoveExif"), m_removeExif->isChecked());
        settings.insert(QLatin1String("RemoveIptc"), m_removeIptc->isChecked());
        settings.insert(QLatin1String("RemoveXmp"),  m_removeXmp->isChecked());
        settings.insert(QLatin1String("ExifData"),   m_exifComboBox->currentData().toInt());
        settings.insert(QLatin1String("IptcData"),   m_iptcComboBox->currentData().toInt());
        settings.insert(QLatin1String("XmpData" ),   m_xmpComboBox->currentData().toInt());

        m_exifComboBox->setEnabled(m_removeExif->isChecked());
        m_iptcComboBox->setEnabled(m_removeIptc->isChecked());
        m_xmpComboBox->setEnabled(m_removeXmp->isChecked());

        BatchTool::slotSettingsChanged(settings);
    }
}

bool RemoveMetadata::toolOperations()
{
    if (!isLastChainedTool())
    {
        setErrorDescription(i18n("Remove Metadata: Not the last tool in the list."));
        return false;
    }

    bool ret = true;
    DMetadata meta;

    if (image().isNull())
    {
        QFile::remove(outputUrl().toLocalFile());
        ret = QFile::copy(inputUrl().toLocalFile(), outputUrl().toLocalFile());

        if (!ret || !meta.load(outputUrl().toLocalFile()))
        {
            return ret;
        }
    }
    else
    {
        ret = savefromDImg();
        meta.setData(image().getMetadata());
    }

    bool removeExif = settings()[QLatin1String("RemoveExif")].toBool();
    bool removeIptc = settings()[QLatin1String("RemoveIptc")].toBool();
    bool removeXmp  = settings()[QLatin1String("RemoveXmp")].toBool();

    int exifData    = settings()[QLatin1String("ExifData")].toInt();
    int iptcData    = settings()[QLatin1String("IptcData")].toInt();
    int xmpData     = settings()[QLatin1String("XmpData")].toInt();

    if (removeExif)
    {
        if      (exifData == RemoveAction::ALL)
        {
            meta.clearExif();
        }
        else if (exifData == RemoveAction::DATE)
        {
            meta.removeExifTag("Exif.Image.DateTime");
            meta.removeExifTag("Exif.Image.PreviewDateTime");
            meta.removeExifTag("Exif.Photo.DateTimeOriginal");
            meta.removeExifTag("Exif.Photo.DateTimeDigitized");
        }
    }

    if (removeIptc)
    {
        if      (iptcData == RemoveAction::ALL)
        {
            meta.clearIptc();
        }
        else if (iptcData == RemoveAction::DATE)
        {
            meta.removeIptcTag("Iptc.Application2.DateCreated");
            meta.removeIptcTag("Iptc.Application2.TimeCreated");
        }
    }

    if (removeXmp)
    {
        if      (xmpData == RemoveAction::ALL)
        {
            meta.clearXmp();
        }
        else if (xmpData == RemoveAction::DATE)
        {
            meta.removeXmpTag("Xmp.photoshop.DateCreated");
            meta.removeXmpTag("Xmp.exif.DateTimeOriginal");
            meta.removeXmpTag("Xmp.xmp.MetadataDate");
            meta.removeXmpTag("Xmp.xmp.CreateDate");
            meta.removeXmpTag("Xmp.xmp.ModifyDate");
            meta.removeXmpTag("Xmp.tiff.DateTime");
            meta.removeXmpTag("Xmp.video.DateTimeDigitized");
            meta.removeXmpTag("Xmp.video.DateTimeOriginal");
            meta.removeXmpTag("Xmp.video.ModificationDate");
            meta.removeXmpTag("Xmp.video.DateUTC");
        }
        else if (xmpData == RemoveAction::VIDEO)
        {
            meta.removeXmpTags(QStringList() << QLatin1String("video"));
        }
    }

    if (ret && (removeExif || removeIptc || removeXmp))
    {
        ret = meta.save(outputUrl().toLocalFile());
    }

    return ret;
}

} // namespace DigikamBqmRemoveMetadataPlugin
