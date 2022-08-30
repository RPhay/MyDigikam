/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-30
 * Description : translate metadata batch tool.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "translate.h"

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
#include "donlinetranslator.h"
#include "localizesettings.h"

namespace DigikamBqmTranslatePlugin
{

class Q_DECL_HIDDEN Translate::Private
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
        COMMENT,
        DIGIKAM,
        HISTORY,
        XPKEYWORDS

    };

public:

    explicit Private()
      : titleCB       (nullptr),
        captionCB     (nullptr),
        copyrightsCB  (nullptr),
        usageTermsCB  (nullptr),
        trComboBox    (nullptr),
        changeSettings(true)
    {
    }

    QCheckBox* titleCB;
    QCheckBox* captionCB;
    QCheckBox* copyrightsCB;
    QCheckBox* usageTermsCB;

    QComboBox* trComboBox;

    bool       changeSettings;
};

Translate::Translate(QObject* const parent)
    : BatchTool(QLatin1String("Translate"), MetadataTool, parent),
      d        (new Private)
{
}

Translate::~Translate()
{
    delete d;
}

BatchTool* Translate::clone(QObject* const parent) const
{
    return new Translate(parent);
}

void Translate::registerSettingsWidget()
{
    QWidget* const panel     = new QWidget;
    QGridLayout* const grid  = new QGridLayout(panel);

    d->titleCB               = new QCheckBox(i18nc("@title", "Title"),       panel);
    d->captionCB             = new QCheckBox(i18nc("@title", "Caption"),     panel);
    d->copyrightsCB          = new QCheckBox(i18nc("@title", "Copyrights"),  panel);
    d->usageTermsCB          = new QCheckBox(i18nc("@title", "Usage Terms"), panel);

    d->trComboBox            = new QComboBox(panel);

    QStringList allRFC3066  = DOnlineTranslator::supportedRFC3066(LocalizeSettings::instance()->settings().translatorEngine);
    LocalizeContainer set   = LocalizeSettings::instance()->settings();

    Q_FOREACH (const QString& lg, set.translatorLang)
    {
        d->trComboBox->addItem(lg);
        d->trComboBox->setItemData(d->trComboBox->findText(lg), i18nc("@info", "Translate to %1", languageNameRFC3066(lg)), Qt::ToolTipRole);
    }

    grid->addWidget(d->titleCB,      0, 0, 1, 1);
    grid->addWidget(d->captionCB,    1, 0, 1, 1);
    grid->addWidget(d->copyrightsCB, 2, 0, 1, 1);
    grid->addWidget(d->usageTermsCB, 3, 0, 1, 1);
    grid->addWidget(d->trComboBox,   4, 0, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(5, 10);

    m_settingsWidget = panel;

    connect(d->titleCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->captionCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->copyrightsCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->usageTermsCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->trComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings Translate::defaultSettings()
{
    BatchToolSettings settings;

    settings.insert(QLatin1String("Title"),      false);
    settings.insert(QLatin1String("Caption"),    false);
    settings.insert(QLatin1String("Copyrights"), false);
    settings.insert(QLatin1String("UsageTerms"), false);
    settings.insert(QLatin1String("TrLang"),     QLatin1String("en-US"));

    return settings;
}

void Translate::slotAssignSettings2Widget()
{
    d->changeSettings = false;

    d->titleCB->setChecked(settings()[QLatin1String("Title")].toBool());
    d->captionCB->setChecked(settings()[QLatin1String("Caption")].toBool());
    d->copyrightsCB->setChecked(settings()[QLatin1String("Copyrights")].toBool());
    d->usageTermsCB->setChecked(settings()[QLatin1String("UsageTerms")].toBool());

    QString lang = settings()[QLatin1String("TrLang")].toString();
    d->trComboBox->setCurrentIndex(d->trComboBox->findData(lang));

    d->changeSettings = true;
}

void Translate::slotSettingsChanged()
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

bool Translate::toolOperations()
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
        else if (exifData == Private::COMMENT)
        {
            meta->removeExifTag("Exif.Image.ImageDescription");
            meta->removeExifTag("Exif.Photo.UserComment");
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
        else if (iptcData == Private::COMMENT)
        {
            meta->removeIptcTag("Iptc.Application2.Caption");
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
        else if (xmpData == Private::COMMENT)
        {
            meta->removeXmpTag("Xmp.acdsee.Caption");
            meta->removeXmpTag("Xmp.dc.Description");
            meta->removeXmpTag("Xmp.crs.Description");
            meta->removeXmpTag("Xmp.exif.UserComment");
            meta->removeXmpTag("Xmp.tiff.ImageDescription");
            meta->removeXmpTag("Xmp.xmp.Description");
            meta->removeXmpTag("Xmp.xmpDM.DMComment");
        }
    }

    if (ret && (removeExif || removeIptc || removeXmp))
    {
        ret = meta->save(outputUrl().toLocalFile());
    }

    return ret;
}

} // namespace DigikamBqmTranslatePlugin
