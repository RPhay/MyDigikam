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
    d->trComboBox->setCurrentIndex(d->trComboBox->findText(lang));

    d->changeSettings = true;
}

void Translate::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;

        settings.insert(QLatin1String("Title"),       d->titleCB->isChecked());
        settings.insert(QLatin1String("Caption"),     d->captionCB->isChecked());
        settings.insert(QLatin1String("Copyrights"),  d->copyrightsCB->isChecked());
        settings.insert(QLatin1String("UsageTerms"),  d->usageTermsCB->isChecked());
        settings.insert(QLatin1String("trLang"),      d->trComboBox->currentText());

        BatchTool::slotSettingsChanged(settings);
    }
}

bool Translate::toolOperations()
{
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

    bool titleAc       = settings()[QLatin1String("Title")].toBool();
    bool captionAc     = settings()[QLatin1String("Caption")].toBool();
    bool copyrightsAc  = settings()[QLatin1String("Copyrights")].toBool();
    bool usageTermsAc  = settings()[QLatin1String("UsageTerms")].toBool();
    QString trLang     = settings()[QLatin1String("TrLAng")].toString();

    if (titleAc)
    {
        
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

QString Translate::translate(const QString& text, const QString& trCode)
{
    DOnlineTranslator* const trengine   = new DOnlineTranslator;
    DOnlineTranslator::Language srcLang = DOnlineTranslator::Auto;
    DOnlineTranslator::Language trLang  = DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(LocalizeSettings::instance()->settings().translatorEngine, trCode));

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Request to translate with Web-service:";
    qCDebug(DIGIKAM_WIDGETS_LOG) << "Text to translate        :" << text;
    qCDebug(DIGIKAM_WIDGETS_LOG) << "To target language       :" << trLang;
    qCDebug(DIGIKAM_WIDGETS_LOG) << "With source language     :" << srcLang;

    trengine->translate(text,                                                            // String to translate
                        LocalizeSettings::instance()->settings().translatorEngine,       // Web service
                        trLang,                                                          // Target language
                        srcLang,                                                         // Source langage
                        DOnlineTranslator::Auto);
}

void AltLangStrEdit::slotTranslationFinished()
{
    setDisabled(false);

    if (d->trengine->error() == DOnlineTranslator::NoError)
    {
        if (d->trCode.isEmpty())
        {
            return;
        }

        QString translation = d->trengine->translation();
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Text translated          :" << translation;

        MetaEngine::AltLangMap vals = values();
        vals.insert(d->trCode, translation);
        setValues(vals);

        Q_EMIT signalValueAdded(d->trCode, translation);

        d->languageCB->setCurrentText(d->trCode);
        d->trCode.clear();
    }
    else
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Translation Error       :" << d->trengine->error();

        QMessageBox::information(qApp->activeWindow(),
                                 i18nc("@info", "Failed to translate string with %1 Web-service",
                                 DOnlineTranslator::engineName(LocalizeSettings::instance()->settings().translatorEngine)),
                                 i18nc("@info", "Error message: %1",
                                 d->trengine->errorString()));

    }
}

} // namespace DigikamBqmTranslatePlugin
