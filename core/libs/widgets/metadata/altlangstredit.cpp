/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-15
 * Description : multi-languages string editor
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

#include "altlangstredit.h"

// Qt includes

#include <QEvent>
#include <QMap>
#include <QStyle>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QApplication>
#include <QComboBox>

// KDE includes

#include <klocalizedstring.h>

namespace Digikam
{

class Q_DECL_HIDDEN AltLangStrEdit::Private
{
public:

    explicit Private()
      : currentLanguage (QLatin1String("x-default")),
        linesVisible    (0),
        titleLabel      (nullptr),
        delValueButton  (nullptr),
        valueEdit       (nullptr),
        languageCB      (nullptr)
    {

        /**
         * NOTE: We cannot use KLocale::allLanguagesList() here because KDE only
         * support 2 characters country codes. XMP require 2+2 characters language+country
         * following ISO 3066 (babelwiki.babelzilla.org/index.php?title=Language_codes)
         */

        // The first one from the list is the Default Language code specified by XMP paper

        languageCodeMap.insert( QLatin1String("x-default"), i18nc("@info: country code", "Default Language") );

        // Standard ISO 3066 country codes.

        languageCodeMap.insert( QLatin1String("af-ZA"), i18nc("@info: country code", "Afrikaans (South Africa)") );
        languageCodeMap.insert( QLatin1String("am-ET"), i18nc("@info: country code", "Amharic (Ethiopia)") );
        languageCodeMap.insert( QLatin1String("ar-AE"), i18nc("@info: country code", "Arabic (UAE)") );
        languageCodeMap.insert( QLatin1String("ar-BH"), i18nc("@info: country code", "Arabic (Bahrain)") );
        languageCodeMap.insert( QLatin1String("ar-DZ"), i18nc("@info: country code", "Arabic (Algeria)") );
        languageCodeMap.insert( QLatin1String("ar-EG"), i18nc("@info: country code", "Arabic (Egypt)") );
        languageCodeMap.insert( QLatin1String("ar-IQ"), i18nc("@info: country code", "Arabic (Iraq)") );
        languageCodeMap.insert( QLatin1String("ar-JO"), i18nc("@info: country code", "Arabic (Jordan)") );
        languageCodeMap.insert( QLatin1String("ar-KW"), i18nc("@info: country code", "Arabic (Kuwait)") );
        languageCodeMap.insert( QLatin1String("ar-LB"), i18nc("@info: country code", "Arabic (Lebanon)") );
        languageCodeMap.insert( QLatin1String("ar-LY"), i18nc("@info: country code", "Arabic (Libya)") );
        languageCodeMap.insert( QLatin1String("ar-MA"), i18nc("@info: country code", "Arabic (Morocco)") );
        languageCodeMap.insert( QLatin1String("ar-OM"), i18nc("@info: country code", "Arabic (Oman)") );
        languageCodeMap.insert( QLatin1String("ar-QA"), i18nc("@info: country code", "Arabic (Qatar)") );
        languageCodeMap.insert( QLatin1String("ar-SA"), i18nc("@info: country code", "Arabic (Saudi Arabia)") );
        languageCodeMap.insert( QLatin1String("ar-SY"), i18nc("@info: country code", "Arabic (Syria)") );
        languageCodeMap.insert( QLatin1String("ar-TN"), i18nc("@info: country code", "Arabic (Tunisia)") );
        languageCodeMap.insert( QLatin1String("ar-YE"), i18nc("@info: country code", "Arabic (Yemen)") );
        languageCodeMap.insert( QLatin1String("as-IN"), i18nc("@info: country code", "Assamese (India)") );
        languageCodeMap.insert( QLatin1String("ba-RU"), i18nc("@info: country code", "Bashkir (Russia)") );
        languageCodeMap.insert( QLatin1String("be-BY"), i18nc("@info: country code", "Belarusian (Belarus)") );
        languageCodeMap.insert( QLatin1String("bg-BG"), i18nc("@info: country code", "Bulgarian (Bulgaria)") );
        languageCodeMap.insert( QLatin1String("bn-IN"), i18nc("@info: country code", "Bengali (India)") );
        languageCodeMap.insert( QLatin1String("bo-BT"), i18nc("@info: country code", "Tibetan (Bhutan)") );
        languageCodeMap.insert( QLatin1String("bo-CN"), i18nc("@info: country code", "Tibetan (PRC)") );
        languageCodeMap.insert( QLatin1String("br-FR"), i18nc("@info: country code", "Breton (France)") );
        languageCodeMap.insert( QLatin1String("ca-AD"), i18nc("@info: country code", "Catalan (Andorra)") );
        languageCodeMap.insert( QLatin1String("ca-ES"), i18nc("@info: country code", "Catalan (Spain)") );
        languageCodeMap.insert( QLatin1String("ca-FR"), i18nc("@info: country code", "Catalan (France)") );
        languageCodeMap.insert( QLatin1String("co-FR"), i18nc("@info: country code", "Corsican (France)") );
        languageCodeMap.insert( QLatin1String("cs-CZ"), i18nc("@info: country code", "Czech (Czechia)") );
        languageCodeMap.insert( QLatin1String("cy-GB"), i18nc("@info: country code", "Welsh (United Kingdom)") );
        languageCodeMap.insert( QLatin1String("da-DK"), i18nc("@info: country code", "Danish (Denmark)") );
        languageCodeMap.insert( QLatin1String("de-AT"), i18nc("@info: country code", "German (Austria)") );
        languageCodeMap.insert( QLatin1String("de-CH"), i18nc("@info: country code", "German (Switzerland)") );
        languageCodeMap.insert( QLatin1String("de-DE"), i18nc("@info: country code", "German (Germany)") );
        languageCodeMap.insert( QLatin1String("de-LI"), i18nc("@info: country code", "German (Liechtenstein)") );
        languageCodeMap.insert( QLatin1String("de-LU"), i18nc("@info: country code", "German (Luxembourg)") );
        languageCodeMap.insert( QLatin1String("el-GR"), i18nc("@info: country code", "Greek (Greece)") );
        languageCodeMap.insert( QLatin1String("en-AU"), i18nc("@info: country code", "English (Australia)") );
        languageCodeMap.insert( QLatin1String("en-BZ"), i18nc("@info: country code", "English (Belize)") );
        languageCodeMap.insert( QLatin1String("en-CA"), i18nc("@info: country code", "English (Canada)") );
        languageCodeMap.insert( QLatin1String("en-CB"), i18nc("@info: country code", "English (Caribbean)") );
        languageCodeMap.insert( QLatin1String("en-GB"), i18nc("@info: country code", "English (United Kingdom)") );
        languageCodeMap.insert( QLatin1String("en-IE"), i18nc("@info: country code", "English (Ireland)") );
        languageCodeMap.insert( QLatin1String("en-IN"), i18nc("@info: country code", "English (India)") );
        languageCodeMap.insert( QLatin1String("en-JA"), i18nc("@info: country code", "English (Jamaica)") );
        languageCodeMap.insert( QLatin1String("en-MY"), i18nc("@info: country code", "English (Malaysia)") );
        languageCodeMap.insert( QLatin1String("en-NZ"), i18nc("@info: country code", "English (New Zealand)") );
        languageCodeMap.insert( QLatin1String("en-PH"), i18nc("@info: country code", "English (Philippines)") );
        languageCodeMap.insert( QLatin1String("en-SG"), i18nc("@info: country code", "English (Singapore)") );
        languageCodeMap.insert( QLatin1String("en-TT"), i18nc("@info: country code", "English (Trinidad)") );
        languageCodeMap.insert( QLatin1String("en-US"), i18nc("@info: country code", "English (United States)") );
        languageCodeMap.insert( QLatin1String("en-ZA"), i18nc("@info: country code", "English (South Africa)") );
        languageCodeMap.insert( QLatin1String("en-ZW"), i18nc("@info: country code", "English (Zimbabwe)") );
        languageCodeMap.insert( QLatin1String("es-AR"), i18nc("@info: country code", "Spanish (Argentina)") );
        languageCodeMap.insert( QLatin1String("es-BO"), i18nc("@info: country code", "Spanish (Bolivia)") );
        languageCodeMap.insert( QLatin1String("es-CL"), i18nc("@info: country code", "Spanish (Chile)") );
        languageCodeMap.insert( QLatin1String("es-CO"), i18nc("@info: country code", "Spanish (Colombia)") );
        languageCodeMap.insert( QLatin1String("es-CR"), i18nc("@info: country code", "Spanish (Costa Rica)") );
        languageCodeMap.insert( QLatin1String("es-DO"), i18nc("@info: country code", "Spanish (Dominican Republic)") );
        languageCodeMap.insert( QLatin1String("es-EC"), i18nc("@info: country code", "Spanish (Ecuador)") );
        languageCodeMap.insert( QLatin1String("es-ES"), i18nc("@info: country code", "Spanish (Spain)") );
        languageCodeMap.insert( QLatin1String("es-GT"), i18nc("@info: country code", "Spanish (Guatemala)") );
        languageCodeMap.insert( QLatin1String("es-HN"), i18nc("@info: country code", "Spanish (Honduras)") );
        languageCodeMap.insert( QLatin1String("es-MX"), i18nc("@info: country code", "Spanish (Mexico)") );
        languageCodeMap.insert( QLatin1String("es-NI"), i18nc("@info: country code", "Spanish (Nicaragua)") );
        languageCodeMap.insert( QLatin1String("es-PA"), i18nc("@info: country code", "Spanish (Panama)") );
        languageCodeMap.insert( QLatin1String("es-PE"), i18nc("@info: country code", "Spanish (Peru)") );
        languageCodeMap.insert( QLatin1String("es-PR"), i18nc("@info: country code", "Spanish (Puerto Rico)") );
        languageCodeMap.insert( QLatin1String("es-PY"), i18nc("@info: country code", "Spanish (Paraguay)") );
        languageCodeMap.insert( QLatin1String("es-SV"), i18nc("@info: country code", "Spanish (El Salvador)") );
        languageCodeMap.insert( QLatin1String("es-UR"), i18nc("@info: country code", "Spanish (Uruguay)") );
        languageCodeMap.insert( QLatin1String("es-US"), i18nc("@info: country code", "Spanish (United States)") );
        languageCodeMap.insert( QLatin1String("es-VE"), i18nc("@info: country code", "Spanish (Venezuela)") );
        languageCodeMap.insert( QLatin1String("et-EE"), i18nc("@info: country code", "Estonian (Estonia)") );
        languageCodeMap.insert( QLatin1String("eu-ES"), i18nc("@info: country code", "Basque (Basque Country)") );
        languageCodeMap.insert( QLatin1String("fa-IR"), i18nc("@info: country code", "Persian (Iran)") );
        languageCodeMap.insert( QLatin1String("fi-FI"), i18nc("@info: country code", "Finnish (Finland)") );
        languageCodeMap.insert( QLatin1String("fo-FO"), i18nc("@info: country code", "Faeroese (Faero Islands)") );
        languageCodeMap.insert( QLatin1String("fr-BE"), i18nc("@info: country code", "French (Belgium)") );
        languageCodeMap.insert( QLatin1String("fr-CA"), i18nc("@info: country code", "French (Canada)") );
        languageCodeMap.insert( QLatin1String("fr-CH"), i18nc("@info: country code", "French (Switzerland)") );
        languageCodeMap.insert( QLatin1String("fr-FR"), i18nc("@info: country code", "French (France)") );
        languageCodeMap.insert( QLatin1String("fr-LU"), i18nc("@info: country code", "French (Luxembourg)") );
        languageCodeMap.insert( QLatin1String("fr-MC"), i18nc("@info: country code", "French (Monaco)") );
        languageCodeMap.insert( QLatin1String("fy-NL"), i18nc("@info: country code", "Frisian (Netherlands)") );
        languageCodeMap.insert( QLatin1String("ga-IE"), i18nc("@info: country code", "Irish (Ireland)") );
        languageCodeMap.insert( QLatin1String("gl-ES"), i18nc("@info: country code", "Galician (Galicia)") );
        languageCodeMap.insert( QLatin1String("gu-IN"), i18nc("@info: country code", "Gujarati (India)") );
        languageCodeMap.insert( QLatin1String("he-IL"), i18nc("@info: country code", "Hebrew (Israel)") );
        languageCodeMap.insert( QLatin1String("hi-IN"), i18nc("@info: country code", "Hindi (India)") );
        languageCodeMap.insert( QLatin1String("hr-BA"), i18nc("@info: country code", "Croatian (Bosnia and Herzegovina, Latin)") );
        languageCodeMap.insert( QLatin1String("hr-HR"), i18nc("@info: country code", "Croatian (Croatia)") );
        languageCodeMap.insert( QLatin1String("hu-HU"), i18nc("@info: country code", "Hungarian (Hungary)") );
        languageCodeMap.insert( QLatin1String("hy-AM"), i18nc("@info: country code", "Armenian (Armenia)") );
        languageCodeMap.insert( QLatin1String("id-ID"), i18nc("@info: country code", "(Indonesian)") );
        languageCodeMap.insert( QLatin1String("ii-CN"), i18nc("@info: country code", "Yi (PRC)") );
        languageCodeMap.insert( QLatin1String("is-IS"), i18nc("@info: country code", "Icelandic (Iceland)") );
        languageCodeMap.insert( QLatin1String("it-CH"), i18nc("@info: country code", "Italian (Switzerland)") );
        languageCodeMap.insert( QLatin1String("it-IT"), i18nc("@info: country code", "Italian (Italy)") );
        languageCodeMap.insert( QLatin1String("ja-JP"), i18nc("@info: country code", "Japanese (Japan)") );
        languageCodeMap.insert( QLatin1String("ka-GE"), i18nc("@info: country code", "Georgian (Georgia)") );
        languageCodeMap.insert( QLatin1String("kk-KZ"), i18nc("@info: country code", "Kazakh (Kazakhstan)") );
        languageCodeMap.insert( QLatin1String("kl-GL"), i18nc("@info: country code", "Greenlandic (Greenland)") );
        languageCodeMap.insert( QLatin1String("km-KH"), i18nc("@info: country code", "Khmer (Cambodia)") );
        languageCodeMap.insert( QLatin1String("kn-IN"), i18nc("@info: country code", "Kannada (India)") );
        languageCodeMap.insert( QLatin1String("ko-KR"), i18nc("@info: country code", "Korean (South Korea)") );
        languageCodeMap.insert( QLatin1String("ky-KG"), i18nc("@info: country code", "Kyrgyz (Kyrgyzstan)") );
        languageCodeMap.insert( QLatin1String("lb-LU"), i18nc("@info: country code", "Luxembourgish (Luxembourg)") );
        languageCodeMap.insert( QLatin1String("lo-LA"), i18nc("@info: country code", "Lao (Lao PDR)") );
        languageCodeMap.insert( QLatin1String("lt-LT"), i18nc("@info: country code", "Lithuanian (Lithuania)") );
        languageCodeMap.insert( QLatin1String("lv-LV"), i18nc("@info: country code", "Latvian (Latvia)") );
        languageCodeMap.insert( QLatin1String("mi-NZ"), i18nc("@info: country code", "Maori (New Zealand)") );
        languageCodeMap.insert( QLatin1String("mk-MK"), i18nc("@info: country code", "Macedonian (Macedonia)") );
        languageCodeMap.insert( QLatin1String("ml-IN"), i18nc("@info: country code", "Malayalam (India)") );
        languageCodeMap.insert( QLatin1String("mn-CN"), i18nc("@info: country code", "Mongolian (PRC)") );
        languageCodeMap.insert( QLatin1String("mn-MN"), i18nc("@info: country code", "Mongolian (Mongolia)") );
        languageCodeMap.insert( QLatin1String("mr-IN"), i18nc("@info: country code", "Marathi (India)") );
        languageCodeMap.insert( QLatin1String("ms-BN"), i18nc("@info: country code", "Malay (Brunei Darussalam)") );
        languageCodeMap.insert( QLatin1String("ms-MY"), i18nc("@info: country code", "Malay (Malaysia)") );
        languageCodeMap.insert( QLatin1String("mt-MT"), i18nc("@info: country code", "Maltese (Malta)") );
        languageCodeMap.insert( QLatin1String("nb-NO"), i18nc("@info: country code", "Norwegian Bokmål (Norway)") );
        languageCodeMap.insert( QLatin1String("ne-NP"), i18nc("@info: country code", "Nepali (Nepal)") );
        languageCodeMap.insert( QLatin1String("nl-BE"), i18nc("@info: country code", "Dutch (Belgium)") );
        languageCodeMap.insert( QLatin1String("nl-NL"), i18nc("@info: country code", "Dutch (Netherlands)") );
        languageCodeMap.insert( QLatin1String("nn-NO"), i18nc("@info: country code", "Norwegian Nynorsk (Norway)") );
        languageCodeMap.insert( QLatin1String("ns-ZA"), i18nc("@info: country code", "Sesotho sa Leboa (South Africa)") );
        languageCodeMap.insert( QLatin1String("oc-FR"), i18nc("@info: country code", "Occitan (France)") );
        languageCodeMap.insert( QLatin1String("or-IN"), i18nc("@info: country code", "Oriya (India)") );
        languageCodeMap.insert( QLatin1String("pa-IN"), i18nc("@info: country code", "Punjabi (India)") );
        languageCodeMap.insert( QLatin1String("pl-PL"), i18nc("@info: country code", "Polish (Poland)") );
        languageCodeMap.insert( QLatin1String("ps-AF"), i18nc("@info: country code", "Pashto (Afghanistan)") );
        languageCodeMap.insert( QLatin1String("pt-BR"), i18nc("@info: country code", "Portuguese (Brazil)") );
        languageCodeMap.insert( QLatin1String("pt-PT"), i18nc("@info: country code", "Portuguese (Portugal)") );
        languageCodeMap.insert( QLatin1String("rm-CH"), i18nc("@info: country code", "Romansh (Switzerland)") );
        languageCodeMap.insert( QLatin1String("ro-RO"), i18nc("@info: country code", "Romanian (Romania)") );
        languageCodeMap.insert( QLatin1String("ru-RU"), i18nc("@info: country code", "Russian (Russia)") );
        languageCodeMap.insert( QLatin1String("rw-RW"), i18nc("@info: country code", "Kinyarwanda (Rwanda)") );
        languageCodeMap.insert( QLatin1String("sa-IN"), i18nc("@info: country code", "Sanskrit (India)") );
        languageCodeMap.insert( QLatin1String("se-FI"), i18nc("@info: country code", "Sami (Northern, Finland)") );
        languageCodeMap.insert( QLatin1String("se-NO"), i18nc("@info: country code", "Sami (Northern, Norway)") );
        languageCodeMap.insert( QLatin1String("se-SE"), i18nc("@info: country code", "Sami (Northern, Sweden)") );
        languageCodeMap.insert( QLatin1String("si-LK"), i18nc("@info: country code", "Sinhala (Sri Lanka)") );
        languageCodeMap.insert( QLatin1String("sk-SK"), i18nc("@info: country code", "Slovak (Slovakia)") );
        languageCodeMap.insert( QLatin1String("sl-SI"), i18nc("@info: country code", "Slovenian (Slovenia)") );
        languageCodeMap.insert( QLatin1String("sq-AL"), i18nc("@info: country code", "Albanian (Albania)") );
        languageCodeMap.insert( QLatin1String("sv-FI"), i18nc("@info: country code", "Swedish (Finland)") );
        languageCodeMap.insert( QLatin1String("sv-SE"), i18nc("@info: country code", "Swedish (Sweden)") );
        languageCodeMap.insert( QLatin1String("sw-KE"), i18nc("@info: country code", "Swahili (Kenya)") );
        languageCodeMap.insert( QLatin1String("ta-IN"), i18nc("@info: country code", "Tamil (India)") );
        languageCodeMap.insert( QLatin1String("te-IN"), i18nc("@info: country code", "Telugu (India)") );
        languageCodeMap.insert( QLatin1String("th-TH"), i18nc("@info: country code", "Thai (Thailand)") );
        languageCodeMap.insert( QLatin1String("tk-TM"), i18nc("@info: country code", "Turkmen (Turkmenistan)") );
        languageCodeMap.insert( QLatin1String("tn-ZA"), i18nc("@info: country code", "Setswana Tswana (South Africa)") );
        languageCodeMap.insert( QLatin1String("tr-IN"), i18nc("@info: country code", "Urdu (India)") );
        languageCodeMap.insert( QLatin1String("tr-TR"), i18nc("@info: country code", "Turkish (Turkey)") );
        languageCodeMap.insert( QLatin1String("tt-RU"), i18nc("@info: country code", "Tatar (Russia)") );
        languageCodeMap.insert( QLatin1String("ug-CN"), i18nc("@info: country code", "Uighur (PRC)") );
        languageCodeMap.insert( QLatin1String("uk-UA"), i18nc("@info: country code", "Ukrainian (Ukraine)") );
        languageCodeMap.insert( QLatin1String("ur-PK"), i18nc("@info: country code", "Urdu (Pakistan)") );
        languageCodeMap.insert( QLatin1String("vi-VN"), i18nc("@info: country code", "Vietnamese (Vietnam)") );
        languageCodeMap.insert( QLatin1String("wo-SN"), i18nc("@info: country code", "Wolof (Senegal)") );
        languageCodeMap.insert( QLatin1String("xh-ZA"), i18nc("@info: country code", "isiXhosa Xhosa (South Africa)") );
        languageCodeMap.insert( QLatin1String("yo-NG"), i18nc("@info: country code", "Yoruba (Nigeria)") );
        languageCodeMap.insert( QLatin1String("zh-CN"), i18nc("@info: country code", "Chinese (PRC)") );
        languageCodeMap.insert( QLatin1String("zh-HK"), i18nc("@info: country code", "Chinese (Hong Kong SAR, PRC)") );
        languageCodeMap.insert( QLatin1String("zh-MO"), i18nc("@info: country code", "Chinese (Macao SAR)") );
        languageCodeMap.insert( QLatin1String("zh-SG"), i18nc("@info: country code", "Chinese (Singapore)") );
        languageCodeMap.insert( QLatin1String("zh-TW"), i18nc("@info: country code", "Chinese (Taiwan)") );
        languageCodeMap.insert( QLatin1String("zu-ZA"), i18nc("@info: country code", "isiZulu Zulu (South Africa)") );
    }

    ~Private()
    {
        languageCodeMap.clear();
    }

public:

    typedef QMap<QString, QString> LanguageCodeMap;

    LanguageCodeMap                languageCodeMap;

    QString                        currentLanguage;

    uint                           linesVisible;

    QLabel*                        titleLabel;

    QToolButton*                   delValueButton;

    QTextEdit*                     valueEdit;

    QComboBox*                     languageCB;

    MetaEngine::AltLangMap         values;
};

AltLangStrEdit::AltLangStrEdit(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    QGridLayout* const grid = new QGridLayout(this);
    d->titleLabel           = new QLabel(this);
    d->delValueButton       = new QToolButton(this);
    d->delValueButton->setIcon(QIcon::fromTheme(QLatin1String("edit-clear")));
    d->delValueButton->setToolTip(i18nc("@info: language edit dialog", "Remove entry for this language"));
    d->delValueButton->setEnabled(false);

    d->languageCB = new QComboBox(this);
    d->languageCB->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    d->languageCB->setWhatsThis(i18nc("@info: language edit dialog", "Select item language here."));

    d->valueEdit  = new QTextEdit(this);
    d->valueEdit->setAcceptRichText(false);

    // --------------------------------------------------------

    grid->setAlignment( Qt::AlignTop );
    grid->addWidget(d->titleLabel,     0, 0, 1, 1);
    grid->addWidget(d->languageCB,     0, 2, 1, 1);
    grid->addWidget(d->delValueButton, 0, 3, 1, 1);
    grid->addWidget(d->valueEdit,      1, 0, 1,-1);
    grid->setColumnStretch(1, 10);
    grid->setContentsMargins(QMargins());
    grid->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    loadLangAltListEntries();

    // --------------------------------------------------------

    connect(d->languageCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &AltLangStrEdit::slotSelectionChanged);

    connect(d->delValueButton, &QToolButton::clicked,
            this, &AltLangStrEdit::slotDeleteValue);

    connect(d->valueEdit, &QTextEdit::textChanged,
            this, &AltLangStrEdit::slotTextChanged);
}

AltLangStrEdit::~AltLangStrEdit()
{
    delete d;
}

QString AltLangStrEdit::currentLanguageCode() const
{
    return d->currentLanguage;
}

void AltLangStrEdit::setCurrentLanguageCode(const QString& lang)
{
    if (d->currentLanguage.isEmpty())
    {
        d->currentLanguage = QLatin1String("x-default");
    }
    else
    {
        d->currentLanguage = lang;
    }
}

QString AltLangStrEdit::languageCode(int index) const
{
    return d->languageCB->itemText(index);
}

void AltLangStrEdit::setTitle(const QString& title)
{
    d->titleLabel->setText(title);
}

void AltLangStrEdit::setPlaceholderText(const QString& msg)
{
    d->valueEdit->setPlaceholderText(msg);
}

void AltLangStrEdit::reset()
{
    setValues(MetaEngine::AltLangMap());
}

void AltLangStrEdit::slotDeleteValue()
{
    d->values.remove(d->currentLanguage);
    setValues(d->values);
    emit signalValueDeleted(d->currentLanguage);
}

void AltLangStrEdit::slotSelectionChanged()
{
    d->currentLanguage = d->languageCB->currentText();

    // There are bogus signals caused by spell checking, see bug #141663.
    // so we must block signals here.

    d->valueEdit->blockSignals(true);

    QString text = d->values.value(d->currentLanguage);
    d->valueEdit->setPlainText(text);
    d->delValueButton->setEnabled(!text.isNull());

    d->valueEdit->blockSignals(false);

    d->languageCB->setToolTip(d->languageCodeMap.value(d->currentLanguage));

    emit signalSelectionChanged(d->currentLanguage);
}

void AltLangStrEdit::setValues(const MetaEngine::AltLangMap& values)
{
    d->values    = values;
    loadLangAltListEntries();

    d->valueEdit->blockSignals(true);

    QString text = d->values.value(d->currentLanguage);
    d->valueEdit->setPlainText(text);
    d->delValueButton->setEnabled(!text.isNull());

    d->valueEdit->blockSignals(false);
}

MetaEngine::AltLangMap& AltLangStrEdit::values() const
{
    return d->values;
}

void AltLangStrEdit::loadLangAltListEntries()
{
    d->languageCB->blockSignals(true);

    d->languageCB->clear();

    // In first we fill already assigned languages.

    QStringList list = d->values.keys();

    if (!list.isEmpty())
    {
        foreach (const QString& item, list)
        {
              d->languageCB->addItem(item);
              d->languageCB->setItemIcon(d->languageCB->count()-1, QIcon::fromTheme(QLatin1String("dialog-ok-apply")).pixmap(16, 16));
        }

        d->languageCB->insertSeparator(d->languageCB->count());
    }

    // ...and now, all the rest...

    for (Private::LanguageCodeMap::Iterator it = d->languageCodeMap.begin() ;
         it != d->languageCodeMap.end() ; ++it)
    {
        if (!list.contains(it.key()))
        {
            d->languageCB->addItem(it.key());
        }
    }

    d->languageCB->setCurrentIndex(d->languageCB->findText(d->currentLanguage));

    d->languageCB->blockSignals(false);
}

QString AltLangStrEdit::defaultAltLang() const
{
    return d->values.value(QLatin1String("x-default"));
}

bool AltLangStrEdit::asDefaultAltLang() const
{
    return !defaultAltLang().isNull();
}

void AltLangStrEdit::slotTextChanged()
{
    QString editedText   = d->valueEdit->toPlainText();
    QString previousText = d->values.value(d->currentLanguage);

    if      (editedText.isEmpty())
    {
        slotDeleteValue();
    }
    else if (previousText.isNull())
    {
        addCurrent();
    }
    else if (editedText != previousText)
    {
        // we cannot trust that the text actually changed
        // (there are bogus signals caused by spell checking, see bug #141663)
        // so we have to check before marking the metadata as modified.

        d->values.insert(d->currentLanguage, editedText);
        emit signalModified(d->currentLanguage, editedText);
    }
}

void AltLangStrEdit::addCurrent()
{
    QString text = d->valueEdit->toPlainText();

    d->values.insert(d->currentLanguage, text);
    loadLangAltListEntries();
    d->delValueButton->setEnabled(true);
    emit signalValueAdded(d->currentLanguage, text);
}

void AltLangStrEdit::setLinesVisible(uint lines)
{
    d->linesVisible = lines;

    if (d->linesVisible == 0)
    {
        d->valueEdit->setFixedHeight(QWIDGETSIZE_MAX); // reset
    }
    else
    {
        d->valueEdit->setFixedHeight(d->valueEdit->fontMetrics().lineSpacing() * d->linesVisible         +
                                     d->valueEdit->contentsMargins().top()                               +
                                     d->valueEdit->contentsMargins().bottom()                            +
                                     1                                                                   +
                                     2*(d->valueEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth) +
                                        d->valueEdit->style()->pixelMetric(QStyle::PM_FocusFrameVMargin))
                                    );
    }

    // It's not possible to display scrollbar properly if size is too small

    if (d->linesVisible < 3)
    {
        d->valueEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

uint AltLangStrEdit::linesVisible() const
{
    return d->linesVisible;
}

void AltLangStrEdit::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::FontChange)
    {
        setLinesVisible(linesVisible());
    }

    QWidget::changeEvent(e);
}

QTextEdit* AltLangStrEdit::textEdit() const
{
    return d->valueEdit;
}

} // namespace Digikam
