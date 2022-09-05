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
#include <QListWidget>
#include <QLabel>
#include <QFile>
#include <QMenu>
#include <QScopedPointer>
#include <QEventLoop>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "dimg.h"
#include "donlinetranslator.h"
#include "localizesettings.h"
#include "localizeselector.h"
#include "altlangstredit.h"

namespace DigikamBqmTranslatePlugin
{

class Q_DECL_HIDDEN Translate::Private
{

public:

    enum Entries
    {
        Title       = 0x01,
        Caption     = 0x02,
        Copyrights  = 0x04,
        UsageTerms  = 0x08
    };

public:

    explicit Private()
      : titleCB       (nullptr),
        captionCB     (nullptr),
        copyrightsCB  (nullptr),
        usageTermsCB  (nullptr),
        trLabel       (nullptr),
        tagsLabel     (nullptr),
        trSelector    (nullptr),
        trList        (nullptr),
        changeSettings(true)
    {
    }

    QCheckBox*        titleCB;
    QCheckBox*        captionCB;
    QCheckBox*        copyrightsCB;
    QCheckBox*        usageTermsCB;

    QLabel*           trLabel;
    QLabel*           tagsLabel;

    LocalizeSelector* trSelector;
    QListWidget*      trList;

    bool              changeSettings;
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

    d->tagsLabel             = new QLabel(i18nc("@label", "Entries to Translate:"), panel);
    d->titleCB               = new QCheckBox(i18nc("@option:check", "Title"),       panel);
    d->captionCB             = new QCheckBox(i18nc("@option:check", "Caption"),     panel);
    d->copyrightsCB          = new QCheckBox(i18nc("@option:check", "Copyrights"),  panel);
    d->usageTermsCB          = new QCheckBox(i18nc("@option:check", "Usage Terms"), panel);

    d->trLabel               = new QLabel(i18nc("@label", "Translate to:"),         panel);
    d->trSelector            = new LocalizeSelector(panel);
    d->trList                = new QListWidget(panel);
    d->trList->setContextMenuPolicy(Qt::CustomContextMenu);

    grid->addWidget(d->tagsLabel,    0, 0, 1, 2);
    grid->addWidget(d->titleCB,      1, 0, 1, 2);
    grid->addWidget(d->captionCB,    2, 0, 1, 2);
    grid->addWidget(d->copyrightsCB, 3, 0, 1, 2);
    grid->addWidget(d->usageTermsCB, 4, 0, 1, 2);
    grid->addWidget(d->trLabel,      5, 0, 1, 1);
    grid->addWidget(d->trSelector,   5, 1, 1, 1);
    grid->addWidget(d->trList,       6, 0, 1, 2);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(7, 10);

    m_settingsWidget = panel;

    connect(d->titleCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->captionCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->copyrightsCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->usageTermsCB, SIGNAL(toggled(bool)),
            this, SLOT(slotSettingsChanged()));

    connect(d->trSelector, SIGNAL(signalTranslate(QString)),
            this, SLOT(slotAppendTranslation(QString)));

    connect(d->trList, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotShowContextMenu(QPoint)));

    BatchTool::registerSettingsWidget();
}

void Translate::slotShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = d->trList->mapToGlobal(pos);

    QMenu menu;
    QAction* const rm = menu.addAction(i18nc("@action", "Remove this entry"));
    QAction* const cl = menu.addAction(i18nc("@action", "Clear List"));
    QAction* const ac = menu.exec(globalPos);

    if      (ac == rm)
    {
        delete d->trList->takeItem(d->trList->currentRow());
    }
    else if (ac == cl)
    {
        d->trList->clear();
    }

    slotSettingsChanged();
}

void Translate::slotAppendTranslation(const QString& lang)
{
    for (int i = 0 ; i < d->trList->count() ; ++i)
    {
        if (d->trList->item(i)->text().startsWith(lang))
        {
            return;
        }
    }

    d->trList->addItem(QString::fromUtf8("%1 - %2").arg(lang).arg(AltLangStrEdit::languageNameRFC3066(lang)));
    slotSettingsChanged();
}

BatchToolSettings Translate::defaultSettings()
{
    BatchToolSettings settings;

    settings.insert(QLatin1String("Title"),      false);
    settings.insert(QLatin1String("Caption"),    false);
    settings.insert(QLatin1String("Copyrights"), false);
    settings.insert(QLatin1String("UsageTerms"), false);
    settings.insert(QLatin1String("TrLangs"),    QStringList() << QLatin1String("en-US"));

    return settings;
}

void Translate::slotAssignSettings2Widget()
{
    d->changeSettings = false;

    d->titleCB->setChecked(settings()[QLatin1String("Title")].toBool());
    d->captionCB->setChecked(settings()[QLatin1String("Caption")].toBool());
    d->copyrightsCB->setChecked(settings()[QLatin1String("Copyrights")].toBool());
    d->usageTermsCB->setChecked(settings()[QLatin1String("UsageTerms")].toBool());

    QStringList langs = settings()[QLatin1String("TrLangs")].toStringList();

    d->trList->clear();

    Q_FOREACH (const QString& lg, langs)
    {
        d->trList->addItem(QString::fromUtf8("%1 - %2").arg(lg).arg(AltLangStrEdit::languageNameRFC3066(lg)));
    }

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

        QStringList langs;

        for (int i = 0 ; i < d->trList->count() ; ++i)
        {
            langs << d->trList->item(i)->text().section(QLatin1String(" - "), 0, 0);
        }

        settings.insert(QLatin1String("TrLangs"),     langs);

        BatchTool::slotSettingsChanged(settings);
    }
}

bool Translate::toolOperations()
{
    bool ret = true;
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (image().isNull())
    {
        if (!meta->load(inputUrl().toLocalFile()))
        {
            return false;
        }
    }
    else
    {
        meta->setData(image().getMetadata());
    }

    bool titleStage      = settings()[QLatin1String("Title")].toBool();
    bool captionStage    = settings()[QLatin1String("Caption")].toBool();
    bool copyrightsStage = settings()[QLatin1String("Copyrights")].toBool();
    bool usageTermsStage = settings()[QLatin1String("UsageTerms")].toBool();
    QStringList langs    = settings()[QLatin1String("TrLangs")].toStringList();

    Q_FOREACH (const QString& trLang, langs)
    {
        if (titleStage)
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Translate Title";
            ret &= insertTranslation(Private::Title, trLang, meta.data());
        }

        if (captionStage)
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Translate Caption";
            ret &= insertTranslation(Private::Caption, trLang, meta.data());
        }

        if (copyrightsStage)
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Translate Copyrights";
            ret &= insertTranslation(Private::Copyrights, trLang, meta.data());
        }

        if (usageTermsStage)
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Translate Usage Terms";
            ret &= insertTranslation(Private::UsageTerms, trLang, meta.data());
        }

        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Title     :" << meta->getXmpTagStringListLangAlt("Xmp.dc.title", false);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Caption   :" << meta->getXmpTagStringListLangAlt("Xmp.dc.description", false);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "CopyRights:" << meta->getXmpTagStringListLangAlt("Xmp.dc.rights", false);
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "UsageTerms:" << meta->getXmpTagStringListLangAlt("Xmp.xmpRights.UsageTerms", false);

        if (image().isNull())
        {
            QFile::remove(outputUrl().toLocalFile());
            ret &= QFile::copy(inputUrl().toLocalFile(), outputUrl().toLocalFile());

            if (ret && (titleStage || captionStage || copyrightsStage || usageTermsStage))
            {
                ret &= meta->save(outputUrl().toLocalFile());
                qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Save metadata to file:" << ret;
            }
        }
        else
        {
            if (titleStage || captionStage || copyrightsStage || usageTermsStage)
            {
                qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Save metadata to image";
                image().setMetadata(meta->data());
            }

            ret &= savefromDImg();
        }
    }

    return ret;
}

bool Translate::insertTranslation(int entry, const QString& trLang, DMetadata* const meta) const
{
    bool ret = true;
    CaptionsMap captions;
    DMetadata::AltLangMap map;

    switch (entry)
    {
        case Private::Title:
        {
            captions = meta->getItemTitles();
            map      = captions.toAltLangMap();
            break;
        }

        case Private::Caption:
        {
            captions = meta->getItemComments();
            map     = captions.toAltLangMap();
            break;
        }

        case Private::Copyrights:
        {
            map = meta->getXmpTagStringListLangAlt("Xmp.dc.rights", false);
            break;
        }

        case Private::UsageTerms:
        {
            map = meta->getXmpTagStringListLangAlt("Xmp.xmpRights.UsageTerms", false);
            break;
        }
    }

    if (!map.isEmpty() && map.contains(QLatin1String("x-default")))
    {
        QString sc = map.value(QLatin1String("x-default"));

        if (!sc.isEmpty())
        {
            QString tr;
            ret = translateString(sc, trLang, tr);

            if (ret)
            {
                switch (entry)
                {
                    case Private::Title:
                    {
                        CaptionValues val = captions[QLatin1String("x-default")];
                        val.caption       = tr;
                        captions[trLang]  = val;
                        meta->setItemTitles(captions);
                        break;
                    }

                    case Private::Caption:
                    {
                        CaptionValues val = captions[QLatin1String("x-default")];
                        val.caption       = tr;
                        captions[trLang]  = val;
                        meta->setItemComments(captions);
                        break;
                    }

                    case Private::Copyrights:
                    {
                        map[trLang] = tr;
                        meta->setXmpTagStringListLangAlt("Xmp.dc.rights", map);
                        break;
                    }

                    case Private::UsageTerms:
                    {
                        map[trLang] = tr;
                        meta->setXmpTagStringListLangAlt("Xmp.xmpRights.UsageTerms", map);
                        break;
                    }
                }
            }
            else
            {
                qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Cannot process online translation from" << meta->getFilePath();
            }
        }
        else
        {
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "No x-default language found from" << meta->getFilePath();
        }
    }
    else
    {
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "No alternative language string found from" << meta->getFilePath();
    }

    return ret;
}

bool Translate::translateString(const QString& text, const QString& trCode, QString& tr) const
{
    QScopedPointer<DOnlineTranslator> trengine(new DOnlineTranslator);
    QScopedPointer<QEventLoop> waitingLoop(new QEventLoop);

    DOnlineTranslator::Language srcLang = DOnlineTranslator::Auto;
    DOnlineTranslator::Language trLang  = DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(LocalizeSettings::instance()->settings().translatorEngine, trCode));

    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Request to translate with Web-service:";
    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Text to translate        :" << text;
    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "To target language       :" << trLang;
    qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "With source language     :" << srcLang;

    connect(trengine.data(), &DOnlineTranslator::signalFinished,
            waitingLoop.data(), &QEventLoop::quit);

    trengine->translate(text,                                                            // String to translate
                        LocalizeSettings::instance()->settings().translatorEngine,       // Web service
                        trLang,                                                          // Target language
                        srcLang,                                                         // Source langage
                        DOnlineTranslator::Auto);

    waitingLoop->exec(QEventLoop::ExcludeUserInputEvents);

    if (trengine->error() == DOnlineTranslator::NoError)
    {
        tr = trengine->translation();
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Text translated          :" << tr;

        return true;
    }
    else
    {
        qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Translation Error       :" << trengine->error();
    }

    return false;
}

} // namespace DigikamBqmTranslatePlugin
