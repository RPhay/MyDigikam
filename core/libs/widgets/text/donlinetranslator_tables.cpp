/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services - Conversion tables.
 *
 * Copyright (C) 2018-2022 by Hennadii Chernyshchyk <genaloner at gmail dot com>
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

#include "donlinetranslator.h"

// Qt includes

#include <QCoreApplication>
#include <QFinalState>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QStateMachine>

// KDE includes

#include <klocalizedstring.h>

namespace Digikam
{

QString DOnlineTranslator::languageName(Language lang)
{
    switch (lang)
    {
        case Auto:
            return i18n("Automatically detect");

        case Afrikaans:
            return i18n("Afrikaans");

        case Albanian:
            return i18n("Albanian");

        case Amharic:
            return i18n("Amharic");

        case Arabic:
            return i18n("Arabic");

        case Armenian:
            return i18n("Armenian");

        case Azerbaijani:
            return i18n("Azeerbaijani");

        case Basque:
            return i18n("Basque");

        case Bashkir:
            return i18n("Bashkir");

        case Belarusian:
            return i18n("Belarusian");

        case Bengali:
            return i18n("Bengali");

        case Bosnian:
            return i18n("Bosnian");

        case Bulgarian:
            return i18n("Bulgarian");

        case Catalan:
            return i18n("Catalan");

        case Cantonese:
            return i18n("Cantonese");

        case Cebuano:
            return i18n("Cebuano");

        case SimplifiedChinese:
            return i18n("Chinese (Simplified)");

        case TraditionalChinese:
            return i18n("Chinese (Traditional)");

        case Corsican:
            return i18n("Corsican");

        case Croatian:
            return i18n("Croatian");

        case Czech:
            return i18n("Czech");

        case Danish:
            return i18n("Danish");

        case Dutch:
            return i18n("Dutch");

        case English:
            return i18n("English");

        case Esperanto:
            return i18n("Esperanto");

        case Estonian:
            return i18n("Estonian");

        case Fijian:
            return i18n("Fijian");

        case Filipino:
            return i18n("Filipino");

        case Finnish:
            return i18n("Finnish");

        case French:
            return i18n("French");

        case Frisian:
            return i18n("Frisian");

        case Galician:
            return i18n("Galician");

        case Georgian:
            return i18n("Georgian");

        case German:
            return i18n("German");

        case Greek:
            return i18n("Greek");

        case Gujarati:
            return i18n("Gujarati");

        case HaitianCreole:
            return i18n("Haitian Creole");

        case Hausa:
            return i18n("Hausa");

        case Hawaiian:
            return i18n("Hawaiian");

        case Hebrew:
            return i18n("Hebrew");

        case HillMari:
            return i18n("Hill Mari");

        case Hindi:
            return i18n("Hindi");

        case Hmong:
            return i18n("Hmong");

        case Hungarian:
            return i18n("Hungarian");

        case Icelandic:
            return i18n("Icelandic");

        case Igbo:
            return i18n("Igbo");

        case Indonesian:
            return i18n("Indonesian");

        case Irish:
            return i18n("Irish");

        case Italian:
            return i18n("Italian");

        case Japanese:
            return i18n("Japanese");

        case Javanese:
            return i18n("Javanese");

        case Kannada:
            return i18n("Kannada");

        case Kazakh:
            return i18n("Kazakh");

        case Khmer:
            return i18n("Khmer");

        case Kinyarwanda:
            return i18n("Kinyarwanda");

        case Klingon:
            return i18n("Klingon");

        case KlingonPlqaD:
            return i18n("Klingon (PlqaD)");

        case Korean:
            return i18n("Korean");

        case Kurdish:
            return i18n("Kurdish");

        case Kyrgyz:
            return i18n("Kyrgyz");

        case Lao:
            return i18n("Lao");

        case Latin:
            return i18n("Latin");

        case Latvian:
            return i18n("Latvian");

        case LevantineArabic:
            return i18n("Levantine Arabic");

        case Lithuanian:
            return i18n("Lithuanian");

        case Luxembourgish:
            return i18n("Luxembourgish");

        case Macedonian:
            return i18n("Macedonian");

        case Malagasy:
            return i18n("Malagasy");

        case Malay:
            return i18n("Malay");

        case Malayalam:
            return i18n("Malayalam");

        case Maltese:
            return i18n("Maltese");

        case Maori:
            return i18n("Maori");

        case Marathi:
            return i18n("Marathi");

        case Mari:
            return i18n("Mari");

        case Mongolian:
            return i18n("Mongolian");

        case Myanmar:
            return i18n("Myanmar");

        case Nepali:
            return i18n("Nepali");

        case Norwegian:
            return i18n("Norwegian");

        case Oriya:
            return i18n("Oriya");

        case Chichewa:
            return i18n("Chichewa");

        case Papiamento:
            return i18n("Papiamento");

        case Pashto:
            return i18n("Pashto");

        case Persian:
            return i18n("Persian");

        case Polish:
            return i18n("Polish");

        case Portuguese:
            return i18n("Portuguese");

        case Punjabi:
            return i18n("Punjabi");

        case QueretaroOtomi:
            return i18n("Queretaro Otomi");

        case Romanian:
            return i18n("Romanian");

        case Russian:
            return i18n("Russian");

        case Samoan:
            return i18n("Samoan");

        case ScotsGaelic:
            return i18n("Scots Gaelic");

        case SerbianCyrillic:
            return i18n("Serbian (Cyrillic)");

        case SerbianLatin:
            return i18n("Serbian (Latin)");

        case Sesotho:
            return i18n("Sesotho");

        case Shona:
            return i18n("Shona");

        case Sindhi:
            return i18n("Sindhi");

        case Sinhala:
            return i18n("Sinhala");

        case Slovak:
            return i18n("Slovak");

        case Slovenian:
            return i18n("Slovenian");

        case Somali:
            return i18n("Somali");

        case Spanish:
            return i18n("Spanish");

        case Sundanese:
            return i18n("Sundanese");

        case Swahili:
            return i18n("Swahili");

        case Swedish:
            return i18n("Swedish");

        case Tagalog:
            return i18n("Tagalog");

        case Tahitian:
            return i18n("Tahitian");

        case Tajik:
            return i18n("Tajik");

        case Tamil:
            return i18n("Tamil");

        case Tatar:
            return i18n("Tatar");

        case Telugu:
            return i18n("Telugu");

        case Thai:
            return i18n("Thai");

        case Tongan:
            return i18n("Tongan");

        case Turkish:
            return i18n("Turkish");

        case Turkmen:
            return i18n("Turkmen");

        case Udmurt:
            return i18n("Udmurt");

        case Uighur:
            return i18n("Uighur");

        case Ukrainian:
            return i18n("Ukrainian");

        case Urdu:
            return i18n("Urdu");

        case Uzbek:
            return i18n("Uzbek");

        case Vietnamese:
            return i18n("Vietnamese");

        case Welsh:
            return i18n("Welsh");

        case Xhosa:
            return i18n("Xhosa");

        case Yiddish:
            return i18n("Yiddish");

        case Yoruba:
            return i18n("Yoruba");

        case YucatecMaya:
            return i18n("Yucatec Maya");

        case Zulu:
            return i18n("Zulu");

        default:
            return QString();
    }
}

DOnlineTranslator::Language DOnlineTranslator::language(const QLocale& locale)
{
    switch (locale.language())
    {
        case QLocale::Afrikaans:
            return Afrikaans;

        case QLocale::Albanian:
            return Albanian;

        case QLocale::Amharic:
            return Amharic;

        case QLocale::Arabic:
            return Arabic;

        case QLocale::Armenian:
            return Armenian;

        case QLocale::Azerbaijani:
            return Azerbaijani;

        case QLocale::Basque:
            return Basque;

        case QLocale::Belarusian:
            return Belarusian;

        case QLocale::Bengali:
            return Bengali;

        case QLocale::Bosnian:
            return Bosnian;

        case QLocale::Bulgarian:
            return Bulgarian;

        case QLocale::Catalan:
            return Catalan;

        case QLocale::Chinese:
            return SimplifiedChinese;

        case QLocale::LiteraryChinese:
            return TraditionalChinese;

        case QLocale::Corsican:
            return Corsican;

        case QLocale::Croatian:
            return Croatian;

        case QLocale::Czech:
            return Czech;

        case QLocale::Danish:
            return Danish;

        case QLocale::Dutch:
            return Dutch;

        case QLocale::Esperanto:
            return Esperanto;

        case QLocale::Estonian:
            return Estonian;

        case QLocale::Finnish:
            return Finnish;

        case QLocale::French:
            return French;

        case QLocale::Frisian:
            return Frisian;

        case QLocale::Galician:
            return Galician;

        case QLocale::Georgian:
            return Georgian;

        case QLocale::German:
            return German;

        case QLocale::Greek:
            return Greek;

        case QLocale::Gujarati:
            return Gujarati;

        case QLocale::Haitian:
            return HaitianCreole;

        case QLocale::Hausa:
            return Hausa;

        case QLocale::Hawaiian:
            return Hawaiian;

        case QLocale::Hebrew:
            return Hebrew;

        case QLocale::Hindi:
            return Hindi;

        case QLocale::Hungarian:
            return Hungarian;

        case QLocale::Icelandic:
            return Icelandic;

        case QLocale::Igbo:
            return Igbo;

        case QLocale::Indonesian:
            return Indonesian;

        case QLocale::Irish:
            return Irish;

        case QLocale::Italian:
            return Italian;

        case QLocale::Japanese:
            return Japanese;

        case QLocale::Javanese:
            return Javanese;

        case QLocale::Kannada:
            return Kannada;

        case QLocale::Kazakh:
            return Kazakh;

        case QLocale::Khmer:
            return Khmer;

        case QLocale::Kinyarwanda:
            return Kinyarwanda;

        case QLocale::Korean:
            return Korean;

        case QLocale::Kurdish:
            return Kurdish;

        case QLocale::Lao:
            return Lao;

        case QLocale::Latin:
            return Latin;

        case QLocale::Latvian:
            return Latvian;

        case QLocale::Lithuanian:
            return Lithuanian;

        case QLocale::Luxembourgish:
            return Luxembourgish;

        case QLocale::Macedonian:
            return Macedonian;

        case QLocale::Malagasy:
            return Malagasy;

        case QLocale::Malay:
            return Malay;

        case QLocale::Malayalam:
            return Malayalam;

        case QLocale::Maltese:
            return Maltese;

        case QLocale::Maori:
            return Maori;

        case QLocale::Marathi:
            return Marathi;

        case QLocale::Mongolian:
            return Mongolian;

        case QLocale::Nepali:
            return Nepali;

        case QLocale::NorwegianBokmal:
            return Norwegian;

        case QLocale::Oriya:
            return Oriya;

        case QLocale::Pashto:
            return Pashto;

        case QLocale::Persian:
            return Persian;

        case QLocale::Polish:
            return Polish;

        case QLocale::Portuguese:
            return Portuguese;

        case QLocale::Punjabi:
            return Punjabi;

        case QLocale::Romanian:
            return Romanian;

        case QLocale::Russian:
            return Russian;

        case QLocale::Samoan:
            return Samoan;

        case QLocale::Gaelic:
            return ScotsGaelic;

        case QLocale::Serbian:
            return SerbianCyrillic;

        case QLocale::Shona:
            return Shona;

        case QLocale::Sindhi:
            return Sindhi;

        case QLocale::Sinhala:
            return Sinhala;

        case QLocale::Slovak:
            return Slovak;

        case QLocale::Slovenian:
            return Slovenian;

        case QLocale::Somali:
            return Somali;

        case QLocale::Spanish:
            return Spanish;

        case QLocale::Sundanese:
            return Sundanese;

        case QLocale::Swahili:
            return Swahili;

        case QLocale::Swedish:
            return Swedish;

        case QLocale::Filipino:
            return Filipino;

        case QLocale::Tajik:
            return Tajik;

        case QLocale::Tamil:
            return Tamil;

        case QLocale::Tatar:
            return Tatar;

        case QLocale::Telugu:
            return Telugu;

        case QLocale::Thai:
            return Thai;

        case QLocale::Turkish:
            return Turkish;

        case QLocale::Turkmen:
            return Turkmen;

        case QLocale::Uighur:
            return Uighur;

        case QLocale::Ukrainian:
            return Ukrainian;

        case QLocale::Urdu:
            return Urdu;

        case QLocale::Uzbek:
            return Uzbek;

        case QLocale::Vietnamese:
            return Vietnamese;

        case QLocale::Welsh:
            return Welsh;

        case QLocale::Xhosa:
            return Xhosa;

        case QLocale::Yiddish:
            return Yiddish;

        case QLocale::Yoruba:
            return Yoruba;

        case QLocale::Zulu:
            return Zulu;

        default:
            return English;
    }
}

bool DOnlineTranslator::isSupportTranslation(Engine engine, Language lang)
{
    bool isSupported = false;

    switch (engine)
    {
        case Google:
        case Lingva: // Lingva is a frontend to Google Translate

            switch (lang)
            {
                case NoLanguage:
                case Bashkir:
                case Cantonese:
                case Fijian:
                case Filipino:
                case Georgian:
                case HillMari:
                case Klingon:
                case KlingonPlqaD:
                case LevantineArabic:
                case Mari:
                case Papiamento:
                case QueretaroOtomi:
                case SerbianLatin:
                case Tahitian:
                case Tongan:
                case Udmurt:
                case YucatecMaya:
                    isSupported = false;
                    break;

                default:
                    isSupported = true;
                    break;
            }

            break;

        case Yandex:

            switch (lang)
            {
                case NoLanguage:
                case Cantonese:
                case Chichewa:
                case Corsican:
                case Fijian:
                case Filipino:
                case Frisian:
                case Hausa:
                case Hawaiian:
                case Igbo:
                case Kinyarwanda:
                case Klingon:
                case KlingonPlqaD:
                case Kurdish:
                case LevantineArabic:
                case Oriya:
                case Pashto:
                case QueretaroOtomi:
                case Samoan:
                case SerbianLatin:
                case Sesotho:
                case Shona:
                case Sindhi:
                case Somali:
                case Tahitian:
                case Tongan:
                case Turkmen:
                case Uighur:
                case Yoruba:
                case YucatecMaya:
                case Zulu:
                    isSupported = false;
                    break;

                default:
                    isSupported = true;
                    break;
                }

                break;

        case Bing:

            switch (lang)
            {
                case NoLanguage:
                case Albanian:
                case Amharic:
                case Armenian:
                case Azerbaijani:
                case Basque:
                case Bashkir:
                case Belarusian:
                case Cebuano:
                case Corsican:
                case Esperanto:
                case Frisian:
                case Galician:
                case Georgian:
                case Gujarati:
                case Hausa:
                case Hawaiian:
                case HillMari:
                case Igbo:
                case Irish:
                case Javanese:
                case Kannada:
                case Kazakh:
                case Khmer:
                case Kinyarwanda:
                case Kurdish:
                case Kyrgyz:
                case Lao:
                case Latin:
                case Luxembourgish:
                case Macedonian:
                case Malayalam:
                case Maori:
                case Marathi:
                case Mari:
                case Mongolian:
                case Myanmar:
                case Nepali:
                case Oriya:
                case Chichewa:
                case Papiamento:
                case Pashto:
                case Punjabi:
                case ScotsGaelic:
                case Sesotho:
                case Shona:
                case Sindhi:
                case Sinhala:
                case Somali:
                case Sundanese:
                case Tagalog:
                case Tajik:
                case Tatar:
                case Turkmen:
                case Uighur:
                case Udmurt:
                case Uzbek:
                case Xhosa:
                case Yiddish:
                case Yoruba:
                case Zulu:
                    isSupported = false;
                    break;

                default:
                    isSupported = true;
                    break;
                }

                break;

        case LibreTranslate:

            switch (lang)
            {
                case NoLanguage:
                case Afrikaans:
                case Albanian:
                case Amharic:
                case Armenian:
                case Azerbaijani:
                case Bashkir:
                case Basque:
                case Belarusian:
                case Bengali:
                case Bosnian:
                case Bulgarian:
                case Cantonese:
                case Catalan:
                case Cebuano:
                case Chichewa:
                case Corsican:
                case Croatian:
                case Czech:
                case Danish:
                case Dutch:
                case Esperanto:
                case Estonian:
                case Fijian:
                case Filipino:
                case Finnish:
                case Frisian:
                case Galician:
                case Georgian:
                case Greek:
                case Gujarati:
                case HaitianCreole:
                case Hausa:
                case Hawaiian:
                case Hebrew:
                case HillMari:
                case Hmong:
                case Hungarian:
                case Icelandic:
                case Igbo:
                case Javanese:
                case Kannada:
                case Kazakh:
                case Khmer:
                case Kinyarwanda:
                case Klingon:
                case KlingonPlqaD:
                case Kurdish:
                case Kyrgyz:
                case Lao:
                case Latin:
                case Latvian:
                case LevantineArabic:
                case Lithuanian:
                case Luxembourgish:
                case Macedonian:
                case Malagasy:
                case Malay:
                case Malayalam:
                case Maltese:
                case Maori:
                case Marathi:
                case Mari:
                case Mongolian:
                case Myanmar:
                case Nepali:
                case Norwegian:
                case Oriya:
                case Papiamento:
                case Pashto:
                case Persian:
                case Punjabi:
                case QueretaroOtomi:
                case Romanian:
                case Samoan:
                case ScotsGaelic:
                case SerbianCyrillic:
                case SerbianLatin:
                case Sesotho:
                case Shona:
                case SimplifiedChinese:
                case Sindhi:
                case Sinhala:
                case Slovak:
                case Slovenian:
                case Somali:
                case Sundanese:
                case Swahili:
                case Swedish:
                case Tagalog:
                case Tahitian:
                case Tajik:
                case Tamil:
                case Tatar:
                case Telugu:
                case Thai:
                case Tongan:
                case Turkmen:
                case Udmurt:
                case Uighur:
                case Ukrainian:
                case Urdu:
                case Uzbek:
                case Welsh:
                case Xhosa:
                case Yiddish:
                case Yoruba:
                case YucatecMaya:
                case Zulu:
                    isSupported = false;
                    break;

                default:
                    isSupported = true;
                    break;
            }

            break;
    }

    return isSupported;
}

bool DOnlineTranslator::isSupportTranslit(Engine engine, Language lang)
{
    switch (engine)
    {
        case Google:
            isSupportTranslation(Google, lang); // Google supports transliteration for all supported languages
            break;

        case Yandex:

            switch (lang)
            {
                case Amharic:
                case Armenian:
                case Bengali:
                case SimplifiedChinese:
                case Georgian:
                case Greek:
                case Gujarati:
                case Hebrew:
                case Hindi:
                case Japanese:
                case Kannada:
                case Korean:
                case Malayalam:
                case Marathi:
                case Nepali:
                case Punjabi:
                case Russian:
                case Sinhala:
                case Tamil:
                case Telugu:
                case Thai:
                case Yiddish:
                    return true;
                default:
                    return false;
            }

        case Bing:

            switch (lang)
            {
                case Arabic:
                case Bengali:
                case Gujarati:
                case Hebrew:
                case Hindi:
                case Japanese:
                case Kannada:
                case Malayalam:
                case Marathi:
                case Punjabi:
                case SerbianCyrillic:
                case SerbianLatin:
                case Tamil:
                case Telugu:
                case Thai:
                case SimplifiedChinese:
                case TraditionalChinese:
                    return true;
                default:
                    return false;
            }

        case LibreTranslate:    // LibreTranslate doesn't support translit
        case Lingva:            // Although Lingvo is a frontend to Google Translate, it doesn't support transliteration
            return false;
    }

    return false;
}

bool DOnlineTranslator::isSupportDictionary(Engine engine, Language sourceLang, Language translationLang)
{
    switch (engine)
    {
        case Google:
            // Google supports dictionary for all supported languages
            return isSupportTranslation(Google, sourceLang) && isSupportTranslation(Google, translationLang);

        case Yandex:

            switch (sourceLang)
            {
                case Belarusian:

                    switch (translationLang)
                    {
                        case Belarusian:
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case Bulgarian:

                    switch (translationLang)
                    {
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case Czech:
                case Danish:
                case Dutch:
                case Estonian:
                case Greek:
                case Latvian:
                case Norwegian:
                case Portuguese:
                case Slovak:
                case Swedish:

                    switch (translationLang)
                    {
                        case English:
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case German:

                    switch (translationLang)
                    {
                        case German:
                        case English:
                        case Russian:
                        case Turkish:
                            return true;
                        default:
                            return false;
                    }

                case English:

                    switch (translationLang)
                    {
                        case Czech:
                        case Danish:
                        case German:
                        case Greek:
                        case English:
                        case Spanish:
                        case Estonian:
                        case Finnish:
                        case French:
                        case Italian:
                        case Lithuanian:
                        case Latvian:
                        case Dutch:
                        case Norwegian:
                        case Portuguese:
                        case Russian:
                        case Slovak:
                        case Swedish:
                        case Turkish:
                        case Ukrainian:
                            return true;
                        default:
                            return false;
                    }

                case Spanish:

                    switch (translationLang)
                    {
                        case English:
                        case Spanish:
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case Finnish:

                    switch (translationLang)
                    {
                        case English:
                        case Russian:
                        case Finnish:
                            return true;
                        default:
                            return false;
                    }
                case French:

                    switch (translationLang)
                    {
                        case French:
                        case English:
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case Hungarian:

                    switch (translationLang)
                    {
                        case Hungarian:
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case Italian:

                    switch (translationLang)
                    {
                        case English:
                        case Italian:
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case Lithuanian:
                    switch (translationLang)
                    {
                        case English:
                        case Lithuanian:
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case Mari:
                case HillMari:
                case Polish:
                case Tatar:

                    switch (translationLang)
                    {
                        case Russian:
                            return true;
                        default:
                            return false;
                    }

                case Russian:

                    switch (translationLang)
                    {
                        case Belarusian:
                        case Bulgarian:
                        case Czech:
                        case Danish:
                        case German:
                        case Greek:
                        case English:
                        case Spanish:
                        case Estonian:
                        case Finnish:
                        case French:
                        case Italian:
                        case Lithuanian:
                        case Latvian:
                        case Mari:
                        case HillMari:
                        case Dutch:
                        case Norwegian:
                        case Portuguese:
                        case Russian:
                        case Slovak:
                        case Swedish:
                        case Turkish:
                        case Tatar:
                        case Ukrainian:
                            return true;
                        default:
                            return false;
                    }

                    case Turkish:

                        switch (translationLang)
                        {
                            case German:
                            case English:
                            case Russian:
                                return true;
                            default:
                                return false;
                        }

                    case Ukrainian:

                        switch (translationLang)
                        {
                            case English:
                            case Russian:
                            case Ukrainian:
                                return true;
                            default:
                                return false;
                        }

                    default:
                        return false;
            }

            case Bing:

                // Bing support dictionary only to or from English

                Language secondLang;

                if      (sourceLang == English)
                    secondLang = translationLang;
                else if (translationLang == English)
                    secondLang = sourceLang;
                else
                    return false;

                switch (secondLang)
                {
                    case Afrikaans:
                    case Arabic:
                    case Bengali:
                    case Bosnian:
                    case Bulgarian:
                    case Catalan:
                    case SimplifiedChinese:
                    case Croatian:
                    case Czech:
                    case Danish:
                    case Dutch:
                    case Estonian:
                    case Finnish:
                    case French:
                    case German:
                    case Greek:
                    case HaitianCreole:
                    case Hebrew:
                    case Hindi:
                    case Hmong:
                    case Hungarian:
                    case Icelandic:
                    case Indonesian:
                    case Italian:
                    case Japanese:
                    case Swahili:
                    case Klingon:
                    case Korean:
                    case Latvian:
                    case Lithuanian:
                    case Malay:
                    case Maltese:
                    case Norwegian:
                    case Persian:
                    case Polish:
                    case Portuguese:
                    case Romanian:
                    case Russian:
                    case SerbianLatin:
                    case Slovak:
                    case Slovenian:
                    case Spanish:
                    case Swedish:
                    case Tamil:
                    case Thai:
                    case Turkish:
                    case Ukrainian:
                    case Urdu:
                    case Vietnamese:
                    case Welsh:
                        return true;
                    default:
                        return false;
                }

            case LibreTranslate: // LibreTranslate doesn't support dictinaries
            case Lingva:         // Although Lingvo is a frontend to Google Translate, it doesn't support dictionaries
                return false;
    }

    return false;
}

} // namespace Digikam
