/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-02-22
 * Description : A text translator using web-services.
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

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::s_genericLanguageCodes =
{
    { Auto,                 QStringLiteral("auto")          },
    { Afrikaans,            QStringLiteral("af")            },
    { Albanian,             QStringLiteral("sq")            },
    { Amharic,              QStringLiteral("am")            },
    { Arabic,               QStringLiteral("ar")            },
    { Armenian,             QStringLiteral("hy")            },
    { Azerbaijani,          QStringLiteral("az")            },
    { Bashkir,              QStringLiteral("ba")            },
    { Basque,               QStringLiteral("eu")            },
    { Belarusian,           QStringLiteral("be")            },
    { Bengali,              QStringLiteral("bn")            },
    { Bosnian,              QStringLiteral("bs")            },
    { Bulgarian,            QStringLiteral("bg")            },
    { Cantonese,            QStringLiteral("yue")           },
    { Catalan,              QStringLiteral("ca")            },
    { Cebuano,              QStringLiteral("ceb")           },
    { Chichewa,             QStringLiteral("ny")            },
    { Corsican,             QStringLiteral("co")            },
    { Croatian,             QStringLiteral("hr")            },
    { Czech,                QStringLiteral("cs")            },
    { Danish,               QStringLiteral("da")            },
    { Dutch,                QStringLiteral("nl")            },
    { English,              QStringLiteral("en")            },
    { Esperanto,            QStringLiteral("eo")            },
    { Estonian,             QStringLiteral("et")            },
    { Fijian,               QStringLiteral("fj")            },
    { Filipino,             QStringLiteral("fil")           },
    { Finnish,              QStringLiteral("fi")            },
    { French,               QStringLiteral("fr")            },
    { Frisian,              QStringLiteral("fy")            },
    { Galician,             QStringLiteral("gl")            },
    { Georgian,             QStringLiteral("ka")            },
    { German,               QStringLiteral("de")            },
    { Greek,                QStringLiteral("el")            },
    { Gujarati,             QStringLiteral("gu")            },
    { HaitianCreole,        QStringLiteral("ht")            },
    { Hausa,                QStringLiteral("ha")            },
    { Hawaiian,             QStringLiteral("haw")           },
    { Hebrew,               QStringLiteral("he")            },
    { HillMari,             QStringLiteral("mrj")           },
    { Hindi,                QStringLiteral("hi")            },
    { Hmong,                QStringLiteral("hmn")           },
    { Hungarian,            QStringLiteral("hu")            },
    { Icelandic,            QStringLiteral("is")            },
    { Igbo,                 QStringLiteral("ig")            },
    { Indonesian,           QStringLiteral("id")            },
    { Irish,                QStringLiteral("ga")            },
    { Italian,              QStringLiteral("it")            },
    { Japanese,             QStringLiteral("ja")            },
    { Javanese,             QStringLiteral("jw")            },
    { Kannada,              QStringLiteral("kn")            },
    { Kazakh,               QStringLiteral("kk")            },
    { Khmer,                QStringLiteral("km")            },
    { Kinyarwanda,          QStringLiteral("rw")            },
    { Klingon,              QStringLiteral("tlh")           },
    { KlingonPlqaD,         QStringLiteral("tlh-Qaak")      },
    { Korean,               QStringLiteral("ko")            },
    { Kurdish,              QStringLiteral("ku")            },
    { Kyrgyz,               QStringLiteral("ky")            },
    { Lao,                  QStringLiteral("lo")            },
    { Latin,                QStringLiteral("la")            },
    { Latvian,              QStringLiteral("lv")            },
    { LevantineArabic,      QStringLiteral("apc")           },
    { Lithuanian,           QStringLiteral("lt")            },
    { Luxembourgish,        QStringLiteral("lb")            },
    { Macedonian,           QStringLiteral("mk")            },
    { Malagasy,             QStringLiteral("mg")            },
    { Malay,                QStringLiteral("ms")            },
    { Malayalam,            QStringLiteral("ml")            },
    { Maltese,              QStringLiteral("mt")            },
    { Maori,                QStringLiteral("mi")            },
    { Marathi,              QStringLiteral("mr")            },
    { Mari,                 QStringLiteral("mhr")           },
    { Mongolian,            QStringLiteral("mn")            },
    { Myanmar,              QStringLiteral("my")            },
    { Nepali,               QStringLiteral("ne")            },
    { Norwegian,            QStringLiteral("no")            },
    { Oriya,                QStringLiteral("or")            },
    { Papiamento,           QStringLiteral("pap")           },
    { Pashto,               QStringLiteral("ps")            },
    { Persian,              QStringLiteral("fa")            },
    { Polish,               QStringLiteral("pl")            },
    { Portuguese,           QStringLiteral("pt")            },
    { Punjabi,              QStringLiteral("pa")            },
    { QueretaroOtomi,       QStringLiteral("otq")           },
    { Romanian,             QStringLiteral("ro")            },
    { Russian,              QStringLiteral("ru")            },
    { Samoan,               QStringLiteral("sm")            },
    { ScotsGaelic,          QStringLiteral("gd")            },
    { SerbianCyrillic,      QStringLiteral("sr")            },
    { SerbianLatin,         QStringLiteral("sr-Latin")      },
    { Sesotho,              QStringLiteral("st")            },
    { Shona,                QStringLiteral("sn")            },
    { SimplifiedChinese,    QStringLiteral("zh-CN")         },
    { Sindhi,               QStringLiteral("sd")            },
    { Sinhala,              QStringLiteral("si")            },
    { Slovak,               QStringLiteral("sk")            },
    { Slovenian,            QStringLiteral("sl")            },
    { Somali,               QStringLiteral("so")            },
    { Spanish,              QStringLiteral("es")            },
    { Sundanese,            QStringLiteral("su")            },
    { Swahili,              QStringLiteral("sw")            },
    { Swedish,              QStringLiteral("sv")            },
    { Tagalog,              QStringLiteral("tl")            },
    { Tahitian,             QStringLiteral("ty")            },
    { Tajik,                QStringLiteral("tg")            },
    { Tamil,                QStringLiteral("ta")            },
    { Tatar,                QStringLiteral("tt")            },
    { Telugu,               QStringLiteral("te")            },
    { Thai,                 QStringLiteral("th")            },
    { Tongan,               QStringLiteral("to")            },
    { TraditionalChinese,   QStringLiteral("zh-TW")         },
    { Turkish,              QStringLiteral("tr")            },
    { Turkmen,              QStringLiteral("tk")            },
    { Udmurt,               QStringLiteral("udm")           },
    { Uighur,               QStringLiteral("ug")            },
    { Ukrainian,            QStringLiteral("uk")            },
    { Urdu,                 QStringLiteral("ur")            },
    { Uzbek,                QStringLiteral("uz")            },
    { Vietnamese,           QStringLiteral("vi")            },
    { Welsh,                QStringLiteral("cy")            },
    { Xhosa,                QStringLiteral("xh")            },
    { Yiddish,              QStringLiteral("yi")            },
    { Yoruba,               QStringLiteral("yo")            },
    { YucatecMaya,          QStringLiteral("yua")           },
    { Zulu,                 QStringLiteral("zu")            }
};

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::s_googleLanguageCodes =
{
    { Hebrew,               QStringLiteral("iw")            }
};

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::s_yandexLanguageCodes =
{
    { SimplifiedChinese,    QStringLiteral("zn")            },
    { Javanese,             QStringLiteral("jv")            }
};

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::s_bingLanguageCodes =
{
    { Auto,                 QStringLiteral("auto-detect")   },
    { Bosnian,              QStringLiteral("bs-Latn")       },
    { SerbianCyrillic,      QStringLiteral("sr-Cyrl")       },
    { SimplifiedChinese,    QStringLiteral("zh-Hans")       },
    { TraditionalChinese,   QStringLiteral("zh-Hant")       },
    { Hmong,                QStringLiteral("mww")           }
};

const QMap<DOnlineTranslator::Language, QString> DOnlineTranslator::s_lingvaLanguageCodes =
{
    { SimplifiedChinese,    QStringLiteral("zh")            },
    { TraditionalChinese,   QStringLiteral("zh_HANT")       }
};

// --------------------------------------------------------------------------------------------

const QMap<QString, QString> DOnlineTranslator::s_rfc3066LanguageCodes =
{
    { QLatin1String("af-ZA"),  QLatin1String("af")       },
    { QLatin1String("am-ET"),  QLatin1String("am")       },
//     { QLatin1String("")     ,  QLatin1String("apc")      },      // North Levantine Arabic: do not exists as 2 letters code
    { QLatin1String("ar-AE"),  QLatin1String("ar")       },
    { QLatin1String("ar-BH"),  QLatin1String("ar")       },
    { QLatin1String("ar-DZ"),  QLatin1String("ar")       },
    { QLatin1String("ar-EG"),  QLatin1String("ar")       },
    { QLatin1String("ar-IL"),  QLatin1String("ar")       },
    { QLatin1String("ar-IQ"),  QLatin1String("ar")       },
    { QLatin1String("ar-JO"),  QLatin1String("ar")       },
    { QLatin1String("ar-KW"),  QLatin1String("ar")       },
    { QLatin1String("ar-LB"),  QLatin1String("ar")       },
    { QLatin1String("ar-LY"),  QLatin1String("ar")       },
    { QLatin1String("ar-MA"),  QLatin1String("ar")       },
    { QLatin1String("ar-MR"),  QLatin1String("ar")       },
    { QLatin1String("ar-OM"),  QLatin1String("ar")       },
    { QLatin1String("ar-PS"),  QLatin1String("ar")       },
    { QLatin1String("ar-QA"),  QLatin1String("ar")       },
    { QLatin1String("ar-SA"),  QLatin1String("ar")       },
    { QLatin1String("ar-SD"),  QLatin1String("ar")       },
    { QLatin1String("ar-SO"),  QLatin1String("ar")       },
    { QLatin1String("ar-SY"),  QLatin1String("ar")       },
    { QLatin1String("ar-TD"),  QLatin1String("ar")       },
    { QLatin1String("ar-TN"),  QLatin1String("ar")       },
    { QLatin1String("ar-YE"),  QLatin1String("ar")       },
//     { QLatin1String("as-IN"),  QLatin1String("")         },
    { QLatin1String("az-AZ"),  QLatin1String("az")       },
    { QLatin1String("ba-RU"),  QLatin1String("ba")       },
    { QLatin1String("be-BY"),  QLatin1String("be")       },
    { QLatin1String("bn-BD"),  QLatin1String("bn")       },
    { QLatin1String("bn-IN"),  QLatin1String("bn")       },
    { QLatin1String("bn-SG"),  QLatin1String("bn")       },
//     { QLatin1String("bo-BT"),  QLatin1String("")         },
//     { QLatin1String("bo-CN"),  QLatin1String("")         },
    { QLatin1String("bs-BA"),  QLatin1String("bs")       },
    { QLatin1String("bs-BG"),  QLatin1String("bs-Latn")  },
    { QLatin1String("bg-BG"),  QLatin1String("bg")       },
//     { QLatin1String("br-FR"),  QLatin1String("")         },
//     { QLatin1String("")     ,  QLatin1String("yue")      },      // Yue Chinese: do not exists as 2 letters code
    { QLatin1String("ca-ES"),  QLatin1String("ca")       },
    { QLatin1String("ca-AD"),  QLatin1String("ca")       },
    { QLatin1String("ca-FR"),  QLatin1String("ca")       },
//     { QLatin1String("")     ,  QLatin1String("ceb")      },      // Cebuano: do not exists as 2 letters code
//    { QLatin1String("ch-GU"),  QLatin1String("")       },
//    { QLatin1String("ch-MP"),  QLatin1String("")       },
    { QLatin1String("co-FR"),  QLatin1String("co")       },
    { QLatin1String("cs-CZ"),  QLatin1String("cs")       },
    { QLatin1String("cy-GB"),  QLatin1String("cy")       },
    { QLatin1String("da-DK"),  QLatin1String("da")       },
    { QLatin1String("da-GL"),  QLatin1String("da")       },
    { QLatin1String("de-DE"),  QLatin1String("de")       },
    { QLatin1String("de-DK"),  QLatin1String("de")       },
    { QLatin1String("de-LI"),  QLatin1String("de")       },
    { QLatin1String("de-AT"),  QLatin1String("de")       },
    { QLatin1String("de-LU"),  QLatin1String("de")       },
    { QLatin1String("de-CH"),  QLatin1String("de")       },
    { QLatin1String("el-CY"),  QLatin1String("el")       },
    { QLatin1String("el-GR"),  QLatin1String("el")       },
    { QLatin1String("en-AG"),  QLatin1String("en")       },
    { QLatin1String("en-AI"),  QLatin1String("en")       },
    { QLatin1String("en-AS"),  QLatin1String("en")       },
    { QLatin1String("en-AU"),  QLatin1String("en")       },
    { QLatin1String("en-BB"),  QLatin1String("en")       },
    { QLatin1String("en-BM"),  QLatin1String("en")       },
    { QLatin1String("en-BN"),  QLatin1String("en")       },
    { QLatin1String("en-BS"),  QLatin1String("en")       },
    { QLatin1String("en-BW"),  QLatin1String("en")       },
    { QLatin1String("en-BZ"),  QLatin1String("en")       },
    { QLatin1String("en-CA"),  QLatin1String("en")       },
    { QLatin1String("en-CB"),  QLatin1String("en")       },
    { QLatin1String("en-CK"),  QLatin1String("en")       },
    { QLatin1String("en-CM"),  QLatin1String("en")       },
    { QLatin1String("en-DM"),  QLatin1String("en")       },
    { QLatin1String("en-ER"),  QLatin1String("en")       },
    { QLatin1String("en-ET"),  QLatin1String("en")       },
    { QLatin1String("en-FJ"),  QLatin1String("en")       },
    { QLatin1String("en-FK"),  QLatin1String("en")       },
    { QLatin1String("en-FM"),  QLatin1String("en")       },
    { QLatin1String("en-GD"),  QLatin1String("en")       },
    { QLatin1String("en-GH"),  QLatin1String("en")       },
    { QLatin1String("en-GI"),  QLatin1String("en")       },
    { QLatin1String("en-GM"),  QLatin1String("en")       },
    { QLatin1String("en-GU"),  QLatin1String("en")       },
    { QLatin1String("en-GY"),  QLatin1String("en")       },
    { QLatin1String("en-IE"),  QLatin1String("en")       },
    { QLatin1String("en-IL"),  QLatin1String("en")       },
    { QLatin1String("en-IN"),  QLatin1String("en")       },
    { QLatin1String("en-IO"),  QLatin1String("en")       },
    { QLatin1String("en-JM"),  QLatin1String("en")       },
    { QLatin1String("en-KR"),  QLatin1String("en")       },
    { QLatin1String("en-KI"),  QLatin1String("en")       },
    { QLatin1String("en-KN"),  QLatin1String("en")       },
    { QLatin1String("en-KY"),  QLatin1String("en")       },
    { QLatin1String("en-LC"),  QLatin1String("en")       },
    { QLatin1String("en-LR"),  QLatin1String("en")       },
    { QLatin1String("en-LS"),  QLatin1String("en")       },
    { QLatin1String("en-MP"),  QLatin1String("en")       },
    { QLatin1String("en-MS"),  QLatin1String("en")       },
    { QLatin1String("en-MT"),  QLatin1String("en")       },
    { QLatin1String("en-MU"),  QLatin1String("en")       },
    { QLatin1String("en-MW"),  QLatin1String("en")       },
    { QLatin1String("en-MY"),  QLatin1String("en")       },
    { QLatin1String("en-NA"),  QLatin1String("en")       },
    { QLatin1String("en-NF"),  QLatin1String("en")       },
    { QLatin1String("en-NG"),  QLatin1String("en")       },
    { QLatin1String("en-NR"),  QLatin1String("en")       },
    { QLatin1String("en-NU"),  QLatin1String("en")       },
    { QLatin1String("en-NZ"),  QLatin1String("en")       },
    { QLatin1String("en-PG"),  QLatin1String("en")       },
    { QLatin1String("en-PH"),  QLatin1String("en")       },
    { QLatin1String("en-PK"),  QLatin1String("en")       },
    { QLatin1String("en-PN"),  QLatin1String("en")       },
    { QLatin1String("en-PR"),  QLatin1String("en")       },
    { QLatin1String("en-PW"),  QLatin1String("en")       },
    { QLatin1String("en-RW"),  QLatin1String("en")       },
    { QLatin1String("en-SB"),  QLatin1String("en")       },
    { QLatin1String("en-SC"),  QLatin1String("en")       },
    { QLatin1String("en-SG"),  QLatin1String("en")       },
    { QLatin1String("en-SH"),  QLatin1String("en")       },
    { QLatin1String("en-SL"),  QLatin1String("en")       },
    { QLatin1String("en-SO"),  QLatin1String("en")       },
    { QLatin1String("en-SZ"),  QLatin1String("en")       },
    { QLatin1String("en-TC"),  QLatin1String("en")       },
    { QLatin1String("en-TK"),  QLatin1String("en")       },
    { QLatin1String("en-TO"),  QLatin1String("en")       },
    { QLatin1String("en-TT"),  QLatin1String("en")       },
    { QLatin1String("en-UG"),  QLatin1String("en")       },
    { QLatin1String("en-US"),  QLatin1String("en")       },
    { QLatin1String("en-VC"),  QLatin1String("en")       },
    { QLatin1String("en-VG"),  QLatin1String("en")       },
    { QLatin1String("en-VI"),  QLatin1String("en")       },
    { QLatin1String("en-VU"),  QLatin1String("en")       },
    { QLatin1String("en-WS"),  QLatin1String("en")       },
    { QLatin1String("en-ZA"),  QLatin1String("en")       },
    { QLatin1String("en-ZM"),  QLatin1String("en")       },
    { QLatin1String("en-ZW"),  QLatin1String("en")       },
    { QLatin1String("eo-FR"),  QLatin1String("eo")       },
    { QLatin1String("et-EE"),  QLatin1String("et")       },
    { QLatin1String("es-AR"),  QLatin1String("es")       },
    { QLatin1String("es-BO"),  QLatin1String("es")       },
    { QLatin1String("es-CL"),  QLatin1String("es")       },
    { QLatin1String("es-CQ"),  QLatin1String("es")       },
    { QLatin1String("es-CR"),  QLatin1String("es")       },
    { QLatin1String("es-CU"),  QLatin1String("es")       },
    { QLatin1String("es-DO"),  QLatin1String("es")       },
    { QLatin1String("es-EC"),  QLatin1String("es")       },
    { QLatin1String("es-ES"),  QLatin1String("es")       },
    { QLatin1String("es-GT"),  QLatin1String("es")       },
    { QLatin1String("es-HN"),  QLatin1String("es")       },
    { QLatin1String("es-MX"),  QLatin1String("es")       },
    { QLatin1String("es-NI"),  QLatin1String("es")       },
    { QLatin1String("es-PA"),  QLatin1String("es")       },
    { QLatin1String("es-PE"),  QLatin1String("es")       },
    { QLatin1String("es-PR"),  QLatin1String("es")       },
    { QLatin1String("es-PY"),  QLatin1String("es")       },
    { QLatin1String("es-SV"),  QLatin1String("es")       },
    { QLatin1String("es-US"),  QLatin1String("es")       },
    { QLatin1String("es-UY"),  QLatin1String("es")       },
    { QLatin1String("es-VE"),  QLatin1String("es")       },
    { QLatin1String("eu-ES"),  QLatin1String("eu")       },
    { QLatin1String("fa-IR"),  QLatin1String("fa")       },
//     { QLatin1String("")     ,  QLatin1String("fil")      },      // Filipino: do not exists as 2 letters code
    { QLatin1String("fi-FI"),  QLatin1String("fi")       },
    { QLatin1String("fj-FJ"),  QLatin1String("fj")       },
    { QLatin1String("fr-AD"),  QLatin1String("fr")       },
    { QLatin1String("fr-BF"),  QLatin1String("fr")       },
    { QLatin1String("fr-BI"),  QLatin1String("fr")       },
    { QLatin1String("fr-BJ"),  QLatin1String("fr")       },
    { QLatin1String("fr-BE"),  QLatin1String("fr")       },
    { QLatin1String("fr-CA"),  QLatin1String("fr")       },
    { QLatin1String("fr-CD"),  QLatin1String("fr")       },
    { QLatin1String("fr-CH"),  QLatin1String("fr")       },
    { QLatin1String("fr-CI"),  QLatin1String("fr")       },
    { QLatin1String("fr-CM"),  QLatin1String("fr")       },
    { QLatin1String("fr-DJ"),  QLatin1String("fr")       },
    { QLatin1String("fr-FR"),  QLatin1String("fr")       },
    { QLatin1String("fr-GA"),  QLatin1String("fr")       },
    { QLatin1String("fr-GB"),  QLatin1String("fr")       },
    { QLatin1String("fr-GF"),  QLatin1String("fr")       },
    { QLatin1String("fr-GN"),  QLatin1String("fr")       },
    { QLatin1String("fr-GP"),  QLatin1String("fr")       },
    { QLatin1String("fr-HT"),  QLatin1String("fr")       },
    { QLatin1String("fr-IT"),  QLatin1String("fr")       },
    { QLatin1String("fr-KM"),  QLatin1String("fr")       },
    { QLatin1String("fr-LB"),  QLatin1String("fr")       },
    { QLatin1String("fr-LU"),  QLatin1String("fr")       },
    { QLatin1String("fr-MC"),  QLatin1String("fr")       },
    { QLatin1String("fr-MG"),  QLatin1String("fr")       },
    { QLatin1String("fr-ML"),  QLatin1String("fr")       },
    { QLatin1String("fr-MQ"),  QLatin1String("fr")       },
    { QLatin1String("fr-NC"),  QLatin1String("fr")       },
    { QLatin1String("fr-PF"),  QLatin1String("fr")       },
    { QLatin1String("fr-PM"),  QLatin1String("fr")       },
    { QLatin1String("fr-RE"),  QLatin1String("fr")       },
    { QLatin1String("fr-RW"),  QLatin1String("fr")       },
    { QLatin1String("fr-SC"),  QLatin1String("fr")       },
    { QLatin1String("fr-TD"),  QLatin1String("fr")       },
    { QLatin1String("fr-TG"),  QLatin1String("fr")       },
    { QLatin1String("fr-VU"),  QLatin1String("fr")       },
    { QLatin1String("fr-WF"),  QLatin1String("fr")       },
    { QLatin1String("fr-YT"),  QLatin1String("fr")       },
    { QLatin1String("fy-NL"),  QLatin1String("fy")       },
    { QLatin1String("ga-GB"),  QLatin1String("ga")       },
    { QLatin1String("ga-IE"),  QLatin1String("ga")       },
    { QLatin1String("gd-GB"),  QLatin1String("gd")       },
    { QLatin1String("gl-ES"),  QLatin1String("gl")       },
//    { QLatin1String("gv-GB"),  QLatin1String("")          },
    { QLatin1String("gu-IN"),  QLatin1String("gu")       },
    { QLatin1String("ha-TD"),  QLatin1String("ha")       },
//     { QLatin1String("")     ,  QLatin1String("haw")      },      // Hawaiia: do not exists as 2 letters code
    { QLatin1String("he-IL"),  QLatin1String("he")       },
    { QLatin1String("hi-IN"),  QLatin1String("hi")       },
    { QLatin1String("hr-HR"),  QLatin1String("hr")       },
    { QLatin1String("hr-BA"),  QLatin1String("")         },
//     { QLatin1String("")     ,  QLatin1String("hmn")      },      // Hmong: do not exists as 2 letters code
    { QLatin1String("ht-FR"),  QLatin1String("ht")       },
    { QLatin1String("hu-HU"),  QLatin1String("hu")       },
    { QLatin1String("hu-SI"),  QLatin1String("hu")       },
    { QLatin1String("hy-AM"),  QLatin1String("hy")       },
    { QLatin1String("id-ID"),  QLatin1String("id")       },
    { QLatin1String("ig-NG"),  QLatin1String("ig")       },
    { QLatin1String("is-IS"),  QLatin1String("is")       },
    { QLatin1String("iw-IL"),  QLatin1String("iw")       },
//    { QLatin1String("ii-CN"),  QLatin1String("")         },
    { QLatin1String("it-CH"),  QLatin1String("it")       },
    { QLatin1String("it-HR"),  QLatin1String("it")       },
    { QLatin1String("it-IT"),  QLatin1String("it")       },
    { QLatin1String("it-SI"),  QLatin1String("it")       },
    { QLatin1String("it-SM"),  QLatin1String("it")       },
    { QLatin1String("ja-JP"),  QLatin1String("ja")       },
    { QLatin1String("jv-ID"),  QLatin1String("jv")       },
    { QLatin1String("jw-ID"),  QLatin1String("jw")       },
    { QLatin1String("ka-GE"),  QLatin1String("ka")       },
    { QLatin1String("kn-IN"),  QLatin1String("kn")       },
    { QLatin1String("kk-KZ"),  QLatin1String("kk")       },
//     { QLatin1String("kl-GL"),  QLatin1String("")         },
    { QLatin1String("km-KH"),  QLatin1String("km")       },
    { QLatin1String("ko-KR"),  QLatin1String("ko")       },
    { QLatin1String("ko-KP"),  QLatin1String("ko")       },
    { QLatin1String("ku-IR"),  QLatin1String("ku")       },
    { QLatin1String("ky-KG"),  QLatin1String("ky")       },
//    { QLatin1String("kw-GB"),  QLatin1String("")          },
    { QLatin1String("la-IT"),  QLatin1String("la")       },
//    { QLatin1String("ln-CD"),  QLatin1String("")          },
//    { QLatin1String("ln-CG"),  QLatin1String("")          },
    { QLatin1String("lo-LA"),  QLatin1String("lo")       },
    { QLatin1String("lb-LU"),  QLatin1String("lb")       },
    { QLatin1String("lt-LT"),  QLatin1String("lt")       },
    { QLatin1String("lv-LV"),  QLatin1String("lv")       },
    { QLatin1String("mg-MG"),  QLatin1String("mg")       },
    { QLatin1String("mi-NZ"),  QLatin1String("mi")       },
    { QLatin1String("mk-MK"),  QLatin1String("mk")       },
    { QLatin1String("ml-IN"),  QLatin1String("ml")       },
//     { QLatin1String("")     ,  QLatin1String("mhr")      },      // Meadow Mari language: do not exists as 2 letters code
    { QLatin1String("mn-MN"),  QLatin1String("mn")       },
    { QLatin1String("mn-CN"),  QLatin1String("mn")       },
    { QLatin1String("mr-IN"),  QLatin1String("mr")       },
//     { QLatin1String("")     ,  QLatin1String("mrj")      },      // Western Mari: do not exists as 2 letters code
    { QLatin1String("ms-BN"),  QLatin1String("ms")         },
    { QLatin1String("ms-MY"),  QLatin1String("ms")       },
    { QLatin1String("ms-SG"),  QLatin1String("ms")       },
    { QLatin1String("mt-MT"),  QLatin1String("mt")       },
    { QLatin1String("my-MM"),  QLatin1String("my")       },
//     { QLatin1String("")     ,  QLatin1String("mww")      },      // Hmong: do not exists as 2 letters code
//     { QLatin1String("nb-NO"),  QLatin1String("")         },
    { QLatin1String("ne-NP"),  QLatin1String("ne")       },
    { QLatin1String("nl-AN"),  QLatin1String("nl")       },
    { QLatin1String("nl-AW"),  QLatin1String("nl")       },
    { QLatin1String("nl-NL"),  QLatin1String("nl")       },
    { QLatin1String("nl-BE"),  QLatin1String("nl")       },
    { QLatin1String("nl-SR"),  QLatin1String("nl")       },
//     { QLatin1String("nn-NO"),  QLatin1String("")         },
    { QLatin1String("no-NO"),  QLatin1String("no")       },
//     { QLatin1String("ns-ZA"),  QLatin1String("")         },
    { QLatin1String("ny-ZM"),  QLatin1String("ny")       },
//     { QLatin1String("oc-FR"),  QLatin1String("")         },
    { QLatin1String("or-IN"),  QLatin1String("or")       },
//     { QLatin1String("")     ,  QLatin1String("pap")      },      // Papiamento: do not exists as 2 letters code
    { QLatin1String("pa-IN"),  QLatin1String("pa")       },
    { QLatin1String("pl-PL"),  QLatin1String("pl")       },
    { QLatin1String("pt-AO"),  QLatin1String("pt")         },
    { QLatin1String("pt-BR"),  QLatin1String("pt")         },
    { QLatin1String("pt-CV"),  QLatin1String("pt")         },
    { QLatin1String("pt-GW"),  QLatin1String("pt")         },
    { QLatin1String("pt-MZ"),  QLatin1String("pt")         },
    { QLatin1String("pt-PT"),  QLatin1String("pt")       },
    { QLatin1String("pt-ST"),  QLatin1String("pt")       },
    { QLatin1String("ps-AF"),  QLatin1String("ps")       },
//     { QLatin1String("")     ,  QLatin1String("otq")      },      // Otomi Querétaro:  do not exists as 2 letters code
    { QLatin1String("ro-RO"),  QLatin1String("ro")       },
    { QLatin1String("ru-RU"),  QLatin1String("ru")       },
    { QLatin1String("rw-RW"),  QLatin1String("rw")       },
//     { QLatin1String("sa-IN"),  QLatin1String("")         },
    { QLatin1String("sd-PK"),  QLatin1String("sd")       },
//     { QLatin1String("se-FI"),  QLatin1String("")         },
//     { QLatin1String("se-NO"),  QLatin1String("")         },
//     { QLatin1String("se-SE"),  QLatin1String("")         },
    { QLatin1String("si-LK"),  QLatin1String("si")       },
    { QLatin1String("sk-SK"),  QLatin1String("sk")       },
    { QLatin1String("sl-SI"),  QLatin1String("sl")       },
    { QLatin1String("sm-WS"),  QLatin1String("sm")       },
    { QLatin1String("sn-ZW"),  QLatin1String("sn")       },
    { QLatin1String("so-SO"),  QLatin1String("so")       },
    { QLatin1String("sq-AL"),  QLatin1String("sq")       },
    { QLatin1String("sr-BA"),  QLatin1String("sr")       },
    { QLatin1String("sr-YU"),  QLatin1String("sr")       },
    { QLatin1String("sr-RS"),  QLatin1String("sr-Cyrl")  },
    { QLatin1String("sr-SP"),  QLatin1String("sr-Latin") },
//     { QLatin1String("ss-SZ")     ,  QLatin1String("")       },
//     { QLatin1String("ss-ZA")     ,  QLatin1String("")       },
    { QLatin1String("st-LS"),  QLatin1String("st")       },
    { QLatin1String("su-ID"),  QLatin1String("su")       },
    { QLatin1String("sv-FI"),  QLatin1String("sv")       },
    { QLatin1String("sv-SE"),  QLatin1String("sv")       },
    { QLatin1String("sw-KE"),  QLatin1String("sw")       },
    { QLatin1String("sw-TZ"),  QLatin1String("sw")       },
    { QLatin1String("ta-IN"),  QLatin1String("ta")       },
    { QLatin1String("ta-SG"),  QLatin1String("ta")       },
    { QLatin1String("te-IN"),  QLatin1String("te")       },
    { QLatin1String("tg-TJ"),  QLatin1String("tg")       },
    { QLatin1String("th-TH"),  QLatin1String("th")       },
    { QLatin1String("tk-TM"),  QLatin1String("tk")       },
    { QLatin1String("tl-PH"),  QLatin1String("tl")       },
    { QLatin1String("to-TO"),  QLatin1String("to")       },
//     { QLatin1String("")     ,  QLatin1String("tlh")      },      // Klingon tlhIngan-Hol: do not exists as 2 letters code
//     { QLatin1String("")     ,  QLatin1String("tlh-Qaak") },      // Klingon tlhIngan-Hol: do not exists as 2 letters code
//     { QLatin1String("tn-ZA"),  QLatin1String("")         },
    { QLatin1String("tr-BG"),  QLatin1String("tr")       },
    { QLatin1String("tr-CY"),  QLatin1String("tr")       },
    { QLatin1String("tr-TR"),  QLatin1String("tr")       },
    { QLatin1String("tt-RU"),  QLatin1String("tt")       },
    { QLatin1String("ty-PF"),  QLatin1String("ty")       },
//     { QLatin1String("")     ,  QLatin1String("udm")      },      // Udmurt: do not exists as 2 letters code
    { QLatin1String("ug-CN"),  QLatin1String("ug")       },
    { QLatin1String("uk-UA"),  QLatin1String("uk")       },
    { QLatin1String("ur-IN"),  QLatin1String("ur")       },
    { QLatin1String("ur-PK"),  QLatin1String("ur")       },
    { QLatin1String("uz-UZ"),  QLatin1String("uz")       },
    { QLatin1String("vi-VN"),  QLatin1String("vi")       },
//     { QLatin1String("wo-SN"),  QLatin1String("")         },
    { QLatin1String("xh-ZA"),  QLatin1String("xh")       },
    { QLatin1String("yi-DE"),  QLatin1String("yi")       },
    { QLatin1String("yo-NG"),  QLatin1String("yo")       },
//     { QLatin1String("")     ,  QLatin1String("yua")      },      // Maya Yucatec: do not exists as 2 letters code
//     { QLatin1String("")     ,  QLatin1String("zh-Hans")  },      // SimplifiedChinese
//     { QLatin1String(""),       QLatin1String("zh-Hant")  },      // TraditionalChinese
//     { QLatin1String(""),       QLatin1String("zh_HANT")  },      // TraditionalChinese
    { QLatin1String("zh-CN"),  QLatin1String("zh-CN")    },         // Chinese (Simplified, PRC)
    { QLatin1String("zh-MO"),  QLatin1String("zh")       },         // Chinese (Traditional, Macao S.A.R.)
    { QLatin1String("zh-HK"),  QLatin1String("zh")       },         // Chinese (Traditional, Hong Kong S.A.R.)
    { QLatin1String("zh-SG"),  QLatin1String("zh")       },         // Chinese (Simplified, Singapore)
    { QLatin1String("zh-TW"),  QLatin1String("zh-TW")    },         // Chinese (Traditional, Taiwan)
    { QLatin1String("zn-CN"),  QLatin1String("zn")       },
    { QLatin1String("zu-ZA"),  QLatin1String("zu")       }
};

DOnlineTranslator::DOnlineTranslator(QObject* const parent)
    : QObject         (parent),
      m_stateMachine  (new QStateMachine(this)),
      m_networkManager(new QNetworkAccessManager(this)),
      m_libreUrl      (QLatin1String("https://translate.argosopentech.com")),
      m_lingvaUrl     (QLatin1String("https://lingva.ml"))
{
    connect(m_stateMachine, &QStateMachine::finished,
            this, &DOnlineTranslator::signalFinished);

    connect(m_stateMachine, &QStateMachine::stopped,
            this, &DOnlineTranslator::signalFinished);
}

QString DOnlineTranslator::fromRFC3066(const QString& langCodeRFC3066)
{
    return s_rfc3066LanguageCodes[langCodeRFC3066];
}

QStringList DOnlineTranslator::supportedRFC3066()
{
    return s_rfc3066LanguageCodes.keys();
}

void DOnlineTranslator::translate(const QString& text,
                                  Engine engine,
                                  Language translationLang,
                                  Language sourceLang,
                                  Language uiLang)
{
    abort();
    resetData();

    m_onlyDetectLanguage = false;
    m_source             = text;
    m_sourceLang         = sourceLang;
    m_translationLang    = (translationLang == Auto) ? language(QLocale()) : translationLang;
    m_uiLang             = (uiLang == Auto)          ? language(QLocale()) : uiLang;

    // Check if the selected languages are supported by the engine
 
   if (!isSupportTranslation(engine, m_sourceLang))
    {
        resetData(ParametersError,
                  i18n("Selected source language %1 is not supported for %2",
                       languageName(m_sourceLang),
                       QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

        Q_EMIT signalFinished();

        return;
    }

    if (!isSupportTranslation(engine, m_translationLang))
    {
        resetData(ParametersError,
                  i18n("Selected translation language %1 is not supported for %2",
                       languageName(m_translationLang),
                       QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

        Q_EMIT signalFinished();

        return;
    }

    if (!isSupportTranslation(engine, m_uiLang))
    {
        resetData(ParametersError,
                  i18n("Selected ui language %1 is not supported for %2",
                       languageName(m_uiLang),
                       QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

        Q_EMIT signalFinished();

        return;
    }

    switch (engine)
    {
        case Google:
            buildGoogleStateMachine();
            break;

        case Yandex:
            buildYandexStateMachine();
            break;

        case Bing:
            buildBingStateMachine();
            break;

        case LibreTranslate:

            if (m_libreUrl.isEmpty())
            {
                resetData(ParametersError,
                        i18n("%1 URL can't be empty.",
                             QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

                Q_EMIT signalFinished();

                return;
            }

            buildLibreStateMachine();
            break;

        case Lingva:

            if (m_lingvaUrl.isEmpty())
            {
                resetData(ParametersError,
                        i18n("%1 URL can't be empty.",
                             QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

                Q_EMIT signalFinished();

                return;
            }

            buildLingvaStateMachine();
            break;
    }

    m_stateMachine->start();
}

QString DOnlineTranslator::engineName(Engine engine)
{
    switch (engine)
    {
        case Yandex:
            return QLatin1String("Yandex");

        case Bing:
            return QLatin1String("Bing");

        case LibreTranslate:
            return QLatin1String("Libre Translate");

        case Lingva:
            return QLatin1String("Lingva");

        default:
            return QLatin1String("Google");
    }
}

void DOnlineTranslator::detectLanguage(const QString& text, Engine engine)
{
    abort();
    resetData();

    m_onlyDetectLanguage = true;
    m_source             = text;
    m_sourceLang         = Auto;
    m_translationLang    = English;
    m_uiLang             = language(QLocale());

    switch (engine)
    {
        case Google:
            buildGoogleDetectStateMachine();
            break;

        case Yandex:
            buildYandexDetectStateMachine();
            break;

        case Bing:
            buildBingDetectStateMachine();
            break;

        case LibreTranslate:

            if (m_libreUrl.isEmpty())
            {
                resetData(ParametersError,
                        i18n("%1 URL can't be empty.",
                             QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

                Q_EMIT signalFinished();

                return;
            }

            buildLibreDetectStateMachine();
            break;

        case Lingva:

            if (m_lingvaUrl.isEmpty())
            {
                resetData(ParametersError,
                        i18n("%1 URL can't be empty.",
                             QString::fromUtf8(QMetaEnum::fromType<Engine>().valueToKey(engine))));

                Q_EMIT signalFinished();

                return;
            }

            buildLingvaDetectStateMachine();
            break;
    }

    m_stateMachine->start();
}

void DOnlineTranslator::abort()
{
    if (m_currentReply != nullptr)
    {
        m_currentReply->abort();
    }
}

bool DOnlineTranslator::isRunning() const
{
    return m_stateMachine->isRunning();
}

void DOnlineTranslator::slotSkipGarbageText()
{
    m_translation.append(sender()->property(s_textProperty).toString());
}

void DOnlineTranslator::buildSplitNetworkRequest(QState* const parent,
                                                 void (DOnlineTranslator::*requestMethod)(),
                                                 void (DOnlineTranslator::*parseMethod)(),
                                                 const QString& text,
                                                 int textLimit)
{
    QString unsendedText       = text;
    auto* nextTranslationState = new QState(parent);
    parent->setInitialState(nextTranslationState);

    while (!unsendedText.isEmpty())
    {
        auto* currentTranslationState = nextTranslationState;
        nextTranslationState          = new QState(parent);

        // Do not translate the part if it looks like garbage

        const int splitIndex          = getSplitIndex(unsendedText, textLimit);

        if (splitIndex == -1)
        {
            currentTranslationState->setProperty(s_textProperty, unsendedText.left(textLimit));
            currentTranslationState->addTransition(nextTranslationState);

            connect(currentTranslationState, &QState::entered,
                    this, &DOnlineTranslator::slotSkipGarbageText);

            // Remove the parsed part from the next parsing

            unsendedText = unsendedText.mid(textLimit);
        }
        else
        {
            buildNetworkRequestState(currentTranslationState, requestMethod, parseMethod, unsendedText.left(splitIndex));
            currentTranslationState->addTransition(currentTranslationState, &QState::finished, nextTranslationState);

            // Remove the parsed part from the next parsing

            unsendedText = unsendedText.mid(splitIndex);
        }
    }

    nextTranslationState->addTransition(new QFinalState(parent));
}

void DOnlineTranslator::buildNetworkRequestState(QState* const parent,
                                                 void (DOnlineTranslator::*requestMethod)(),
                                                 void (DOnlineTranslator::*parseMethod)(),
                                                 const QString& text)
{
    // Network substates

    auto* requestingState = new QState(parent);
    auto* parsingState    = new QState(parent);

    parent->setInitialState(requestingState);

    // Substates transitions

    requestingState->addTransition(m_networkManager, &QNetworkAccessManager::finished, parsingState);
    parsingState->addTransition(new QFinalState(parent));

    // Setup requesting state

    requestingState->setProperty(s_textProperty, text);

    connect(requestingState, &QState::entered,
            this, requestMethod);

    // Setup parsing state

    connect(parsingState, &QState::entered,
            this, parseMethod);
}

void DOnlineTranslator::resetData(TranslationError error, const QString& errorString)
{
    m_error       = error;
    m_errorString = errorString;
    m_translation.clear();
    m_translationTranslit.clear();
    m_sourceTranslit.clear();
    m_sourceTranscription.clear();
    m_translationOptions.clear();

    m_stateMachine->stop();

    for (QAbstractState* state : m_stateMachine->findChildren<QAbstractState*>())
    {
        if (!m_stateMachine->configuration().contains(state))
        {
            state->deleteLater();
        }
    }
}

QString DOnlineTranslator::languageApiCode(Engine engine, Language lang)
{
    if (!isSupportTranslation(engine, lang))
    {
        return QString();
    }

    switch (engine)
    {
        case Google:
            return s_googleLanguageCodes.value(lang, s_genericLanguageCodes.value(lang));

        case Yandex:
            return s_yandexLanguageCodes.value(lang, s_genericLanguageCodes.value(lang));

        case Bing:
            return s_bingLanguageCodes.value(lang, s_genericLanguageCodes.value(lang));

        case LibreTranslate:
            return s_genericLanguageCodes.value(lang);

        case Lingva:
            return s_lingvaLanguageCodes.value(lang, s_genericLanguageCodes.value(lang));
    }

    Q_UNREACHABLE();
}

DOnlineTranslator::Language DOnlineTranslator::language(Engine engine, const QString& langCode)
{
    // Engine exceptions

    switch (engine)
    {
        case Google:
            return s_googleLanguageCodes.key(langCode, s_genericLanguageCodes.key(langCode, NoLanguage));

        case Yandex:
            return s_yandexLanguageCodes.key(langCode, s_genericLanguageCodes.key(langCode, NoLanguage));

        case Bing:
            return s_bingLanguageCodes.key(langCode, s_genericLanguageCodes.key(langCode, NoLanguage));

        case LibreTranslate:
            return s_genericLanguageCodes.key(langCode, NoLanguage);

        case Lingva:
            return s_lingvaLanguageCodes.key(langCode, s_genericLanguageCodes.key(langCode, NoLanguage));
    }

    Q_UNREACHABLE();
}

int DOnlineTranslator::getSplitIndex(const QString& untranslatedText, int limit)
{
    if (untranslatedText.size() < limit)
    {
        return limit;
    }

    int splitIndex = untranslatedText.lastIndexOf(QLatin1String(". "), limit - 1);

    if (splitIndex != -1)
    {
        return splitIndex + 1;
    }

    splitIndex = untranslatedText.lastIndexOf(QLatin1Char(' '), limit - 1);

    if (splitIndex != -1)
    {
        return splitIndex + 1;
    }

    splitIndex = untranslatedText.lastIndexOf(QLatin1Char('\n'), limit - 1);

    if (splitIndex != -1)
    {
        return splitIndex + 1;
    }

    // Non-breaking space

    splitIndex = untranslatedText.lastIndexOf(0x00a0, limit - 1);

    if (splitIndex != -1)
    {
        return splitIndex + 1;
    }

    // If the text has not passed any check and is most likely garbage

    return limit;
}

bool DOnlineTranslator::isContainsSpace(const QString& text)
{
    return std::any_of(text.cbegin(), text.cend(), [](QChar symbol)
        {
            return symbol.isSpace();
        }
    );
}

void DOnlineTranslator::addSpaceBetweenParts(QString& text)
{
    if (text.isEmpty())
    {
        return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)

    if (!text.back().isSpace())
    {

#else

    if (!text.at(text.size() - 1).isSpace())
    {

#endif

        text.append(QLatin1Char(' '));
    }
}

} // namespace Digikam
