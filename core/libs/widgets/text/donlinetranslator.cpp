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
//     { QLatin1String("")     ,  QLatin1String("mrj")      },      // Western Mari: do not exists as 2 letters code
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
//     { QLatin1String("")     ,  QLatin1String("apc")      },      // North Levantine Arabic: do not exists as 2 letters code
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

QJsonDocument DOnlineTranslator::toJson() const
{
    QJsonObject translationOptions;

    for (auto it = m_translationOptions.cbegin() ; it != m_translationOptions.cend() ; ++it)
    {
        QJsonArray arr;

        for (const DOnlineTranslatorOption& option : it.value())
        {
            arr.append(option.toJson());
        }

        translationOptions.insert(it.key(), arr);
    }

    QJsonObject object
    {
        { QLatin1String("source"),               m_source                   },
        { QLatin1String("sourceTranscription"),  m_sourceTranscription      },
        { QLatin1String("sourceTranslit"),       m_sourceTranslit           },
        { QLatin1String("translation"),          m_translation              },
        { QLatin1String("translationOptions"),   qMove(translationOptions)  },
        { QLatin1String("translationTranslit"),  m_translationTranslit      },
    };

    return QJsonDocument(object);
}

QString DOnlineTranslator::source() const
{
    return m_source;
}

QString DOnlineTranslator::sourceTranslit() const
{
    return m_sourceTranslit;
}

QString DOnlineTranslator::sourceTranscription() const
{
    return m_sourceTranscription;
}

QString DOnlineTranslator::sourceLanguageName() const
{
    return languageName(m_sourceLang);
}

DOnlineTranslator::Language DOnlineTranslator::sourceLanguage() const
{
    return m_sourceLang;
}

QString DOnlineTranslator::translation() const
{
    return m_translation;
}

QString DOnlineTranslator::translationTranslit() const
{
    return m_translationTranslit;
}

QString DOnlineTranslator::translationLanguageName() const
{
    return languageName(m_translationLang);
}

DOnlineTranslator::Language DOnlineTranslator::translationLanguage() const
{
    return m_translationLang;
}

QMap<QString, QVector<DOnlineTranslatorOption>> DOnlineTranslator::translationOptions() const
{
    return m_translationOptions;
}

DOnlineTranslator::TranslationError DOnlineTranslator::error() const
{
    return m_error;
}

QString DOnlineTranslator::errorString() const
{
    return m_errorString;
}

bool DOnlineTranslator::isSourceTranslitEnabled() const
{
    return m_sourceTranslitEnabled;
}

void DOnlineTranslator::setSourceTranslitEnabled(bool enable)
{
    m_sourceTranslitEnabled = enable;
}

bool DOnlineTranslator::isTranslationTranslitEnabled() const
{
    return m_translationTranslitEnabled;
}

void DOnlineTranslator::setTranslationTranslitEnabled(bool enable)
{
    m_translationTranslitEnabled = enable;
}

bool DOnlineTranslator::isSourceTranscriptionEnabled() const
{
    return m_sourceTranscriptionEnabled;
}

void DOnlineTranslator::setSourceTranscriptionEnabled(bool enable)
{
    m_sourceTranscriptionEnabled = enable;
}

bool DOnlineTranslator::isTranslationOptionsEnabled() const
{
    return m_translationOptionsEnabled;
}

void DOnlineTranslator::setTranslationOptionsEnabled(bool enable)
{
    m_translationOptionsEnabled = enable;
}

void DOnlineTranslator::setEngineUrl(Engine engine, const QString& url)
{
    switch (engine)
    {
        case LibreTranslate:
            m_libreUrl  = url;
            break;

        case Lingva:
            m_lingvaUrl = url;
            break;

        default:
            break;
    }
}

void DOnlineTranslator::setEngineApiKey(Engine engine, const QByteArray& apiKey)
{
    switch (engine)
    {
        case LibreTranslate:
            m_libreApiKey = apiKey;
            break;

        default:
            break;
    }
}

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

QString DOnlineTranslator::languageCode(Language lang)
{
    return s_genericLanguageCodes.value(lang);
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

DOnlineTranslator::Language DOnlineTranslator::language(const QString& langCode)
{
    return s_genericLanguageCodes.key(langCode, NoLanguage);
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

void DOnlineTranslator::skipGarbageText()
{
    m_translation.append(sender()->property(s_textProperty).toString());
}

void DOnlineTranslator::requestGoogleTranslate()
{
    const QString sourceText = sender()->property(s_textProperty).toString();

    // Generate API url

    QUrl url(QStringLiteral("https://translate.googleapis.com/translate_a/single"));

    url.setQuery(QStringLiteral("client=gtx&ie=UTF-8&oe=UTF-8&dt=bd&dt=ex&dt=ld&dt=md&dt=rw&dt=rm&dt=ss&dt=t&dt=at&dt=qc&sl=%1&tl=%2&hl=%3&q=%4")
                     .arg(languageApiCode(Google, m_sourceLang),
                          languageApiCode(Google, m_translationLang),
                          languageApiCode(Google, m_uiLang),
                          QString::fromUtf8(QUrl::toPercentEncoding(sourceText))));

    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::parseGoogleTranslate()
{
    m_currentReply->deleteLater();

    // Check for error

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        if (m_currentReply->error() == QNetworkReply::ServiceUnavailableError)
        {
            resetData(ServiceError, i18n("Error: Engine systems have detected suspicious traffic from your "
                                         "computer network. Please try your request again later."));
        }
        else
        {
            resetData(NetworkError, m_currentReply->errorString());
        }

        return;
    }

    // Check availability of service

    const QByteArray data = m_currentReply->readAll();

    if (data.startsWith('<'))
    {
        resetData(ServiceError, i18n("Error: Engine systems have detected suspicious traffic from your "
                                     "computer network. Please try your request again later."));
        return;
    }

    // Read Json

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(data);
    const QJsonArray jsonData        = jsonResponse.array();

    if (m_sourceLang == Auto)
    {
        // Parse language

        m_sourceLang = language(Google, jsonData.at(2).toString());

        if (m_sourceLang == NoLanguage)
        {
            resetData(ParsingError, i18n("Error: Unable to parse autodetected language"));
            return;
        }

        if (m_onlyDetectLanguage)
        {
            return;
        }
    }

    addSpaceBetweenParts(m_translation);
    addSpaceBetweenParts(m_translationTranslit);
    addSpaceBetweenParts(m_sourceTranslit);

    for (const QJsonValueRef translationData : jsonData.at(0).toArray())
    {
        const QJsonArray translationArray = translationData.toArray();
        m_translation.append(translationArray.at(0).toString());

        if (m_translationTranslitEnabled)
        {
            m_translationTranslit.append(translationArray.at(2).toString());
        }

        if (m_sourceTranslitEnabled)
        {
            m_sourceTranslit.append(translationArray.at(3).toString());
        }
    }

    if (m_source.size() >= s_googleTranslateLimit)
    {
        return;
    }

    // Translation options

    if (m_translationOptionsEnabled)
    {
        for (const QJsonValueRef typeOfSpeechData : jsonData.at(1).toArray())
        {
            const QJsonArray typeOfSpeechDataArray = typeOfSpeechData.toArray();
            const QString typeOfSpeech             = typeOfSpeechDataArray.at(0).toString();

            for (const QJsonValueRef wordData : typeOfSpeechDataArray.at(2).toArray())
            {
                const QJsonArray wordDataArray     = wordData.toArray();
                const QString word                 = wordDataArray.at(0).toString();
                const QString gender               = wordDataArray.at(4).toString();
                const QJsonArray translationsArray = wordDataArray.at(1).toArray();
                QStringList translations;
                translations.reserve(translationsArray.size());

                for (const QJsonValue &wordTranslation : translationsArray)
                {
                    translations.append(wordTranslation.toString());
                }

                m_translationOptions[typeOfSpeech].append({word, gender, translations});
            }
        }
    }
}

void DOnlineTranslator::requestYandexKey()
{
    const QUrl url(QStringLiteral("https://translate.yandex.com"));
    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::parseYandexKey()
{
    m_currentReply->deleteLater();

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    // Check availability of service

    const QByteArray webSiteData = m_currentReply->readAll();

    if (webSiteData.isEmpty()                        ||
        webSiteData.contains("<title>Oops!</title>") ||
        webSiteData.contains("<title>302 Found</title>"))
    {
        resetData(ServiceError, i18n("Error: Engine systems have detected suspicious traffic "
                                     "from your computer network. Please try your request again later."));
        return;
    }

    const QByteArray sidBeginString = "SID: '";
    const int sidBeginStringPos     = webSiteData.indexOf(sidBeginString);

    if (sidBeginStringPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to find Yandex SID in web version."));
        return;
    }

    const int sidBeginPosition = sidBeginStringPos + sidBeginString.size();
    const int sidEndPosition   = webSiteData.indexOf('\'', sidBeginPosition);

    if (sidEndPosition == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract Yandex SID from web version."));
        return;
    }

    // Yandex show reversed parts of session ID, need to decode

    const QString sid    = QString::fromUtf8(webSiteData.mid(sidBeginPosition,
                                                             sidEndPosition - sidBeginPosition));

    QStringList sidParts = sid.split(QLatin1Char('.'));

    for (int i = 0 ; i < sidParts.size() ; ++i)
    {
        std::reverse(sidParts[i].begin(), sidParts[i].end());
    }

    s_yandexKey = sidParts.join(QLatin1Char('.'));
}

void DOnlineTranslator::requestYandexTranslate()
{
    const QString sourceText = sender()->property(s_textProperty).toString();

    QString lang;

    if (m_sourceLang == Auto)
    {
        lang = languageApiCode(Yandex, m_translationLang);
    }
    else
    {
        lang = languageApiCode(Yandex, m_sourceLang) + QLatin1Char('-') + languageApiCode(Yandex, m_translationLang);
    }

    // Generate API url

    QUrl url(QStringLiteral("https://translate.yandex.net/api/v1/tr.json/translate"));

    url.setQuery(QStringLiteral("id=%1-2-0&srv=tr-text&text=%2&lang=%3")
                     .arg(s_yandexKey,
                          QString::fromUtf8(QUrl::toPercentEncoding(sourceText)),
                          lang));

    // Setup request

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(url);

    // Make reply

    m_currentReply = m_networkManager->post(request, QByteArray());
}

void DOnlineTranslator::parseYandexTranslate()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        // Network errors

        if (m_currentReply->error() < QNetworkReply::ContentAccessDenied)
        {
            resetData(NetworkError, m_currentReply->errorString());
            return;
        }

        // Parse data to get request error type

        s_yandexKey.clear();
        const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
        resetData(ServiceError, jsonResponse.object().value(QStringLiteral("message")).toString());

        return;
    }

    // Read Json

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject jsonData       = jsonResponse.object();

    // Parse language

    if (m_sourceLang == Auto)
    {
        QString sourceCode = jsonData.value(QStringLiteral("lang")).toString();
        sourceCode         = sourceCode.left(sourceCode.indexOf(QLatin1Char('-')));
        m_sourceLang       = language(Yandex, sourceCode);

        if (m_sourceLang == NoLanguage)
        {
            resetData(ParsingError, i18n("Error: Unable to parse autodetected language"));
            return;
        }

        if (m_onlyDetectLanguage)
        {
            return;
        }
    }

    // Parse translation data

    m_translation += jsonData.value(QStringLiteral("text")).toArray().at(0).toString();
}

void DOnlineTranslator::requestYandexSourceTranslit()
{
    requestYandexTranslit(m_sourceLang);
}

void DOnlineTranslator::parseYandexSourceTranslit()
{
    parseYandexTranslit(m_sourceTranslit);
}

void DOnlineTranslator::requestYandexTranslationTranslit()
{
    requestYandexTranslit(m_translationLang);
}

void DOnlineTranslator::parseYandexTranslationTranslit()
{
    parseYandexTranslit(m_translationTranslit);
}

void DOnlineTranslator::requestYandexDictionary()
{
    // Check if language is supported (need to check here because language may be autodetected)

    if (!isSupportDictionary(Yandex, m_sourceLang, m_translationLang) &&
        !m_source.contains(QLatin1Char(' ')))
    {
        auto* state = qobject_cast<QState*>(sender());
        state->addTransition(new QFinalState(state->parentState()));
        return;
    }

    // Generate API url

    const QString text = sender()->property(s_textProperty).toString();
    QUrl url(QStringLiteral("https://dictionary.yandex.net/dicservice.json/lookupMultiple"));

    url.setQuery(QStringLiteral("text=%1&ui=%2&dict=%3-%4")
                     .arg(QString::fromUtf8(QUrl::toPercentEncoding(text)),
                          languageApiCode(Yandex, m_uiLang),
                          languageApiCode(Yandex, m_sourceLang),
                          languageApiCode(Yandex, m_translationLang)));

    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::parseYandexDictionary()
{
    m_currentReply->deleteLater();

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    // Parse reply

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonValue jsonData        = jsonResponse.object().value(languageApiCode(Yandex, m_sourceLang) +
                                       QLatin1Char('-')                                                  +
                                       languageApiCode(Yandex, m_translationLang)).toObject().value(QStringLiteral("regular"));

    if (m_sourceTranscriptionEnabled)
    {
        m_sourceTranscription = jsonData.toArray().at(0).toObject().value(QStringLiteral("ts")).toString();
    }

    for (const QJsonValueRef typeOfSpeechData : jsonData.toArray())
    {
        QJsonObject typeOfSpeechObject = typeOfSpeechData.toObject();
        const QString typeOfSpeech     = typeOfSpeechObject.value(QStringLiteral("pos")).toObject().value(QStringLiteral("text")).toString();

        for (const QJsonValueRef wordData : typeOfSpeechObject.value(QStringLiteral("tr")).toArray())
        {
            // Parse translation options

            const QJsonObject wordObject       = wordData.toObject();
            const QString word                 = wordObject.value(QStringLiteral("text")).toString();
            const QString gender               = wordObject.value(QStringLiteral("gen")).toObject().value(QStringLiteral("text")).toString();
            const QJsonArray translationsArray = wordObject.value(QStringLiteral("mean")).toArray();
            QStringList translations;
            translations.reserve(translationsArray.size());

            for (const QJsonValue &wordTranslation : translationsArray)
            {
                translations.append(wordTranslation.toObject().value(QStringLiteral("text")).toString());
            }

            m_translationOptions[typeOfSpeech].append({word, gender, translations});
        }
    }
}

void DOnlineTranslator::requestBingCredentials()
{
    const QUrl url(QStringLiteral("https://www.bing.com/translator"));
    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::parseBingCredentials()
{
    m_currentReply->deleteLater();

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    const QByteArray webSiteData            = m_currentReply->readAll();
    const QByteArray credentialsBeginString = "var params_RichTranslateHelper = [";
    const int credentialsBeginPos           = webSiteData.indexOf(credentialsBeginString);

    if (credentialsBeginPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to find Bing credentials in web version."));
        return;
    }

    const int keyBeginPos = credentialsBeginPos + credentialsBeginString.size();
    const int keyEndPos   = webSiteData.indexOf(',', keyBeginPos);

    if (keyEndPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract Bing key from web version."));
        return;
    }

    s_bingKey               = webSiteData.mid(keyBeginPos, keyEndPos - keyBeginPos);
    const int tokenBeginPos = keyEndPos + 2; // Skip two symbols instead of one because the value is enclosed in quotes
    const int tokenEndPos   = webSiteData.indexOf('"', tokenBeginPos);

    if (tokenEndPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract Bing token from web version."));
        return;
    }

    s_bingToken          = webSiteData.mid(tokenBeginPos, tokenEndPos - tokenBeginPos);
    const int igBeginPos = webSiteData.indexOf("IG");
    const int igEndPos   = webSiteData.indexOf('"', igBeginPos + 2);

    if (igEndPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract additional Bing information from web version."));
        return;
    }

    s_bingIg              = QString::fromUtf8(webSiteData.mid(igBeginPos, igEndPos - igBeginPos));
    const int iidBeginPos = webSiteData.indexOf("data-iid");
    const int iidEndPos   = webSiteData.indexOf('"', iidBeginPos + 2);

    if (iidEndPos == -1)
    {
        resetData(ParsingError, i18n("Error: Unable to extract additional Bing information from web version."));
        return;
    }

    s_bingIid = QString::fromUtf8(webSiteData.mid(iidBeginPos, iidEndPos - iidBeginPos));
}

void DOnlineTranslator::requestBingTranslate()
{
    const QString sourceText = sender()->property(s_textProperty).toString();

    // Generate POST data

    const QByteArray postData = "&text="     + QUrl::toPercentEncoding(sourceText)
                              + "&fromLang=" + languageApiCode(Bing, m_sourceLang).toUtf8()
                              + "&to="       + languageApiCode(Bing, m_translationLang).toUtf8()
                              + "&token="    + s_bingToken
                              + "&key="      + s_bingKey;

    QUrl url(QStringLiteral("https://www.bing.com/ttranslatev3"));
    url.setQuery(QStringLiteral("IG=%1&IID=%2").arg(s_bingIg, s_bingIid));

    // Setup request

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::UserAgentHeader, QString::fromUtf8("%1/%2").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion()));
    request.setUrl(url);

    // Make reply

    m_currentReply = m_networkManager->post(request, postData);
}

void DOnlineTranslator::parseBingTranslate()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    // Parse translation data

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.array().first().toObject();

    if (m_sourceLang == Auto)
    {
        const QString langCode = responseObject.value(QStringLiteral("detectedLanguage")).toObject().value(QStringLiteral("language")).toString();
        m_sourceLang           = language(Bing, langCode);

        if (m_sourceLang == NoLanguage)
        {
            resetData(ParsingError, i18n("Error: Unable to parse autodetected language"));
            return;
        }

        if (m_onlyDetectLanguage)
            return;
    }

    const QJsonObject translationsObject = responseObject.value(QStringLiteral("translations")).toArray().first().toObject();
    m_translation                       += translationsObject.value(QStringLiteral("text")).toString();
    m_translationTranslit               += translationsObject.value(QStringLiteral("transliteration")).toObject().value(QStringLiteral("text")).toString();
}

void DOnlineTranslator::requestBingDictionary()
{
    // Check if language is supported (need to check here because language may be autodetected)

    if (!isSupportDictionary(Bing, m_sourceLang, m_translationLang) && !m_source.contains(QLatin1Char(' ')))
    {
        auto* state = qobject_cast<QState *>(sender());
        state->addTransition(new QFinalState(state->parentState()));
        return;
    }

    // Generate POST data

    const QByteArray postData = "&text=" + QUrl::toPercentEncoding(sender()->property(s_textProperty).toString())
                              + "&from=" + languageApiCode(Bing, m_sourceLang).toUtf8()
                              + "&to="   + languageApiCode(Bing, m_translationLang).toUtf8();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(QUrl(QStringLiteral("https://www.bing.com/tlookupv3")));

    m_currentReply = m_networkManager->post(request, postData);
}

void DOnlineTranslator::parseBingDictionary()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.array().first().toObject();

    for (const QJsonValueRef dictionaryData : responseObject.value(QStringLiteral("translations")).toArray())
    {
        const QJsonObject dictionaryObject = dictionaryData.toObject();
        const QString typeOfSpeech         = dictionaryObject.value(QStringLiteral("posTag")).toString().toLower();
        const QString word                 = dictionaryObject.value(QStringLiteral("displayTarget")).toString().toLower();
        const QJsonArray translationsArray = dictionaryObject.value(QStringLiteral("backTranslations")).toArray();
        QStringList translations;
        translations.reserve(translationsArray.size());

        for (const QJsonValue &wordTranslation : translationsArray)
        {
            translations.append(wordTranslation.toObject().value(QStringLiteral("displayText")).toString());
        }

        m_translationOptions[typeOfSpeech].append({word, {}, translations});
    }
}

void DOnlineTranslator::requestLibreLangDetection()
{
    const QString sourceText  = sender()->property(s_textProperty).toString();

    // Generate POST data

    const QByteArray postData = "&q="       + QUrl::toPercentEncoding(sourceText)
                              + "&api_key=" + m_libreApiKey;

    // Setup request

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(QUrl(QString::fromUtf8("%1/detect").arg(m_libreUrl)));

    // Make reply

    m_currentReply            = m_networkManager->post(request, postData);
}

void DOnlineTranslator::parseLibreLangDetection()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.array().first().toObject();

    if (m_sourceLang == Auto)
    {
        const QString langCode = responseObject.value(QStringLiteral("language")).toString();
        m_sourceLang           = language(LibreTranslate, langCode);

        if (m_sourceLang == NoLanguage)
        {
            resetData(ParsingError, i18n("Error: Unable to parse autodetected language"));
        }
    }
}

void DOnlineTranslator::requestLibreTranslate()
{
    const QString sourceText  = sender()->property(s_textProperty).toString();

    // Generate POST data

    const QByteArray postData = "&q="       + QUrl::toPercentEncoding(sourceText)
                              + "&source="  + languageApiCode(LibreTranslate, m_sourceLang).toUtf8()
                              + "&target="  + languageApiCode(LibreTranslate, m_translationLang).toUtf8()
                              + "&api_key=" + m_libreApiKey;

    // Setup request

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/x-www-form-urlencoded"));
    request.setUrl(QUrl(QString::fromUtf8("%1/translate").arg(m_libreUrl)));

    // Make reply

    m_currentReply            = m_networkManager->post(request, postData);
}

void DOnlineTranslator::parseLibreTranslate()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.object();
    m_translation                    = responseObject.value(QStringLiteral("translatedText")).toString();
}

void DOnlineTranslator::requestLingvaTranslate()
{
    const QString sourceText = sender()->property(s_textProperty).toString();

    // Generate API url

    QUrl url(QString::fromUtf8("%1/api/v1/%2/%3/%4")
                    .arg(m_lingvaUrl)
                    .arg(languageApiCode(Lingva, m_sourceLang))
                    .arg(languageApiCode(Lingva, m_translationLang))
                    .arg(QString::fromUtf8(QUrl::toPercentEncoding(sourceText))));

    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::parseLingvaTranslate()
{
    m_currentReply->deleteLater();

    // Check for errors

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());
        return;
    }

    // Parse translation data

    const QJsonDocument jsonResponse = QJsonDocument::fromJson(m_currentReply->readAll());
    const QJsonObject responseObject = jsonResponse.object();
    m_translation                    = responseObject.value(QStringLiteral("translation")).toString();
}

void DOnlineTranslator::buildGoogleStateMachine()
{
    // States (Google sends translation, translit and dictionary in one request,
    // that will be splitted into several by the translation limit)

    auto* translationState = new QState(m_stateMachine);
    auto* finalState       = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(translationState);

    translationState->addTransition(translationState, &QState::finished, finalState);

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::requestGoogleTranslate,
                             &DOnlineTranslator::parseGoogleTranslate,
                             m_source,
                             s_googleTranslateLimit);
}

void DOnlineTranslator::buildGoogleDetectStateMachine()
{
    // States

    auto* detectState  = new QState(m_stateMachine);
    auto* finalState   = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup detect state

    const QString text = m_source.left(getSplitIndex(m_source, s_googleTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::requestGoogleTranslate,
                             &DOnlineTranslator::parseGoogleTranslate,
                             text);
}

void DOnlineTranslator::buildYandexStateMachine()
{
    // States

    auto* keyState                  = new QState(m_stateMachine); // Generate SID from web version first to access API
    auto* translationState          = new QState(m_stateMachine);
    auto* sourceTranslitState       = new QState(m_stateMachine);
    auto* translationTranslitState  = new QState(m_stateMachine);
    auto* dictionaryState           = new QState(m_stateMachine);
    auto* finalState                = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(keyState);

    // Transitions

    keyState->addTransition(keyState, &QState::finished, translationState);
    translationState->addTransition(translationState, &QState::finished, sourceTranslitState);
    sourceTranslitState->addTransition(sourceTranslitState, &QState::finished, translationTranslitState);
    translationTranslitState->addTransition(translationTranslitState, &QState::finished, dictionaryState);
    dictionaryState->addTransition(dictionaryState, &QState::finished, finalState);

    // Setup key state

    if (s_yandexKey.isEmpty())
    {
        buildNetworkRequestState(keyState,
                                 &DOnlineTranslator::requestYandexKey,
                                 &DOnlineTranslator::parseYandexKey);
    }
    else
    {
        keyState->setInitialState(new QFinalState(keyState));
    }

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::requestYandexTranslate,
                             &DOnlineTranslator::parseYandexTranslate,
                             m_source,
                             s_yandexTranslateLimit);

    // Setup source translit state

    if (m_sourceTranslitEnabled)
    {
        buildSplitNetworkRequest(sourceTranslitState,
                                 &DOnlineTranslator::requestYandexSourceTranslit,
                                 &DOnlineTranslator::parseYandexSourceTranslit,
                                 m_source,
                                 s_yandexTranslitLimit);
    }
    else
    {
        sourceTranslitState->setInitialState(new QFinalState(sourceTranslitState));
    }

    // Setup translation translit state

    if (m_translationTranslitEnabled)
    {
        buildSplitNetworkRequest(translationTranslitState,
                                 &DOnlineTranslator::requestYandexTranslationTranslit,
                                 &DOnlineTranslator::parseYandexTranslationTranslit,
                                 m_translation,
                                 s_yandexTranslitLimit);
    }
    else
    {
        translationTranslitState->setInitialState(new QFinalState(translationTranslitState));
    }

    // Setup dictionary state

    if (m_translationOptionsEnabled && !isContainsSpace(m_source))
    {
        buildNetworkRequestState(dictionaryState,
                                 &DOnlineTranslator::requestYandexDictionary,
                                 &DOnlineTranslator::parseYandexDictionary,
                                 m_source);
    }
    else
    {
        dictionaryState->setInitialState(new QFinalState(dictionaryState));
    }
}

void DOnlineTranslator::buildYandexDetectStateMachine()
{
    // States

    auto* keyState    = new QState(m_stateMachine); // Generate SID from web version first to access API
    auto* detectState = new QState(m_stateMachine);
    auto* finalState  = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(keyState);

    // Transitions

    keyState->addTransition(keyState, &QState::finished, detectState);
    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup key state

    if (s_yandexKey.isEmpty())
    {
        buildNetworkRequestState(keyState,
                                 &DOnlineTranslator::requestYandexKey,
                                 &DOnlineTranslator::parseYandexKey);
    }
    else
    {
        keyState->setInitialState(new QFinalState(keyState));
    }

    // Setup detect state

    const QString text = m_source.left(getSplitIndex(m_source, s_yandexTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::requestYandexTranslate,
                             &DOnlineTranslator::parseYandexTranslate,
                             text);
}

void DOnlineTranslator::buildBingStateMachine()
{
    // States

    auto* credentialsState = new QState(m_stateMachine); // Generate credentials from web version first to access API
    auto* translationState = new QState(m_stateMachine);
    auto* dictionaryState  = new QState(m_stateMachine);
    auto* finalState       = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(credentialsState);

    // Transitions

    credentialsState->addTransition(credentialsState, &QState::finished, translationState);
    translationState->addTransition(translationState, &QState::finished, dictionaryState);
    dictionaryState->addTransition(dictionaryState, &QState::finished, finalState);

    // Setup credentials state

    if (s_bingKey.isEmpty() || s_bingToken.isEmpty())
    {
        buildNetworkRequestState(credentialsState,
                                 &DOnlineTranslator::requestBingCredentials,
                                 &DOnlineTranslator::parseBingCredentials);
    }
    else
    {
        credentialsState->setInitialState(new QFinalState(credentialsState));
    }

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::requestBingTranslate,
                             &DOnlineTranslator::parseBingTranslate,
                             m_source,
                             s_bingTranslateLimit);

    // Setup dictionary state

    if (m_translationOptionsEnabled && !isContainsSpace(m_source))
    {
        buildNetworkRequestState(dictionaryState,
                                 &DOnlineTranslator::requestBingDictionary,
                                 &DOnlineTranslator::parseBingDictionary,
                                 m_source);
    }
    else
    {
        dictionaryState->setInitialState(new QFinalState(dictionaryState));
    }
}

void DOnlineTranslator::buildBingDetectStateMachine()
{
    // States

    auto* detectState = new QState(m_stateMachine);
    auto* finalState  = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup translation state

    const QString text = m_source.left(getSplitIndex(m_source, s_bingTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::requestBingTranslate,
                             &DOnlineTranslator::parseBingTranslate,
                             text);
}

void DOnlineTranslator::buildLibreStateMachine()
{
    // States

    auto* languageDetectionState = new QState(m_stateMachine);
    auto* translationState       = new QState(m_stateMachine);
    auto* finalState             = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(languageDetectionState);

    // Transitions

    languageDetectionState->addTransition(languageDetectionState, &QState::finished, translationState);
    translationState->addTransition(translationState, &QState::finished, finalState);

    // Setup LibreTranslate lang code detection

    buildNetworkRequestState(languageDetectionState,
                             &DOnlineTranslator::requestLibreLangDetection,
                             &DOnlineTranslator::parseLibreLangDetection,
                             m_source);

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::requestLibreTranslate,
                             &DOnlineTranslator::parseLibreTranslate,
                             m_source,
                             s_libreTranslateLimit);
}

void DOnlineTranslator::buildLibreDetectStateMachine()
{
    // States

    auto* detectState = new QState(m_stateMachine);
    auto* finalState  = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup lang detection state

    const QString text = m_source.left(getSplitIndex(m_source, s_libreTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::requestLibreLangDetection,
                             &DOnlineTranslator::parseLibreLangDetection,
                             text);
}

void DOnlineTranslator::buildLingvaStateMachine()
{
    // States

    auto* translationState = new QState(m_stateMachine);
    auto* finalState       = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(translationState);

    // Transitions

    translationState->addTransition(translationState, &QState::finished, finalState);

    // Setup translation state

    buildSplitNetworkRequest(translationState,
                             &DOnlineTranslator::requestLingvaTranslate,
                             &DOnlineTranslator::parseLingvaTranslate,
                             m_source,
                             s_googleTranslateLimit);
}

void DOnlineTranslator::buildLingvaDetectStateMachine()
{
    // States

    auto* detectState = new QState(m_stateMachine);
    auto* finalState  = new QFinalState(m_stateMachine);
    m_stateMachine->setInitialState(detectState);

    detectState->addTransition(detectState, &QState::finished, finalState);

    // Setup lang detection state

    const QString text = m_source.left(getSplitIndex(m_source, s_googleTranslateLimit));

    buildNetworkRequestState(detectState,
                             &DOnlineTranslator::requestLingvaTranslate,
                             &DOnlineTranslator::parseLingvaTranslate,
                             text);
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
                    this, &DOnlineTranslator::skipGarbageText);

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

void DOnlineTranslator::requestYandexTranslit(Language language)
{
    // Check if language is supported (need to check here because language may be autodetected)

    if (!isSupportTranslit(Yandex, language))
    {
        auto* state = qobject_cast<QState *>(sender());
        state->addTransition(new QFinalState(state->parentState()));

        return;
    }

    const QString text = sender()->property(s_textProperty).toString();

    // Generate API url

    QUrl url(QStringLiteral("https://translate.yandex.net/translit/translit"));
    url.setQuery(QString::fromUtf8("text=%1&lang=%2")
                 .arg(QString::fromUtf8(QUrl::toPercentEncoding(text)))
                 .arg(languageApiCode(Yandex, language)));

    m_currentReply = m_networkManager->get(QNetworkRequest(url));
}

void DOnlineTranslator::parseYandexTranslit(QString& text)
{
    m_currentReply->deleteLater();

    if (m_currentReply->error() != QNetworkReply::NoError)
    {
        resetData(NetworkError, m_currentReply->errorString());

        return;
    }

    const QByteArray reply = m_currentReply->readAll();

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)

    text += QString::fromUtf8(reply.mid(1).chopped(1));

#else

    text += QString::fromUtf8(reply.mid(1));
    text.chop(1);

#endif

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
