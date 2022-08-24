/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-15
 * Description : multi-languages string editor
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QIcon>
#include <QLineEdit>
#include <QFontMetrics>
#include <QRect>
#include <QMenu>
#include <QListWidget>
#include <QWidgetAction>
#include <QStyle>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QApplication>
#include <QComboBox>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "donlinetranslator.h"
#include "spellchecksettings.h"
#include "dexpanderbox.h"

namespace Digikam
{

class Q_DECL_HIDDEN TranslateAction : public QWidgetAction
{
    Q_OBJECT

public:

    explicit TranslateAction(QObject* const parent)
        : QWidgetAction(parent)
    {
    }

    QWidget* createWidget(QWidget* parent) override
    {
        m_list         = new QListWidget(parent);
        QFontMetrics fontMt(m_list->font());
        QRect fontRect = fontMt.boundingRect(0, 0, m_list->width(), m_list->height(), 0, QLatin1String("XX-XX"));
        int width      =  m_list->contentsMargins().left() + m_list->contentsMargins().right();
        width         += fontRect.width() + m_list->verticalScrollBar()->height();
        m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_list->setFixedWidth(width);

        return m_list;
    }

public:

    QListWidget* m_list = nullptr;
};

// ------------------------------------------------------------------------

/**
 * NOTE: We cannot use KLocale::allLanguagesList() here because KDE only
 * support 2 characters country codes. XMP require 2+2 characters language+country
 * following RFC 3066.
 * The format is based on xx-XX using ISO-639 two-letter code with ISO-3166 two-letter country code.
 *
 * List version 1       : babelwiki.babelzilla.org/index.php?title=Language_codes
 * List version 2 update: xml.coverpages.org/TexinUsingLangID.html
 * List version 3 update: List of ISO 639-1 codes (https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes)
 *                        List of ISO 639-2 codes (https://en.wikipedia.org/wiki/List_of_ISO_639-2_codes)
 *                        List of ISO 639 macrolanguage (https://en.wikipedia.org/wiki/ISO_639_macrolanguage)
 */
const AltLangStrEdit::LanguageCodeMap AltLangStrEdit::s_rfc3066ForXMP =
{
    // The first one from the list is the Default Language code specified by XMP paper.

    { QLatin1String("x-default"), QString::fromUtf8(I18N_NOOP2("@info: country code", "Default Language")                         )},

    // the rest follow the RFC 3066.

    { QLatin1String("af-ZA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Afrikaans (South Africa)")                                        )},
    { QLatin1String("am-ET"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Amharic (Ethiopia)")                                              )},
    { QLatin1String("ar-AE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (UAE)")                                                    )},
    { QLatin1String("ar-BH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Bahrain)")                                                )},
    { QLatin1String("ar-DZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Algeria)")                                                )},
    { QLatin1String("ar-EG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Egypt)")                                                  )},
    { QLatin1String("ar-IQ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Iraq)")                                                   )},
    { QLatin1String("ar-IL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Israel)")                                                 )},
    { QLatin1String("ar-JO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Jordan)")                                                 )},
    { QLatin1String("ar-KW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Kuwait)")                                                 )},
    { QLatin1String("ar-LB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Lebanon)")                                                )},
    { QLatin1String("ar-LY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Libya)")                                                  )},
    { QLatin1String("ar-MA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Morocco)")                                                )},
    { QLatin1String("ar-MR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Mauritania)")                                             )},
    { QLatin1String("ar-PS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Palestinian West Bank and Gaza)")                         )},
    { QLatin1String("ar-OM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Oman)")                                                   )},
    { QLatin1String("ar-QA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Qatar)")                                                  )},
    { QLatin1String("ar-SA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Saudi Arabia)")                                           )},
    { QLatin1String("ar-SD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Sudan)")                                                  )},
    { QLatin1String("ar-SO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Somalia)")                                                )},
    { QLatin1String("ar-SY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Syria)")                                                  )},
    { QLatin1String("ar-TD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Chad)")                                                   )},
    { QLatin1String("ar-TN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Tunisia)")                                                )},
    { QLatin1String("ar-YE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Arabic (Yemen)")                                                  )},
    { QLatin1String("as-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Assamese (India)")                                                )},
    { QLatin1String("az-AZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Azerbaijani (Republic of Azerbaijan)")                            )},
    { QLatin1String("ba-RU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Bashkir (Russia)")                                                )},
    { QLatin1String("bn-BD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Bengali (Bangladesh)")                                            )},
    { QLatin1String("bn-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Bengali (India)")                                                 )},
    { QLatin1String("bn-SG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Bengali (Singapore)")                                             )},
    { QLatin1String("be-BY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Belarusian (Belarus)")                                            )},
    { QLatin1String("bg-BG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Bulgarian (Bulgaria)")                                            )},
    { QLatin1String("bn-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Bengali (India)")                                                 )},
    { QLatin1String("bo-BT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tibetan (Bhutan)")                                                )},
    { QLatin1String("bo-CN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tibetan (People's Republic of China)")                            )},
    { QLatin1String("br-FR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Breton (France)")                                                 )},
    { QLatin1String("bs-BA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Bosnian (Bosnia and Herzegovina)")                                )},
    { QLatin1String("bs-BG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Bulgarian (Bulgaria)")                                            )},
    { QLatin1String("ca-AD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Catalan (Andorra)")                                               )},
    { QLatin1String("ca-ES"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Catalan (Spain)")                                                 )},
    { QLatin1String("ca-FR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Catalan (France)")                                                )},
    { QLatin1String("ch-GU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Chamorro (Guam)")                                                 )},
    { QLatin1String("ch-MP"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Chamorro (Northern Mariana Islands)")                             )},
    { QLatin1String("co-FR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Corsican (France)")                                               )},
    { QLatin1String("cs-CZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Czech (Czechia)")                                                 )},
    { QLatin1String("cy-GB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Welsh (United Kingdom)")                                          )},
    { QLatin1String("da-DK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Danish (Denmark)")                                                )},
    { QLatin1String("da-GL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Danish (Greenland)")                                              )},
    { QLatin1String("de-AT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "German (Austria)")                                                )},
    { QLatin1String("de-CH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "German (Switzerland)")                                            )},
    { QLatin1String("de-DE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "German (Germany)")                                                )},
    { QLatin1String("de-DK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "German (Denmark)")                                                )},
    { QLatin1String("de-LI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "German (Liechtenstein)")                                          )},
    { QLatin1String("de-LU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "German (Luxembourg)")                                             )},
    { QLatin1String("el-CY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Greek (Cyprus)")                                                  )},
    { QLatin1String("el-GR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Greek (Greece)")                                                  )},
    { QLatin1String("en-AG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Antigua and Barbuda)")                                   )},
    { QLatin1String("en-AI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Anguilla)")                                              )},
    { QLatin1String("en-AS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (American Samoa)")                                        )},
    { QLatin1String("en-AU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Australia)")                                             )},
    { QLatin1String("en-BB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Barbados)")                                              )},
    { QLatin1String("en-BM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Bermuda)")                                               )},
    { QLatin1String("en-BN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Brunei)")                                                )},
    { QLatin1String("en-BS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Bahamas)")                                               )},
    { QLatin1String("en-BW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Botswana)")                                              )},
    { QLatin1String("en-BZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Belize)")                                                )},
    { QLatin1String("en-CA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Canada)")                                                )},
    { QLatin1String("en-CB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Caribbean)")                                             )},
    { QLatin1String("en-CK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Cook Islands)")                                          )},
    { QLatin1String("en-CM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Cameroon)")                                              )},
    { QLatin1String("en-DM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Dominica)")                                              )},
    { QLatin1String("en-ER"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Eritrea)")                                               )},
    { QLatin1String("en-ET"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Ethiopia)")                                              )},
    { QLatin1String("en-FJ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Fiji)")                                                  )},
    { QLatin1String("en-FK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Falkland Islands)")                                      )},
    { QLatin1String("en-FM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Micronesia)")                                            )},
    { QLatin1String("en-GB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (United Kingdom)")                                        )},
    { QLatin1String("en-GD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Grenada)")                                               )},
    { QLatin1String("en-GH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Ghana)")                                                 )},
    { QLatin1String("en-GI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Gibraltar)")                                             )},
    { QLatin1String("en-GM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Gambia)")                                                )},
    { QLatin1String("en-GU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Guam)")                                                  )},
    { QLatin1String("en-GY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Guyana)")                                                )},
    { QLatin1String("en-IE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Ireland)")                                               )},
    { QLatin1String("en-IL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Israel)")                                                )},
    { QLatin1String("en-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (India)")                                                 )},
    { QLatin1String("en-IO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (British Indian Ocean Territ)")                           )},
    { QLatin1String("en-JM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Jamaica)")                                               )},
    { QLatin1String("en-KR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Kenya)")                                                 )},
    { QLatin1String("en-KI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Kiribati)")                                              )},
    { QLatin1String("en-KN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (St. Kitts-Nevis)")                                       )},
    { QLatin1String("en-KY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Cayman Islands)")                                        )},
    { QLatin1String("en-LC"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (St. Lucia)")                                             )},
    { QLatin1String("en-LR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Liberia)")                                               )},
    { QLatin1String("en-LS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Lesotho)")                                               )},
    { QLatin1String("en-MP"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Northern Mariana Islands)")                              )},
    { QLatin1String("en-MS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Montserrat)")                                            )},
    { QLatin1String("en-MT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Malta)")                                                 )},
    { QLatin1String("en-MU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Mauritius)")                                             )},
    { QLatin1String("en-MW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Malawi)")                                                )},
    { QLatin1String("en-MY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Malaysia)")                                              )},
    { QLatin1String("en-NA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Namibia)")                                               )},
    { QLatin1String("en-NF"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Norfolk Island)")                                        )},
    { QLatin1String("en-NG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Nigeria)")                                               )},
    { QLatin1String("en-NR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Nauru)")                                                 )},
    { QLatin1String("en-NU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Niue)")                                                  )},
    { QLatin1String("en-NZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (New Zealand)")                                           )},
    { QLatin1String("en-PG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Papua New Guinea)")                                      )},
    { QLatin1String("en-PH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Philippines)")                                           )},
    { QLatin1String("en-PK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Pakistan)")                                              )},
    { QLatin1String("en-PN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Pitcairn)")                                              )},
    { QLatin1String("en-PR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Puerto Rico)")                                           )},
    { QLatin1String("en-PW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Palau)")                                                 )},
    { QLatin1String("en-RW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Rwanda)")                                                )},
    { QLatin1String("en-SB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Solomon Islands)")                                       )},
    { QLatin1String("en-SC"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Seychelles)")                                            )},
    { QLatin1String("en-SG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Singapore)")                                             )},
    { QLatin1String("en-SH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (St. Helena)")                                            )},
    { QLatin1String("en-SL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Sierra Leone)")                                          )},
    { QLatin1String("en-SO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Somalia)")                                               )},
    { QLatin1String("en-SZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Swaziland)")                                             )},
    { QLatin1String("en-TC"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Turks and Caicos Islands)")                              )},
    { QLatin1String("en-TK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Tokelau)")                                               )},
    { QLatin1String("en-TO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Tonga)")                                                 )},
    { QLatin1String("en-TT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Trinidad)")                                              )},
    { QLatin1String("en-UG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Uganda)")                                                )},
    { QLatin1String("en-US"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (United States)")                                         )},
    { QLatin1String("en-VC"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (St. Vincent and the Grenadi)")                           )},
    { QLatin1String("en-VG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (British Virgin Islands)")                                )},
    { QLatin1String("en-VI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (U.S. Virgin Islands)")                                   )},
    { QLatin1String("en-VU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Vanuatu)")                                               )},
    { QLatin1String("en-WS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Western Samoa)")                                         )},
    { QLatin1String("en-ZA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (South Africa)")                                          )},
    { QLatin1String("en-ZM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Zambia)")                                                )},
    { QLatin1String("en-ZW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "English (Zimbabwe)")                                              )},
    { QLatin1String("eo-FR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Esperanto (France)")                                              )},
    { QLatin1String("es-AR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Argentina)")                                             )},
    { QLatin1String("es-BO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Bolivia)")                                               )},
    { QLatin1String("es-CL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Chile)")                                                 )},
    { QLatin1String("es-CO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Colombia)")                                              )},
    { QLatin1String("es-CR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Costa Rica)")                                            )},
    { QLatin1String("es-CU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Cuba)")                                                  )},
    { QLatin1String("es-DO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Dominican Republic)")                                    )},
    { QLatin1String("es-EC"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Ecuador)")                                               )},
    { QLatin1String("es-ES"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Spain)")                                                 )},
    { QLatin1String("es-GQ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Equatorial Guinea)")                                     )},
    { QLatin1String("es-GT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Guatemala)")                                             )},
    { QLatin1String("es-HN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Honduras)")                                              )},
    { QLatin1String("es-MX"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Mexico)")                                                )},
    { QLatin1String("es-NI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Nicaragua)")                                             )},
    { QLatin1String("es-PA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Panama)")                                                )},
    { QLatin1String("es-PE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Peru)")                                                  )},
    { QLatin1String("es-PR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Puerto Rico)")                                           )},
    { QLatin1String("es-PY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Paraguay)")                                              )},
    { QLatin1String("es-SV"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (El Salvador)")                                           )},
    { QLatin1String("es-US"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (United States)")                                         )},
    { QLatin1String("es-UY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Uruguay)")                                               )},
    { QLatin1String("es-VE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Spanish (Venezuela)")                                             )},
    { QLatin1String("et-EE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Estonian (Estonia)")                                              )},
    { QLatin1String("eu-ES"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Basque (Basque Country)")                                         )},
    { QLatin1String("fa-IR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Persian (Iran)")                                                  )},
    { QLatin1String("fi-FI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Finnish (Finland)")                                               )},
    { QLatin1String("fj-FJ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Fijian (Fiji)")                                                   )},
    { QLatin1String("fo-FO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Faeroese (Faero Islands)")                                        )},
    { QLatin1String("fr-AD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Andorra)")                                                )},
    { QLatin1String("fr-BE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Belgium)")                                                )},
    { QLatin1String("fr-BF"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Burkina Faso)")                                           )},
    { QLatin1String("fr-BI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Burundi)")                                                )},
    { QLatin1String("fr-BJ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Benin)")                                                  )},
    { QLatin1String("fr-CA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Canada)")                                                 )},
    { QLatin1String("fr-CD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Democratic Republic of Congo)")                           )},
    { QLatin1String("fr-CF"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Central African Republic)")                               )},
    { QLatin1String("fr-CG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Congo)")                                                  )},
    { QLatin1String("fr-CH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Switzerland)")                                            )},
    { QLatin1String("fr-CI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Cote d'Ivoire)")                                          )},
    { QLatin1String("fr-CM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Cameroon)")                                               )},
    { QLatin1String("fr-DJ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Djibouti)")                                               )},
    { QLatin1String("fr-FR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (France)")                                                 )},
    { QLatin1String("fr-GA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Gabon)")                                                  )},
    { QLatin1String("fr-GB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (United Kingdom)")                                         )},
    { QLatin1String("fr-GF"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (French Guiana)")                                          )},
    { QLatin1String("fr-GN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Guinea)")                                                 )},
    { QLatin1String("fr-GP"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Guadeloupe)")                                             )},
    { QLatin1String("fr-HT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Haiti)")                                                  )},
    { QLatin1String("fr-IT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Italy)")                                                  )},
    { QLatin1String("fr-KM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Comoros Islands)")                                        )},
    { QLatin1String("fr-LB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Lebanon)")                                                )},
    { QLatin1String("fr-LU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Luxembourg)")                                             )},
    { QLatin1String("fr-MC"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Monaco)")                                                 )},
    { QLatin1String("fr-MG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Madagascar)")                                             )},
    { QLatin1String("fr-ML"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Mali)")                                                   )},
    { QLatin1String("fr-MQ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Martinique)")                                             )},
    { QLatin1String("fr-NC"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (New Caledonia)")                                          )},
    { QLatin1String("fr-PF"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (French Polynesia)")                                       )},
    { QLatin1String("fr-PM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (St. Pierre and Miquelon)")                                )},
    { QLatin1String("fr-RE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Reunion)")                                                )},
    { QLatin1String("fr-RW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Rwanda)")                                                 )},
    { QLatin1String("fr-SC"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Seychelles)")                                             )},
    { QLatin1String("fr-TD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Chad)")                                                   )},
    { QLatin1String("fr-TG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Togo)")                                                   )},
    { QLatin1String("fr-VU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Vanuatu)")                                                )},
    { QLatin1String("fr-WF"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Wallis and Futuna)")                                      )},
    { QLatin1String("fr-YT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "French (Mayotte)")                                                )},
    { QLatin1String("fy-NL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Frisian (Netherlands)")                                           )},
    { QLatin1String("ga-GB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Irish (United Kingdom)")                                          )},
    { QLatin1String("ga-IE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Irish (Ireland)")                                                 )},
    { QLatin1String("gd-GB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Gaelic (United Kingdom)")                                         )},
    { QLatin1String("gl-ES"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Galician (Galicia)")                                              )},
    { QLatin1String("gu-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Gujarati (India)")                                                )},
    { QLatin1String("gv-GB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Manx (United Kingdom)")                                           )},
    { QLatin1String("ha-TD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Hausa (Chad)")                                                    )},
    { QLatin1String("he-IL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Modern Hebrew (Israel)")                                          )},
    { QLatin1String("hi-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Hindi (India)")                                                   )},
    { QLatin1String("hr-BA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Croatian (Bosnia and Herzegovina, Latin)")                        )},
    { QLatin1String("hr-HR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Croatian (Croatia)")                                              )},
    { QLatin1String("ht-FR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Haitian Creole (France)")                                         )},
    { QLatin1String("hu-HU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Hungarian (Hungary)")                                             )},
    { QLatin1String("hu-SI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Hungarian (Slovenia)")                                            )},
    { QLatin1String("hy-AM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Armenian (Armenia)")                                              )},
    { QLatin1String("id-ID"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Indonesian (Bahasa Indonesia)")                                   )},
    { QLatin1String("ig-NG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Igbo (Nigeria)")                                                  )},
    { QLatin1String("ii-CN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sichuan Yi (People's Republic of China)")                         )},
    { QLatin1String("is-IS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Icelandic (Iceland)")                                             )},
    { QLatin1String("it-CH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Italian (Switzerland)")                                           )},
    { QLatin1String("it-HR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Italian (Croatia)")                                               )},
    { QLatin1String("it-IT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Italian (Italy)")                                                 )},
    { QLatin1String("it-SI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Italian (Slovenia)")                                              )},
    { QLatin1String("it-SM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Italian (San Marino)")                                            )},
    { QLatin1String("iw-IL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Hebrew (Israel)")                                                 )},
    { QLatin1String("ja-JP"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Japanese (Japan)")                                                )},
    { QLatin1String("jv-ID"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Modern Javanese (Indonesia)")                                     )},
    { QLatin1String("jw-ID"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Javanese (Indonesia)")                                            )},
    { QLatin1String("ka-GE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Georgian (Georgia)")                                              )},
    { QLatin1String("kk-KZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Kazakh (Kazakhstan)")                                             )},
    { QLatin1String("kl-GL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Greenlandic (Greenland)")                                         )},
    { QLatin1String("km-KH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Khmer (Cambodia)")                                                )},
    { QLatin1String("kn-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Kannada (India)")                                                 )},
    { QLatin1String("ko-KR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Korean (South Korea)")                                            )},
    { QLatin1String("ko-KP"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Korean (North Korea)")                                            )},
    { QLatin1String("ky-KG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Kyrgyz (Kyrgyzstan)")                                             )},
    { QLatin1String("ku-IR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Cornish (United Kingdom)")                                        )},
    { QLatin1String("kw-GB"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Kurdish (Iran)")                                                  )},
    { QLatin1String("la-IT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Latin (Italy)")                                                   )},
    { QLatin1String("lb-LU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Luxembourgish (Luxembourg)")                                      )},
    { QLatin1String("ln-CD"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Lingala (Democratic Republic of Congo)")                          )},
    { QLatin1String("ln-CG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Lingala (Congo)")                                                 )},
    { QLatin1String("lo-LA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Lao (Lao PDR)")                                                   )},
    { QLatin1String("lt-LT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Lithuanian (Lithuania)")                                          )},
    { QLatin1String("lv-LV"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Latvian (Latvia)")                                                )},
    { QLatin1String("mg-MG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Malagasy (Madagascar)")                                           )},
    { QLatin1String("mi-NZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Maori (New Zealand)")                                             )},
    { QLatin1String("mk-MK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Macedonian (Macedonia)")                                          )},
    { QLatin1String("ml-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Malayalam (India)")                                               )},
    { QLatin1String("mn-CN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Mongolian (People's Republic of China)")                          )},
    { QLatin1String("mn-MN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Mongolian (Mongolia)")                                            )},
    { QLatin1String("mr-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Marathi (India)")                                                 )},
    { QLatin1String("ms-BN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Malay (Brunei Darussalam)")                                       )},
    { QLatin1String("ms-MY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Malay (Malaysia)")                                                )},
    { QLatin1String("ms-SG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Malay (Singapore)")                                               )},
    { QLatin1String("mt-MT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Maltese (Malta)")                                                 )},
    { QLatin1String("my-MM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Burmese (Myanmar)")                                               )},
    { QLatin1String("nb-NO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Norwegian Bokmål (Norway)")                                       )},
    { QLatin1String("ne-NP"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Nepali (Nepal)")                                                  )},
    { QLatin1String("nl-AN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Dutch (Netherlands Antilles)")                                    )},
    { QLatin1String("nl-AW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Dutch (Aruba)")                                                   )},
    { QLatin1String("nl-BE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Dutch (Belgium)")                                                 )},
    { QLatin1String("nl-NL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Dutch (Netherlands)")                                             )},
    { QLatin1String("nl-SR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Dutch (Suriname)")                                                )},
    { QLatin1String("no-NO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Norwegian (Norway)")                                              )},
    { QLatin1String("nn-NO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Norwegian Nynorsk (Norway)")                                      )},
    { QLatin1String("ns-ZA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sesotho sa Leboa (South Africa)")                                 )},
    { QLatin1String("ny-ZM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Chewa (Zambia)")                                                  )},
    { QLatin1String("oc-FR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Occitan (France)")                                                )},
    { QLatin1String("or-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Oriya (India)")                                                   )},
    { QLatin1String("pa-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Punjabi (India)")                                                 )},
    { QLatin1String("pl-PL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Polish (Poland)")                                                 )},
    { QLatin1String("ps-AF"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Pashto (Afghanistan)")                                            )},
    { QLatin1String("pt-AO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Portuguese (Angola)")                                             )},
    { QLatin1String("pt-BR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Portuguese (Brazil)")                                             )},
    { QLatin1String("pt-CV"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Portuguese (Cape Verde Islands)")                                 )},
    { QLatin1String("pt-GW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Portuguese (Guinea-Bissau)")                                      )},
    { QLatin1String("pt-MZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Portuguese (Mozambique)")                                         )},
    { QLatin1String("pt-PT"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Portuguese (Portugal)")                                           )},
    { QLatin1String("pt-ST"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Portuguese (São Tomé e Principe)")                                )},
    { QLatin1String("rm-CH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Romansh (Switzerland)")                                           )},
    { QLatin1String("ro-RO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Romanian (Romania)")                                              )},
    { QLatin1String("ru-RU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Russian (Russia)")                                                )},
    { QLatin1String("rw-RW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Kinyarwanda (Rwanda)")                                            )},
    { QLatin1String("sa-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sanskrit (India)")                                                )},
    { QLatin1String("sd-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sindhi (India)")                                                  )},
    { QLatin1String("sd-PK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sindhi (Pakistan)")                                               )},
    { QLatin1String("se-FI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sami (Northern, Finland)")                                        )},
    { QLatin1String("se-NO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sami (Northern, Norway)")                                         )},
    { QLatin1String("se-SE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sami (Northern, Sweden)")                                         )},
    { QLatin1String("si-LK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sinhala (Sri Lanka)")                                             )},
    { QLatin1String("sk-SK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Slovak (Slovakia)")                                               )},
    { QLatin1String("sl-SI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Slovenian (Slovenia)")                                            )},
    { QLatin1String("sm-WS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Samoan (Samoa)")                                                  )},
    { QLatin1String("sn-ZW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Shona (Zimbabwe)")                                                )},
    { QLatin1String("so-SO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Somali  (Somalia)")                                               )},
    { QLatin1String("sq-AL"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Albanian (Albania)")                                              )},
    { QLatin1String("sr-BA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Serbian (Bosnia-Herzegovina)")                                    )},
    { QLatin1String("sr-RS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Serbian (Serbia)")                                                )},
    { QLatin1String("sr-SP"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Serbian (Serbia and Montenegro)")                                 )},
    { QLatin1String("sr-YU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Serbian (Yugoslavia)")                                            )},
    { QLatin1String("ss-SZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Swati (Swaziland)")                                               )},
    { QLatin1String("ss-ZA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Swati (South Africa)")                                            )},
    { QLatin1String("st-LS"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sotho (Lesotho)")                                                 )},
    { QLatin1String("su-ID"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Sundanese (Indonesia)")                                           )},
    { QLatin1String("sv-FI"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Swedish (Finland)")                                               )},
    { QLatin1String("sv-SE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Swedish (Sweden)")                                                )},
    { QLatin1String("sw-KE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Swahili (Kenya)")                                                 )},
    { QLatin1String("sw-TZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Swahili (Tanzania)")                                              )},
    { QLatin1String("ta-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tamil (India)")                                                   )},
    { QLatin1String("ta-SG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tamil (Singapore)")                                               )},
    { QLatin1String("te-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Telugu (India)")                                                  )},
    { QLatin1String("tg-TJ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tajik (Tajikistan)")                                              )},
    { QLatin1String("th-TH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Thai (Thailand)")                                                 )},
    { QLatin1String("tk-TM"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Turkmen (Turkmenistan)")                                          )},
    { QLatin1String("tl-PH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tagalog (Philippines)")                                           )},
    { QLatin1String("tn-BW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Setswana Tswana (Botswana)")                                      )},
    { QLatin1String("tn-ZA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Setswana Tswana (South Africa)")                                  )},
    { QLatin1String("to-TO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tongan (Tonga)")                                                  )},
    { QLatin1String("tr-BG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Turkish (Bulgaria)")                                              )},
    { QLatin1String("tr-CY"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Turkish (Cyprus)")                                                )},
    { QLatin1String("tr-TR"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Turkish (Turkey)")                                                )},
    { QLatin1String("tt-RU"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tatar (Russia)")                                                  )},
    { QLatin1String("ty-PF"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Tahitian (French Polynesia)")                                     )},
    { QLatin1String("ug-CN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Uighur (People's Republic of China)")                             )},
    { QLatin1String("uk-UA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Ukrainian (Ukraine)")                                             )},
    { QLatin1String("ur-IN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Urdu (India)")                                                    )},
    { QLatin1String("ur-PK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Urdu (Pakistan)")                                                 )},
    { QLatin1String("uz-UZ"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Uzbek (Uzbekistan)")                                              )},
    { QLatin1String("vi-VN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Vietnamese (Vietnam)")                                            )},
    { QLatin1String("wo-SN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Wolof (Senegal)")                                                 )},
    { QLatin1String("xh-ZA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "IsiXhosa Xhosa (South Africa)")                                   )},
    { QLatin1String("yi-DE"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Yiddish (Germany)")                                               )},
    { QLatin1String("yo-NG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Yoruba (Nigeria)")                                                )},
    { QLatin1String("zh-CN"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Chinese (People's Republic of China)")                            )},
    { QLatin1String("zh-HK"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Chinese (Hong Kong SAR, People's Republic of China)")             )},
    { QLatin1String("zh-MO"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Chinese (Macao SAR)")                                             )},
    { QLatin1String("zh-SG"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Chinese (Singapore)")                                             )},
    { QLatin1String("zh-TW"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Chinese (Taiwan)")                                                )},
    { QLatin1String("zn-CH"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "Simplified Chinese (People's Republic of China)")                 )},
    { QLatin1String("zu-ZA"),     QString::fromUtf8(I18N_NOOP2("@info: country code", "isiZulu Zulu (South Africa)")                                     )}
};

// ------------------------------------------------------------------------

class Q_DECL_HIDDEN AltLangStrEdit::Private
{
public:

    explicit Private()
      : currentLanguage (QLatin1String("x-default")),
        linesVisible    (0),
        grid            (nullptr),
        titleWidget     (nullptr),
        delValueButton  (nullptr),
        translateButton (nullptr),
        settingsButton  (nullptr),
        valueEdit       (nullptr),
        languageCB      (nullptr),
        translateAction (nullptr),
        trengine        (nullptr)
    {
    }

    ~Private()
    {
    }

public:

    QString                        currentLanguage;

    uint                           linesVisible;

    QGridLayout*                   grid;

    QWidget*                       titleWidget;

    QToolButton*                   delValueButton;
    QToolButton*                   translateButton;
    QToolButton*                   settingsButton;

    DTextEdit*                     valueEdit;

    QComboBox*                     languageCB;
    TranslateAction*               translateAction;
    DOnlineTranslator*             trengine;
    QString                        trCode;

    MetaEngine::AltLangMap         values;
};

AltLangStrEdit::AltLangStrEdit(QWidget* const parent, unsigned int lines)
    : QWidget(parent),
      d      (new Private)
{
    d->titleWidget    = new QLabel(this);

    d->delValueButton = new QToolButton(this);
    d->delValueButton->setIcon(QIcon::fromTheme(QLatin1String("edit-clear")));
    d->delValueButton->setToolTip(i18nc("@info: language edit widget", "Remove entry for this language"));
    d->delValueButton->setEnabled(false);

    d->translateButton = new QToolButton(this);
    d->translateButton->setIcon(QIcon::fromTheme(QLatin1String("language-chooser")));
    d->translateButton->setEnabled(false);
    d->translateButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu* const menu  = new QMenu(d->translateButton);
    d->translateAction = new TranslateAction(d->translateButton);
    menu->addAction(d->translateAction);
    d->translateButton->setMenu(menu);

    d->settingsButton  = new QToolButton(this);
    d->settingsButton->setIcon(QIcon::fromTheme(QLatin1String("configure")));
    d->settingsButton->setToolTip(i18n("Open localize setup"));

    d->trengine        = new DOnlineTranslator(this);

    d->languageCB      = new QComboBox(this);
    d->languageCB->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    d->languageCB->setWhatsThis(i18nc("@info: language edit widget", "Select item language here."));

    d->valueEdit       = new DTextEdit(lines, this);
    d->valueEdit->setAcceptRichText(false);

    // --------------------------------------------------------

    d->grid            = new QGridLayout(this);
    d->grid->setAlignment(Qt::AlignTop);
    d->grid->addWidget(d->languageCB,      0, 2, 1,  1);
    d->grid->addWidget(d->delValueButton,  0, 3, 1,  1);
    d->grid->addWidget(d->translateButton, 0, 4, 1,  1);
    d->grid->addWidget(d->settingsButton,  0, 5, 1,  1);
    d->grid->addWidget(d->valueEdit,       1, 0, 1, -1);
    d->grid->setColumnStretch(1, 10);
    d->grid->setContentsMargins(QMargins());
    d->grid->setSpacing(qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing)));

    populateLangAltListEntries();
    slotLocalizeChanged();

    // --------------------------------------------------------

    connect(d->languageCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &AltLangStrEdit::slotSelectionChanged);

    connect(d->delValueButton, &QToolButton::clicked,
            this, &AltLangStrEdit::slotDeleteValue);

    connect(d->settingsButton, &QToolButton::clicked,
            this, &AltLangStrEdit::slotOpenLocalizeSetup);

    connect(d->translateAction->m_list, &QListWidget::itemClicked,
            this, &AltLangStrEdit::slotTranslate);

    connect(d->valueEdit, &QTextEdit::textChanged,
            this, &AltLangStrEdit::slotTextChanged);

    connect(d->trengine, &DOnlineTranslator::signalFinished,
            this, &AltLangStrEdit::slotTranslationFinished);

    connect(d->translateButton, &QToolButton::pressed,
            d->translateButton, &QToolButton::showMenu);

    connect(SpellCheckSettings::instance(), &SpellCheckSettings::signalSettingsChanged,
            this, &AltLangStrEdit::slotLocalizeChanged);
}

AltLangStrEdit::~AltLangStrEdit()
{
    delete d;
}

QString AltLangStrEdit::languageNameRFC3066(const QString& code)
{
    LanguageCodeMap::const_iterator it = s_rfc3066ForXMP.find(code);

    if (it != s_rfc3066ForXMP.end())
    {
        return it.value();
    }

    return QString();
}

QStringList AltLangStrEdit::allLanguagesRFC3066()
{
    return s_rfc3066ForXMP.keys();
}

void AltLangStrEdit::slotEnabledInternalWidgets(bool b)
{
    d->languageCB->setEnabled(b);
    d->delValueButton->setEnabled(b);
    d->translateButton->setEnabled(b);
    d->valueEdit->setEnabled(b);
}

QString AltLangStrEdit::currentLanguageCode() const
{
    return d->currentLanguage;
}

void AltLangStrEdit::setCurrentLanguageCode(const QString& lang)
{
    if      (d->currentLanguage.isEmpty())
    {
        d->currentLanguage = QLatin1String("x-default");
    }
    else if (!lang.isEmpty())
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
    QLabel* const tlabel = new QLabel(this);
    tlabel->setText(title);
    setTitleWidget(tlabel);
}

void AltLangStrEdit::setTitleWidget(QWidget* const twdg)
{
    if (d->titleWidget)
    {
        delete d->titleWidget;
    }

    d->titleWidget = twdg;
    d->grid->addWidget(d->titleWidget, 0, 0, 1, 1);
}

QWidget* AltLangStrEdit::titleWidget() const
{
    return d->titleWidget;
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

    Q_EMIT signalValueDeleted(d->currentLanguage);
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
    d->translateButton->setEnabled(!text.isNull());

    d->valueEdit->blockSignals(false);

    d->languageCB->setToolTip(s_rfc3066ForXMP.value(d->currentLanguage));

    // NOTE: if no specific language is set, spell-checker failback to auto-detection.

    if (d->currentLanguage == QLatin1String("x-default"))
    {
        d->valueEdit->setCurrentLanguage(QString());
    }
    else
    {
        d->valueEdit->setCurrentLanguage(d->currentLanguage.left(2));
    }

    Q_EMIT signalSelectionChanged(d->currentLanguage);
}

void AltLangStrEdit::setValues(const MetaEngine::AltLangMap& values)
{
    d->values    = values;
    populateLangAltListEntries();

    d->valueEdit->blockSignals(true);

    QString text = d->values.value(d->currentLanguage);
    d->valueEdit->setPlainText(text);
    d->delValueButton->setEnabled(!text.isNull());
    d->translateButton->setEnabled(!text.isNull());

    d->valueEdit->blockSignals(false);
}

MetaEngine::AltLangMap& AltLangStrEdit::values() const
{
    return d->values;
}

void AltLangStrEdit::populateLangAltListEntries()
{
    d->languageCB->blockSignals(true);

    d->languageCB->clear();

    // In first we fill already assigned languages.

    QStringList list = d->values.keys();

    if (!list.isEmpty())
    {
        Q_FOREACH (const QString& item, list)
        {
            d->languageCB->addItem(item);
            d->languageCB->setItemIcon(d->languageCB->count() - 1,
                                       QIcon::fromTheme(QLatin1String("dialog-ok-apply")).pixmap(16, 16));
            d->languageCB->setItemData(d->languageCB->findText(item), languageNameRFC3066(item), Qt::ToolTipRole);
        }

        d->languageCB->insertSeparator(d->languageCB->count());
    }

    // ...and now, all the rest...

    SpellCheckContainer set = SpellCheckSettings::instance()->settings();
    QStringList lang        = set.alternativeLang;

    Q_FOREACH (const QString& lg, lang)
    {
        if (!list.contains(lg))
        {
            d->languageCB->addItem(lg);
            d->languageCB->setItemData(d->languageCB->findText(lg), languageNameRFC3066(lg), Qt::ToolTipRole);
        }
    }

    d->languageCB->setCurrentIndex(d->languageCB->findText(d->currentLanguage));

    d->languageCB->blockSignals(false);
}

void AltLangStrEdit::populateTranslationEntries()
{
    d->translateAction->m_list->clear();

    QStringList allRFC3066  = DOnlineTranslator::supportedRFC3066();
    SpellCheckContainer set = SpellCheckSettings::instance()->settings();

    Q_FOREACH (const QString& lg, set.translatorLang)
    {
        QListWidgetItem* const item = new QListWidgetItem(d->translateAction->m_list);
        item->setText(lg);
        item->setToolTip(i18n("Translate to %1", languageNameRFC3066(lg)));
        d->translateAction->m_list->addItem(item);
    }
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
    bool textChanged     = (editedText != previousText);

    if      (editedText.isEmpty() && textChanged)
    {
        slotDeleteValue();
    }
    else if (previousText.isNull() && textChanged)
    {
        addCurrent();
    }
    else if (textChanged)
    {
        // we cannot trust that the text actually changed
        // (there are bogus signals caused by spell checking, see bug #141663)
        // so we have to check before marking the metadata as modified.

        d->values.insert(d->currentLanguage, editedText);

        Q_EMIT signalModified(d->currentLanguage, editedText);
    }
}

void AltLangStrEdit::addCurrent()
{
    QString text = d->valueEdit->toPlainText();

    d->values.insert(d->currentLanguage, text);
    populateLangAltListEntries();
    d->delValueButton->setEnabled(true);
    d->translateButton->setEnabled(true);

    Q_EMIT signalValueAdded(d->currentLanguage, text);
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

DTextEdit* AltLangStrEdit::textEdit() const
{
    return d->valueEdit;
}

void AltLangStrEdit::slotOpenLocalizeSetup()
{
    SpellCheckSettings::instance()->openLocalizeSetup();
}

void AltLangStrEdit::slotTranslate(QListWidgetItem* item)
{
    d->translateButton->menu()->close();

    if (d->trengine->isRunning())
    {
        return;
    }

    setDisabled(true);

    if (item)
    {
        d->trCode       = item->text();
        QString srcCode = currentLanguageCode();
        DOnlineTranslator::Language trLang;
        DOnlineTranslator::Language srcLang;

        if (srcCode == QLatin1String("x-default"))
        {
            srcLang = DOnlineTranslator::Auto;
        }
        else
        {
            srcLang = DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(srcCode));
        }

        trLang       = DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(d->trCode));
        QString text = textEdit()->text();

        qCDebug(DIGIKAM_WIDGETS_LOG) << "Request to translate with Web-service:";
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Text to translate        :" << text;
        qCDebug(DIGIKAM_WIDGETS_LOG) << "To target language       :" << trLang;
        qCDebug(DIGIKAM_WIDGETS_LOG) << "With source language     :" << srcLang;

        d->trengine->translate(text,                                                            // String to translate
                               SpellCheckSettings::instance()->settings().translatorEngine,     // Web service
                               trLang,                                                          // Target language
                               srcLang,                                                         // Source langage
                               DOnlineTranslator::Auto);
    }
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
                                 i18n("Failed to translate string with %1 Web-service",
                                 DOnlineTranslator::engineName(SpellCheckSettings::instance()->settings().translatorEngine)),
                                 i18n("Error message: %1",
                                 d->trengine->errorString()));

    }
}

void AltLangStrEdit::slotLocalizeChanged()
{
    populateTranslationEntries();
    d->translateButton->setToolTip(i18nc("@info: language edit widget", "Select language to translate with %1",
                                   DOnlineTranslator::engineName(SpellCheckSettings::instance()->settings().translatorEngine)));
}

} // namespace Digikam

#include "altlangstredit.moc"
