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

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "donlinetranslator.h"
#include "spellchecksettings.h"

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
        m_list = new QListWidget(parent);
        QFontMetrics fontMt(m_list->font());
        QRect fontRect = fontMt.boundingRect(0, 0, m_list->width(), m_list->height(), 0, QLatin1String("mm-MM"));
        int width      =  m_list->contentsMargins().left() + m_list->contentsMargins().right();
        width         += fontRect.width() + m_list->verticalScrollBar()->height();
        m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_list->setResizeMode(QListView::Fixed);
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
 *
 */
const AltLangStrEdit::LanguageCodeMap AltLangStrEdit::s_rfc3066ForXMP =
{
    // The first one from the list is the Default Language code specified by XMP paper.

    { QLatin1String("x-default"), i18nc("@info: country code", "Default Language")                         },

    // the rest follow the RFC 3066.

    { QLatin1String("af-ZA"),     i18nc("@info: country code", "Afrikaans (South Africa)")                 },
    { QLatin1String("am-ET"),     i18nc("@info: country code", "Amharic (Ethiopia)")                       },
    { QLatin1String("ar-AE"),     i18nc("@info: country code", "Arabic (UAE)")                             },
    { QLatin1String("ar-BH"),     i18nc("@info: country code", "Arabic (Bahrain)")                         },
    { QLatin1String("ar-DZ"),     i18nc("@info: country code", "Arabic (Algeria)")                         },
    { QLatin1String("ar-EG"),     i18nc("@info: country code", "Arabic (Egypt)")                           },
    { QLatin1String("ar-IQ"),     i18nc("@info: country code", "Arabic (Iraq)")                            },
    { QLatin1String("ar-IL"),     i18nc("@info: country code", "Arabic (Israel)")                          },
    { QLatin1String("ar-JO"),     i18nc("@info: country code", "Arabic (Jordan)")                          },
    { QLatin1String("ar-KW"),     i18nc("@info: country code", "Arabic (Kuwait)")                          },
    { QLatin1String("ar-LB"),     i18nc("@info: country code", "Arabic (Lebanon)")                         },
    { QLatin1String("ar-LY"),     i18nc("@info: country code", "Arabic (Libya)")                           },
    { QLatin1String("ar-MA"),     i18nc("@info: country code", "Arabic (Morocco)")                         },
    { QLatin1String("ar-MR"),     i18nc("@info: country code", "Arabic (Mauritania)")                      },
    { QLatin1String("ar-PS"),     i18nc("@info: country code", "Arabic (Palestinian West Bank and Gaza)")  },
    { QLatin1String("ar-OM"),     i18nc("@info: country code", "Arabic (Oman)")                            },
    { QLatin1String("ar-QA"),     i18nc("@info: country code", "Arabic (Qatar)")                           },
    { QLatin1String("ar-SA"),     i18nc("@info: country code", "Arabic (Saudi Arabia)")                    },
    { QLatin1String("ar-SD"),     i18nc("@info: country code", "Arabic (Sudan)")                           },
    { QLatin1String("ar-SO"),     i18nc("@info: country code", "Arabic (Somalia)")                         },
    { QLatin1String("ar-SY"),     i18nc("@info: country code", "Arabic (Syria)")                           },
    { QLatin1String("ar-TD"),     i18nc("@info: country code", "Arabic (Chad)")                            },
    { QLatin1String("ar-TN"),     i18nc("@info: country code", "Arabic (Tunisia)")                         },
    { QLatin1String("ar-YE"),     i18nc("@info: country code", "Arabic (Yemen)")                           },
    { QLatin1String("as-IN"),     i18nc("@info: country code", "Assamese (India)")                         },
    { QLatin1String("ba-RU"),     i18nc("@info: country code", "Bashkir (Russia)")                         },
    { QLatin1String("bn-BD"),     i18nc("@info: country code", "Bengali (Bangladesh)")                     },
    { QLatin1String("bn-IN"),     i18nc("@info: country code", "Bengali (India)")                          },
    { QLatin1String("bn-SG"),     i18nc("@info: country code", "Bengali (Singapore)")                      },
    { QLatin1String("be-BY"),     i18nc("@info: country code", "Belarusian (Belarus)")                     },
    { QLatin1String("bg-BG"),     i18nc("@info: country code", "Bulgarian (Bulgaria)")                     },
    { QLatin1String("bn-IN"),     i18nc("@info: country code", "Bengali (India)")                          },
    { QLatin1String("bo-BT"),     i18nc("@info: country code", "Tibetan (Bhutan)")                         },
    { QLatin1String("bo-CN"),     i18nc("@info: country code", "Tibetan (PRC)")                            },
    { QLatin1String("br-FR"),     i18nc("@info: country code", "Breton (France)")                          },
    { QLatin1String("ca-AD"),     i18nc("@info: country code", "Catalan (Andorra)")                        },
    { QLatin1String("ca-ES"),     i18nc("@info: country code", "Catalan (Spain)")                          },
    { QLatin1String("ca-FR"),     i18nc("@info: country code", "Catalan (France)")                         },
    { QLatin1String("ch-GU"),     i18nc("@info: country code", "Chamorro (Guam)")                          },
    { QLatin1String("ch-MP"),     i18nc("@info: country code", "Chamorro (Northern Mariana Islands)")      },
    { QLatin1String("co-FR"),     i18nc("@info: country code", "Corsican (France)")                        },
    { QLatin1String("cs-CZ"),     i18nc("@info: country code", "Czech (Czechia)")                          },
    { QLatin1String("cy-GB"),     i18nc("@info: country code", "Welsh (United Kingdom)")                   },
    { QLatin1String("da-DK"),     i18nc("@info: country code", "Danish (Denmark)")                         },
    { QLatin1String("da-GL"),     i18nc("@info: country code", "Danish (Greenland)")                       },
    { QLatin1String("de-AT"),     i18nc("@info: country code", "German (Austria)")                         },
    { QLatin1String("de-CH"),     i18nc("@info: country code", "German (Switzerland)")                     },
    { QLatin1String("de-DE"),     i18nc("@info: country code", "German (Germany)")                         },
    { QLatin1String("de-DK"),     i18nc("@info: country code", "German (Denmark)")                         },
    { QLatin1String("de-LI"),     i18nc("@info: country code", "German (Liechtenstein)")                   },
    { QLatin1String("de-LU"),     i18nc("@info: country code", "German (Luxembourg)")                      },
    { QLatin1String("el-CY"),     i18nc("@info: country code", "Greek (Cyprus)")                           },
    { QLatin1String("el-GR"),     i18nc("@info: country code", "Greek (Greece)")                           },
    { QLatin1String("en-AG"),     i18nc("@info: country code", "English (Antigua and Barbuda)")            },
    { QLatin1String("en-AI"),     i18nc("@info: country code", "English (Anguilla)")                       },
    { QLatin1String("en-AS"),     i18nc("@info: country code", "English (American Samoa)")                 },
    { QLatin1String("en-AU"),     i18nc("@info: country code", "English (Australia)")                      },
    { QLatin1String("en-BB"),     i18nc("@info: country code", "English (Barbados)")                       },
    { QLatin1String("en-BM"),     i18nc("@info: country code", "English (Bermuda)")                        },
    { QLatin1String("en-BN"),     i18nc("@info: country code", "English (Brunei)")                         },
    { QLatin1String("en-BS"),     i18nc("@info: country code", "English (Bahamas)")                        },
    { QLatin1String("en-BW"),     i18nc("@info: country code", "English (Botswana)")                       },
    { QLatin1String("en-BZ"),     i18nc("@info: country code", "English (Belize)")                         },
    { QLatin1String("en-CA"),     i18nc("@info: country code", "English (Canada)")                         },
    { QLatin1String("en-CB"),     i18nc("@info: country code", "English (Caribbean)")                      },
    { QLatin1String("en-CK"),     i18nc("@info: country code", "English (Cook Islands)")                   },
    { QLatin1String("en-CM"),     i18nc("@info: country code", "English (Cameroon)")                       },
    { QLatin1String("en-DM"),     i18nc("@info: country code", "English (Dominica)")                       },
    { QLatin1String("en-ER"),     i18nc("@info: country code", "English (Eritrea)")                        },
    { QLatin1String("en-ET"),     i18nc("@info: country code", "English (Ethiopia)")                       },
    { QLatin1String("en-FJ"),     i18nc("@info: country code", "English (Fiji)")                           },
    { QLatin1String("en-FK"),     i18nc("@info: country code", "English (Falkland Islands)")               },
    { QLatin1String("en-FM"),     i18nc("@info: country code", "English (Micronesia)")                     },
    { QLatin1String("en-GB"),     i18nc("@info: country code", "English (United Kingdom)")                 },
    { QLatin1String("en-GD"),     i18nc("@info: country code", "English (Grenada)")                        },
    { QLatin1String("en-GH"),     i18nc("@info: country code", "English (Ghana)")                          },
    { QLatin1String("en-GI"),     i18nc("@info: country code", "English (Gibraltar)")                      },
    { QLatin1String("en-GM"),     i18nc("@info: country code", "English (Gambia)")                         },
    { QLatin1String("en-GU"),     i18nc("@info: country code", "English (Guam)")                           },
    { QLatin1String("en-GY"),     i18nc("@info: country code", "English (Guyana)")                         },
    { QLatin1String("en-IE"),     i18nc("@info: country code", "English (Ireland)")                        },
    { QLatin1String("en-IL"),     i18nc("@info: country code", "English (Israel)")                         },
    { QLatin1String("en-IN"),     i18nc("@info: country code", "English (India)")                          },
    { QLatin1String("en-IO"),     i18nc("@info: country code", "English (British Indian Ocean Territ)")    },
    { QLatin1String("en-JM"),     i18nc("@info: country code", "English (Jamaica)")                        },
    { QLatin1String("en-KR"),     i18nc("@info: country code", "English (Kenya)")                          },
    { QLatin1String("en-KI"),     i18nc("@info: country code", "English (Kiribati)")                       },
    { QLatin1String("en-KN"),     i18nc("@info: country code", "English (St. Kitts-Nevis)")                },
    { QLatin1String("en-KY"),     i18nc("@info: country code", "English (Cayman Islands)")                 },
    { QLatin1String("en-LC"),     i18nc("@info: country code", "English (St. Lucia)")                      },
    { QLatin1String("en-LR"),     i18nc("@info: country code", "English (Liberia)")                        },
    { QLatin1String("en-LS"),     i18nc("@info: country code", "English (Lesotho)")                        },
    { QLatin1String("en-MP"),     i18nc("@info: country code", "English (Northern Mariana Islands)")       },
    { QLatin1String("en-MS"),     i18nc("@info: country code", "English (Montserrat)")                     },
    { QLatin1String("en-MT"),     i18nc("@info: country code", "English (Malta)")                          },
    { QLatin1String("en-MU"),     i18nc("@info: country code", "English (Mauritius)")                      },
    { QLatin1String("en-MW"),     i18nc("@info: country code", "English (Malawi)")                         },
    { QLatin1String("en-MY"),     i18nc("@info: country code", "English (Malaysia)")                       },
    { QLatin1String("en-NA"),     i18nc("@info: country code", "English (Namibia)")                        },
    { QLatin1String("en-NF"),     i18nc("@info: country code", "English (Norfolk Island)")                 },
    { QLatin1String("en-NG"),     i18nc("@info: country code", "English (Nigeria)")                        },
    { QLatin1String("en-NR"),     i18nc("@info: country code", "English (Nauru)")                          },
    { QLatin1String("en-NU"),     i18nc("@info: country code", "English (Niue)")                           },
    { QLatin1String("en-NZ"),     i18nc("@info: country code", "English (New Zealand)")                    },
    { QLatin1String("en-PG"),     i18nc("@info: country code", "English (Papua New Guinea)")               },
    { QLatin1String("en-PH"),     i18nc("@info: country code", "English (Philippines)")                    },
    { QLatin1String("en-PK"),     i18nc("@info: country code", "English (Pakistan)")                       },
    { QLatin1String("en-PN"),     i18nc("@info: country code", "English (Pitcairn)")                       },
    { QLatin1String("en-PR"),     i18nc("@info: country code", "English (Puerto Rico)")                    },
    { QLatin1String("en-PW"),     i18nc("@info: country code", "English (Palau)")                          },
    { QLatin1String("en-RW"),     i18nc("@info: country code", "English (Rwanda)")                         },
    { QLatin1String("en-SB"),     i18nc("@info: country code", "English (Solomon Islands)")                },
    { QLatin1String("en-SC"),     i18nc("@info: country code", "English (Seychelles)")                     },
    { QLatin1String("en-SG"),     i18nc("@info: country code", "English (Singapore)")                      },
    { QLatin1String("en-SH"),     i18nc("@info: country code", "English (St. Helena)")                     },
    { QLatin1String("en-SL"),     i18nc("@info: country code", "English (Sierra Leone)")                   },
    { QLatin1String("en-SO"),     i18nc("@info: country code", "English (Somalia)")                        },
    { QLatin1String("en-SZ"),     i18nc("@info: country code", "English (Swaziland)")                      },
    { QLatin1String("en-TC"),     i18nc("@info: country code", "English (Turks and Caicos Islands)")       },
    { QLatin1String("en-TK"),     i18nc("@info: country code", "English (Tokelau)")                        },
    { QLatin1String("en-TO"),     i18nc("@info: country code", "English (Tonga)")                          },
    { QLatin1String("en-TT"),     i18nc("@info: country code", "English (Trinidad)")                       },
    { QLatin1String("en-UG"),     i18nc("@info: country code", "English (Uganda)")                         },
    { QLatin1String("en-US"),     i18nc("@info: country code", "English (United States)")                  },
    { QLatin1String("en-VC"),     i18nc("@info: country code", "English (St. Vincent and the Grenadi)")    },
    { QLatin1String("en-VG"),     i18nc("@info: country code", "English (British Virgin Islands)")         },
    { QLatin1String("en-VI"),     i18nc("@info: country code", "English (U.S. Virgin Islands)")            },
    { QLatin1String("en-VU"),     i18nc("@info: country code", "English (Vanuatu)")                        },
    { QLatin1String("en-WS"),     i18nc("@info: country code", "English (Western Samoa)")                  },
    { QLatin1String("en-ZA"),     i18nc("@info: country code", "English (South Africa)")                   },
    { QLatin1String("en-ZM"),     i18nc("@info: country code", "English (Zambia)")                         },
    { QLatin1String("en-ZW"),     i18nc("@info: country code", "English (Zimbabwe)")                       },
    { QLatin1String("es-AR"),     i18nc("@info: country code", "Spanish (Argentina)")                      },
    { QLatin1String("es-BO"),     i18nc("@info: country code", "Spanish (Bolivia)")                        },
    { QLatin1String("es-CL"),     i18nc("@info: country code", "Spanish (Chile)")                          },
    { QLatin1String("es-CO"),     i18nc("@info: country code", "Spanish (Colombia)")                       },
    { QLatin1String("es-CR"),     i18nc("@info: country code", "Spanish (Costa Rica)")                     },
    { QLatin1String("es-CU"),     i18nc("@info: country code", "Spanish (Cuba)")                           },
    { QLatin1String("es-DO"),     i18nc("@info: country code", "Spanish (Dominican Republic)")             },
    { QLatin1String("es-EC"),     i18nc("@info: country code", "Spanish (Ecuador)")                        },
    { QLatin1String("es-ES"),     i18nc("@info: country code", "Spanish (Spain)")                          },
    { QLatin1String("es-GQ"),     i18nc("@info: country code", "Spanish (Equatorial Guinea)")              },
    { QLatin1String("es-GT"),     i18nc("@info: country code", "Spanish (Guatemala)")                      },
    { QLatin1String("es-HN"),     i18nc("@info: country code", "Spanish (Honduras)")                       },
    { QLatin1String("es-MX"),     i18nc("@info: country code", "Spanish (Mexico)")                         },
    { QLatin1String("es-NI"),     i18nc("@info: country code", "Spanish (Nicaragua)")                      },
    { QLatin1String("es-PA"),     i18nc("@info: country code", "Spanish (Panama)")                         },
    { QLatin1String("es-PE"),     i18nc("@info: country code", "Spanish (Peru)")                           },
    { QLatin1String("es-PR"),     i18nc("@info: country code", "Spanish (Puerto Rico)")                    },
    { QLatin1String("es-PY"),     i18nc("@info: country code", "Spanish (Paraguay)")                       },
    { QLatin1String("es-SV"),     i18nc("@info: country code", "Spanish (El Salvador)")                    },
    { QLatin1String("es-US"),     i18nc("@info: country code", "Spanish (United States)")                  },
    { QLatin1String("es-UY"),     i18nc("@info: country code", "Spanish (Uruguay)")                        },
    { QLatin1String("es-VE"),     i18nc("@info: country code", "Spanish (Venezuela)")                      },
    { QLatin1String("et-EE"),     i18nc("@info: country code", "Estonian (Estonia)")                       },
    { QLatin1String("eu-ES"),     i18nc("@info: country code", "Basque (Basque Country)")                  },
    { QLatin1String("fa-IR"),     i18nc("@info: country code", "Persian (Iran)")                           },
    { QLatin1String("fi-FI"),     i18nc("@info: country code", "Finnish (Finland)")                        },
    { QLatin1String("fo-FO"),     i18nc("@info: country code", "Faeroese (Faero Islands)")                 },
    { QLatin1String("fr-AD"),     i18nc("@info: country code", "French (Andorra)")                         },
    { QLatin1String("fr-BE"),     i18nc("@info: country code", "French (Belgium)")                         },
    { QLatin1String("fr-BF"),     i18nc("@info: country code", "French (Burkina Faso)")                    },
    { QLatin1String("fr-BI"),     i18nc("@info: country code", "French (Burundi)")                         },
    { QLatin1String("fr-BJ"),     i18nc("@info: country code", "French (Benin)")                           },
    { QLatin1String("fr-CA"),     i18nc("@info: country code", "French (Canada)")                          },
    { QLatin1String("fr-CD"),     i18nc("@info: country code", "French (Democratic Republic of Congo)")    },
    { QLatin1String("fr-CF"),     i18nc("@info: country code", "French (Central African Republic)")        },
    { QLatin1String("fr-CG"),     i18nc("@info: country code", "French (Congo)")                           },
    { QLatin1String("fr-CH"),     i18nc("@info: country code", "French (Switzerland)")                     },
    { QLatin1String("fr-CI"),     i18nc("@info: country code", "French (Cote d'Ivoire)")                   },
    { QLatin1String("fr-CM"),     i18nc("@info: country code", "French (Cameroon)")                        },
    { QLatin1String("fr-DJ"),     i18nc("@info: country code", "French (Djibouti)")                        },
    { QLatin1String("fr-FR"),     i18nc("@info: country code", "French (France)")                          },
    { QLatin1String("fr-GA"),     i18nc("@info: country code", "French (Gabon)")                           },
    { QLatin1String("fr-GB"),     i18nc("@info: country code", "French (United Kingdom)")                  },
    { QLatin1String("fr-GF"),     i18nc("@info: country code", "French (French Guiana)")                   },
    { QLatin1String("fr-GN"),     i18nc("@info: country code", "French (Guinea)")                          },
    { QLatin1String("fr-GP"),     i18nc("@info: country code", "French (Guadeloupe)")                      },
    { QLatin1String("fr-HT"),     i18nc("@info: country code", "French (Haiti)")                           },
    { QLatin1String("fr-IT"),     i18nc("@info: country code", "French (Italy)")                           },
    { QLatin1String("fr-KM"),     i18nc("@info: country code", "French (Comoros Islands)")                 },
    { QLatin1String("fr-LB"),     i18nc("@info: country code", "French (Lebanon)")                         },
    { QLatin1String("fr-LU"),     i18nc("@info: country code", "French (Luxembourg)")                      },
    { QLatin1String("fr-MC"),     i18nc("@info: country code", "French (Monaco)")                          },
    { QLatin1String("fr-MG"),     i18nc("@info: country code", "French (Madagascar)")                      },
    { QLatin1String("fr-ML"),     i18nc("@info: country code", "French (Mali)")                            },
    { QLatin1String("fr-MQ"),     i18nc("@info: country code", "French (Martinique)")                      },
    { QLatin1String("fr-NC"),     i18nc("@info: country code", "Frisian (New Caledonia)")                  },
    { QLatin1String("fr-PF"),     i18nc("@info: country code", "French (French Polynesia)")                },
    { QLatin1String("fr-PM"),     i18nc("@info: country code", "French (St. Pierre and Miquelon)")         },
    { QLatin1String("fr-RE"),     i18nc("@info: country code", "French (Reunion)")                         },
    { QLatin1String("fr-RW"),     i18nc("@info: country code", "French (Rwanda)")                          },
    { QLatin1String("fr-SC"),     i18nc("@info: country code", "French (Seychelles)")                      },
    { QLatin1String("fr-TD"),     i18nc("@info: country code", "French (Chad)")                            },
    { QLatin1String("fr-TG"),     i18nc("@info: country code", "French (Togo)")                            },
    { QLatin1String("fr-VU"),     i18nc("@info: country code", "French (Vanuatu)")                         },
    { QLatin1String("fr-WF"),     i18nc("@info: country code", "French (Wallis and Futuna)")               },
    { QLatin1String("fr-YT"),     i18nc("@info: country code", "French (Mayotte)")                         },
    { QLatin1String("fy-NL"),     i18nc("@info: country code", "Frisian (Netherlands)")                    },
    { QLatin1String("ga-GB"),     i18nc("@info: country code", "Irish (United Kingdom)")                   },
    { QLatin1String("ga-IE"),     i18nc("@info: country code", "Irish (Ireland)")                          },
    { QLatin1String("gd-GB"),     i18nc("@info: country code", "Gaelic (United Kingdom)")                  },
    { QLatin1String("gl-ES"),     i18nc("@info: country code", "Galician (Galicia)")                       },
    { QLatin1String("gu-IN"),     i18nc("@info: country code", "Gujarati (India)")                         },
    { QLatin1String("gv-GB"),     i18nc("@info: country code", "Manx (United Kingdom)")                    },
    { QLatin1String("he-IL"),     i18nc("@info: country code", "Hebrew (Israel)")                          },
    { QLatin1String("hi-IN"),     i18nc("@info: country code", "Hindi (India)")                            },
    { QLatin1String("hr-BA"),     i18nc("@info: country code", "Croatian (Bosnia and Herzegovina, Latin)") },
    { QLatin1String("hr-HR"),     i18nc("@info: country code", "Croatian (Croatia)")                       },
    { QLatin1String("hu-HU"),     i18nc("@info: country code", "Hungarian (Hungary)")                      },
    { QLatin1String("hu-SI"),     i18nc("@info: country code", "Hungarian (Slovenia)")                     },
    { QLatin1String("hy-AM"),     i18nc("@info: country code", "Armenian (Armenia)")                       },
    { QLatin1String("id-ID"),     i18nc("@info: country code", "(Indonesian)")                             },
    { QLatin1String("ii-CN"),     i18nc("@info: country code", "Yi (PRC)")                                 },
    { QLatin1String("is-IS"),     i18nc("@info: country code", "Icelandic (Iceland)")                      },
    { QLatin1String("it-CH"),     i18nc("@info: country code", "Italian (Switzerland)")                    },
    { QLatin1String("it-HR"),     i18nc("@info: country code", "Italian (Croatia)")                        },
    { QLatin1String("it-IT"),     i18nc("@info: country code", "Italian (Italy)")                          },
    { QLatin1String("it-SI"),     i18nc("@info: country code", "Italian (Slovenia)")                       },
    { QLatin1String("it-SM"),     i18nc("@info: country code", "Italian (San Marino)")                     },
    { QLatin1String("ja-JP"),     i18nc("@info: country code", "Japanese (Japan)")                         },
    { QLatin1String("ka-GE"),     i18nc("@info: country code", "Georgian (Georgia)")                       },
    { QLatin1String("kk-KZ"),     i18nc("@info: country code", "Kazakh (Kazakhstan)")                      },
    { QLatin1String("kl-GL"),     i18nc("@info: country code", "Greenlandic (Greenland)")                  },
    { QLatin1String("km-KH"),     i18nc("@info: country code", "Khmer (Cambodia)")                         },
    { QLatin1String("kn-IN"),     i18nc("@info: country code", "Kannada (India)")                          },
    { QLatin1String("ko-KR"),     i18nc("@info: country code", "Korean (South Korea)")                     },
    { QLatin1String("ko-KP"),     i18nc("@info: country code", "Korean (North Korea)")                     },
    { QLatin1String("ky-KG"),     i18nc("@info: country code", "Kyrgyz (Kyrgyzstan)")                      },
    { QLatin1String("kw-GB"),     i18nc("@info: country code", "Cornish (United Kingdom)")                 },
    { QLatin1String("lb-LU"),     i18nc("@info: country code", "Luxembourgish (Luxembourg)")               },
    { QLatin1String("ln-CD"),     i18nc("@info: country code", "Lingala (Democratic Republic of Congo)")   },
    { QLatin1String("ln-CG"),     i18nc("@info: country code", "Lingala (Congo)")                          },
    { QLatin1String("lo-LA"),     i18nc("@info: country code", "Lao (Lao PDR)")                            },
    { QLatin1String("lt-LT"),     i18nc("@info: country code", "Lithuanian (Lithuania)")                   },
    { QLatin1String("lv-LV"),     i18nc("@info: country code", "Latvian (Latvia)")                         },
    { QLatin1String("mi-NZ"),     i18nc("@info: country code", "Maori (New Zealand)")                      },
    { QLatin1String("mk-MK"),     i18nc("@info: country code", "Macedonian (Macedonia)")                   },
    { QLatin1String("ml-IN"),     i18nc("@info: country code", "Malayalam (India)")                        },
    { QLatin1String("mn-CN"),     i18nc("@info: country code", "Mongolian (PRC)")                          },
    { QLatin1String("mn-MN"),     i18nc("@info: country code", "Mongolian (Mongolia)")                     },
    { QLatin1String("mr-IN"),     i18nc("@info: country code", "Marathi (India)")                          },
    { QLatin1String("ms-BN"),     i18nc("@info: country code", "Malay (Brunei Darussalam)")                },
    { QLatin1String("ms-MY"),     i18nc("@info: country code", "Malay (Malaysia)")                         },
    { QLatin1String("ms-SG"),     i18nc("@info: country code", "Malay (Singapore)")                        },
    { QLatin1String("mt-MT"),     i18nc("@info: country code", "Maltese (Malta)")                          },
    { QLatin1String("nb-NO"),     i18nc("@info: country code", "Norwegian Bokmål (Norway)")                },
    { QLatin1String("ne-NP"),     i18nc("@info: country code", "Nepali (Nepal)")                           },
    { QLatin1String("nl-AN"),     i18nc("@info: country code", "Dutch (Netherlands Antilles)")             },
    { QLatin1String("nl-AW"),     i18nc("@info: country code", "Dutch (Aruba)")                            },
    { QLatin1String("nl-BE"),     i18nc("@info: country code", "Dutch (Belgium)")                          },
    { QLatin1String("nl-NL"),     i18nc("@info: country code", "Dutch (Netherlands)")                      },
    { QLatin1String("nl-SR"),     i18nc("@info: country code", "Dutch (Suriname)")                         },
    { QLatin1String("nn-NO"),     i18nc("@info: country code", "Norwegian Nynorsk (Norway)")               },
    { QLatin1String("ns-ZA"),     i18nc("@info: country code", "Sesotho sa Leboa (South Africa)")          },
    { QLatin1String("oc-FR"),     i18nc("@info: country code", "Occitan (France)")                         },
    { QLatin1String("or-IN"),     i18nc("@info: country code", "Oriya (India)")                            },
    { QLatin1String("pa-IN"),     i18nc("@info: country code", "Punjabi (India)")                          },
    { QLatin1String("pl-PL"),     i18nc("@info: country code", "Polish (Poland)")                          },
    { QLatin1String("ps-AF"),     i18nc("@info: country code", "Pashto (Afghanistan)")                     },
    { QLatin1String("pt-AO"),     i18nc("@info: country code", "Portuguese (Angola)")                      },
    { QLatin1String("pt-BR"),     i18nc("@info: country code", "Portuguese (Brazil)")                      },
    { QLatin1String("pt-CV"),     i18nc("@info: country code", "Portuguese (Cape Verde Islands)")          },
    { QLatin1String("pt-GW"),     i18nc("@info: country code", "Portuguese (Guinea-Bissau)")               },
    { QLatin1String("pt-MZ"),     i18nc("@info: country code", "Portuguese (Mozambique)")                  },
    { QLatin1String("pt-PT"),     i18nc("@info: country code", "Portuguese (Portugal)")                    },
    { QLatin1String("pt-ST"),     i18nc("@info: country code", "Portuguese (São Tomé e Principe)")         },
    { QLatin1String("rm-CH"),     i18nc("@info: country code", "Romansh (Switzerland)")                    },
    { QLatin1String("ro-RO"),     i18nc("@info: country code", "Romanian (Romania)")                       },
    { QLatin1String("ru-RU"),     i18nc("@info: country code", "Russian (Russia)")                         },
    { QLatin1String("rw-RW"),     i18nc("@info: country code", "Kinyarwanda (Rwanda)")                     },
    { QLatin1String("sa-IN"),     i18nc("@info: country code", "Sanskrit (India)")                         },
    { QLatin1String("sd-IN"),     i18nc("@info: country code", "Sindhi (India)")                           },
    { QLatin1String("sd-PK"),     i18nc("@info: country code", "Sindhi (Pakistan)")                        },
    { QLatin1String("se-FI"),     i18nc("@info: country code", "Sami (Northern, Finland)")                 },
    { QLatin1String("se-NO"),     i18nc("@info: country code", "Sami (Northern, Norway)")                  },
    { QLatin1String("se-SE"),     i18nc("@info: country code", "Sami (Northern, Sweden)")                  },
    { QLatin1String("si-LK"),     i18nc("@info: country code", "Sinhala (Sri Lanka)")                      },
    { QLatin1String("sk-SK"),     i18nc("@info: country code", "Slovak (Slovakia)")                        },
    { QLatin1String("sl-SI"),     i18nc("@info: country code", "Slovenian (Slovenia)")                     },
    { QLatin1String("sq-AL"),     i18nc("@info: country code", "Albanian (Albania)")                       },
    { QLatin1String("sr-BA"),     i18nc("@info: country code", "Serbian (Bosnia-Herzegovina)")             },
    { QLatin1String("sr-YU"),     i18nc("@info: country code", "Serbian (Yugoslavia)")                     },
    { QLatin1String("ss-SZ"),     i18nc("@info: country code", "Swati (Swaziland)")                        },
    { QLatin1String("ss-ZA"),     i18nc("@info: country code", "Swati (South Africa)")                     },
    { QLatin1String("sv-FI"),     i18nc("@info: country code", "Swedish (Finland)")                        },
    { QLatin1String("sv-SE"),     i18nc("@info: country code", "Swedish (Sweden)")                         },
    { QLatin1String("sw-KE"),     i18nc("@info: country code", "Swahili (Kenya)")                          },
    { QLatin1String("sw-TZ"),     i18nc("@info: country code", "Swahili (Tanzania)")                       },
    { QLatin1String("ta-IN"),     i18nc("@info: country code", "Tamil (India)")                            },
    { QLatin1String("ta-SG"),     i18nc("@info: country code", "Tamil (Singapore)")                        },
    { QLatin1String("te-IN"),     i18nc("@info: country code", "Telugu (India)")                           },
    { QLatin1String("th-TH"),     i18nc("@info: country code", "Thai (Thailand)")                          },
    { QLatin1String("tk-TM"),     i18nc("@info: country code", "Turkmen (Turkmenistan)")                   },
    { QLatin1String("tn-BW"),     i18nc("@info: country code", "Setswana Tswana (Botswana)")               },
    { QLatin1String("tn-ZA"),     i18nc("@info: country code", "Setswana Tswana (South Africa)")           },
    { QLatin1String("tr-BG"),     i18nc("@info: country code", "Turkish (Bulgaria)")                       },
    { QLatin1String("tr-CY"),     i18nc("@info: country code", "Turkish (Cyprus)")                         },
    { QLatin1String("tr-TR"),     i18nc("@info: country code", "Turkish (Turkey)")                         },
    { QLatin1String("tt-RU"),     i18nc("@info: country code", "Tatar (Russia)")                           },
    { QLatin1String("ug-CN"),     i18nc("@info: country code", "Uighur (PRC)")                             },
    { QLatin1String("uk-UA"),     i18nc("@info: country code", "Ukrainian (Ukraine)")                      },
    { QLatin1String("ur-IN"),     i18nc("@info: country code", "Urdu (India)")                             },
    { QLatin1String("ur-PK"),     i18nc("@info: country code", "Urdu (Pakistan)")                          },
    { QLatin1String("vi-VN"),     i18nc("@info: country code", "Vietnamese (Vietnam)")                     },
    { QLatin1String("wo-SN"),     i18nc("@info: country code", "Wolof (Senegal)")                          },
    { QLatin1String("xh-ZA"),     i18nc("@info: country code", "isiXhosa Xhosa (South Africa)")            },
    { QLatin1String("yo-NG"),     i18nc("@info: country code", "Yoruba (Nigeria)")                         },
    { QLatin1String("zh-CN"),     i18nc("@info: country code", "Chinese (PRC)")                            },
    { QLatin1String("zh-HK"),     i18nc("@info: country code", "Chinese (Hong Kong SAR, PRC)")             },
    { QLatin1String("zh-MO"),     i18nc("@info: country code", "Chinese (Macao SAR)")                      },
    { QLatin1String("zh-SG"),     i18nc("@info: country code", "Chinese (Singapore)")                      },
    { QLatin1String("zh-TW"),     i18nc("@info: country code", "Chinese (Taiwan)")                         },
    { QLatin1String("zu-ZA"),     i18nc("@info: country code", "isiZulu Zulu (South Africa)")              }
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
    d->grid->addWidget(d->valueEdit,       1, 0, 1, -1);
    d->grid->setColumnStretch(1, 10);
    d->grid->setContentsMargins(QMargins());
    d->grid->setSpacing(qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing)));

    loadLangAltListEntries();
    slotLocalizeChanged();

    // --------------------------------------------------------

    connect(d->languageCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &AltLangStrEdit::slotSelectionChanged);

    connect(d->delValueButton, &QToolButton::clicked,
            this, &AltLangStrEdit::slotDeleteValue);

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
    loadLangAltListEntries();

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

void AltLangStrEdit::loadLangAltListEntries()
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
        }

        d->languageCB->insertSeparator(d->languageCB->count());
    }

    // ...and now, all the rest...

    for (LanguageCodeMap::const_iterator it = s_rfc3066ForXMP.constBegin() ;
         it != s_rfc3066ForXMP.constEnd() ; ++it)
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

        Q_EMIT signalModified(d->currentLanguage, editedText);
    }
}

void AltLangStrEdit::addCurrent()
{
    QString text = d->valueEdit->toPlainText();

    d->values.insert(d->currentLanguage, text);
    loadLangAltListEntries();
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

void AltLangStrEdit::loadTranslationTargets()
{
    d->translateAction->m_list->clear();

    QStringList allRFC3066 = DOnlineTranslator::supportedRFC3066();
    QStringList engineRFC3066;

    Q_FOREACH (const QString& rfc, allRFC3066)
    {
        if (
            DOnlineTranslator::isSupportTranslation(SpellCheckSettings::instance()->settings().translatorEngine,
                                                    DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(rfc)))
           )
        {
            engineRFC3066 << rfc;
        }
    }

    for (LanguageCodeMap::const_iterator it = s_rfc3066ForXMP.constBegin() ;
         it != s_rfc3066ForXMP.constEnd() ; ++it)
    {
        if (!it.key().isEmpty() && engineRFC3066.contains(it.key()))
        {
            QListWidgetItem* const item = new QListWidgetItem(d->translateAction->m_list);
            item->setText(it.key());
            item->setToolTip(i18n("Translate to %1", it.value()));
            d->translateAction->m_list->addItem(item);
        }
    }
}

void AltLangStrEdit::slotTranslate(QListWidgetItem* item)
{
    d->translateButton->menu()->close();

    if (d->trengine->isRunning())
    {
        return;
    }

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
        setCurrentLanguageCode(d->trCode);

        Q_EMIT signalValueAdded(d->trCode, translation);

        d->trCode.clear();
    }
    else
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Translation Error       :" << d->trengine->error();
    }
}

void AltLangStrEdit::slotLocalizeChanged()
{
    loadTranslationTargets();
    d->translateButton->setToolTip(i18nc("@info: language edit widget", "Select language to translate with %1",
                                   DOnlineTranslator::engineName(SpellCheckSettings::instance()->settings().translatorEngine)));
}

} // namespace Digikam

#include "altlangstredit.moc"
