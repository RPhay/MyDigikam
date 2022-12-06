/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-23
 * Description : item metadata interface - Iptc helpers.
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2011      by Leif Huhn <leif at dkstat dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "dmetadata.h"

// Qt includes

#include <QLocale>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "metaenginesettings.h"
#include "digikam_version.h"
#include "digikam_globals.h"
#include "digikam_debug.h"

namespace Digikam
{

IptcCoreContactInfo DMetadata::getCreatorContactInfo() const
{
    MetadataFields fields;
    fields << MetadataInfo::IptcCoreContactInfoCity
           << MetadataInfo::IptcCoreContactInfoCountry
           << MetadataInfo::IptcCoreContactInfoAddress
           << MetadataInfo::IptcCoreContactInfoPostalCode
           << MetadataInfo::IptcCoreContactInfoProvinceState
           << MetadataInfo::IptcCoreContactInfoEmail
           << MetadataInfo::IptcCoreContactInfoPhone
           << MetadataInfo::IptcCoreContactInfoWebUrl;

    QVariantList metadataInfos = getMetadataFields(fields);

    IptcCoreContactInfo info;

    if (metadataInfos.size() == 8)
    {
        info.city          = metadataInfos.at(0).toString();
        info.country       = metadataInfos.at(1).toString();
        info.address       = metadataInfos.at(2).toString();
        info.postalCode    = metadataInfos.at(3).toString();
        info.provinceState = metadataInfos.at(4).toString();
        info.email         = metadataInfos.at(5).toString();
        info.phone         = metadataInfos.at(6).toString();
        info.webUrl        = metadataInfos.at(7).toString();
    }

    return info;
}

bool DMetadata::setCreatorContactInfo(const IptcCoreContactInfo& info) const
{
    if (!supportXmp())
    {
        return false;
    }

    if (!info.city.isEmpty())
    {
        if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrCity", info.city))
        {
            return false;
        }
    }

    if (!info.country.isEmpty())
    {
        if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrCtry", info.country))
        {
            return false;
        }
    }

    if (!info.address.isEmpty())
    {
        if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrExtadr", info.address))
        {
            return false;
        }
    }

    if (!info.postalCode.isEmpty())
    {
        if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrPcode", info.postalCode))
        {
            return false;
        }
    }

    if (!info.provinceState.isEmpty())
    {
        if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrRegion", info.provinceState))
        {
            return false;
        }
    }

    if (!info.email.isEmpty())
    {
        if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiEmailWork", info.email))
        {
            return false;
        }
    }

    if (!info.phone.isEmpty())
    {
        if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiTelWork", info.phone))
        {
            return false;
        }
    }

    if (!info.webUrl.isEmpty())
    {
        if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiUrlWork", info.webUrl))
        {
            return false;
        }
    }

    return true;
}

IptcCoreLocationInfo DMetadata::getIptcCoreLocation() const
{
    MetadataFields fields;
    fields << MetadataInfo::IptcCoreCountry
           << MetadataInfo::IptcCoreCountryCode
           << MetadataInfo::IptcCoreCity
           << MetadataInfo::IptcCoreLocation
           << MetadataInfo::IptcCoreProvinceState;

    QVariantList metadataInfos = getMetadataFields(fields);

    IptcCoreLocationInfo location;

    if (fields.size() == 5)
    {
        location.country       = metadataInfos.at(0).toString();
        location.countryCode   = metadataInfos.at(1).toString();
        location.city          = metadataInfos.at(2).toString();
        location.location      = metadataInfos.at(3).toString();
        location.provinceState = metadataInfos.at(4).toString();
    }

    return location;
}

bool DMetadata::setIptcCoreLocation(const IptcCoreLocationInfo& location) const
{
    if (supportXmp())
    {
        if (!location.country.isEmpty())
        {
            if (!setXmpTagString("Xmp.photoshop.Country", location.country))
            {
                return false;
            }
        }

        if (!location.countryCode.isEmpty())
        {
            if (!setXmpTagString("Xmp.iptc.CountryCode", location.countryCode))
            {
                return false;
            }
        }

        if (!location.city.isEmpty())
        {
            if (!setXmpTagString("Xmp.photoshop.City", location.city))
            {
                return false;
            }
        }

        if (!location.location.isEmpty())
        {
            if (!setXmpTagString("Xmp.iptc.Location", location.location))
            {
                return false;
            }
        }

        if (!location.provinceState.isEmpty())
        {
            if (!setXmpTagString("Xmp.photoshop.State", location.provinceState))
            {
                return false;
            }
        }
    }

    if (!location.country.isEmpty())
    {
        if (!setIptcTag(location.country,       64,  "Country",        "Iptc.Application2.CountryName"))
        {
            return false;
        }
    }

    if (!location.countryCode.isEmpty())
    {
        if (!setIptcTag(location.countryCode,    3,  "Country Code",   "Iptc.Application2.CountryCode"))
        {
            return false;
        }
    }

    if (!location.city.isEmpty())
    {
        if (!setIptcTag(location.city,          32,  "City",           "Iptc.Application2.City"))
        {
            return false;
        }
    }

    if (!location.location.isEmpty())
    {
        if (!setIptcTag(location.location,      32,  "SubLocation",    "Iptc.Application2.SubLocation"))
        {
            return false;
        }
    }

    if (!location.provinceState.isEmpty())
    {
        if (!setIptcTag(location.provinceState, 32,  "Province/State", "Iptc.Application2.ProvinceState"))
        {
            return false;
        }
    }

    return true;
}

QStringList DMetadata::getIptcCoreSubjects() const
{
    QStringList list = getXmpSubjects();

    if (!list.isEmpty())
    {
        return list;
    }

    return getIptcSubjects();
}

bool DMetadata::setIptcTag(const QString& text,
                           int maxLength,
                           const char* const debugLabel,
                           const char* const tagKey)  const
{
    QString truncatedText = text;
    truncatedText.truncate(maxLength);
    qCDebug(DIGIKAM_METAENGINE_LOG) << getFilePath() << " ==> " << debugLabel << ": " << truncatedText;

    return setIptcTagString(tagKey, truncatedText);    // returns false if failed
}

QVariant DMetadata::fromIptcEmulateList(const char* const iptcTagName) const
{
    return toStringListVariant(getIptcTagsStringList(iptcTagName));
}

QVariant DMetadata::fromIptcEmulateLangAlt(const char* const iptcTagName) const
{
    QString str = getIptcTagString(iptcTagName);
    QMap<QString, QVariant> map;

    if (str.isNull())
    {
#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))
        return QVariant(QMetaType(QMetaType::QVariantMap));
#else
        return QVariant(QVariant::Map);
#endif
    }

    map[QLatin1String("x-default")] = str;

    return map;
}

bool DMetadata::removeIptcTags(const QStringList& tagFilters)
{
    MetaDataMap m = getIptcTagsDataList(tagFilters);

    if (m.isEmpty())
    {
        return false;
    }

    for (MetaDataMap::iterator it = m.begin() ; it != m.end() ; ++it)
    {
        removeIptcTag(it.key().toLatin1().constData());
    }

    return true;
}

DMetadata::CountryCodeMap DMetadata::countryCodeMap()
{
    CountryCodeMap countryCodeMap;

    // All ISO 639-1 language code based on 2 characters
    // https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes

    countryCodeMap.insert( QLatin1String("AA"), i18nc("@info: country code", "Afar"));
    countryCodeMap.insert( QLatin1String("AB"), i18nc("@info: country code", "Abkhazian"));
    countryCodeMap.insert( QLatin1String("AF"), i18nc("@info: country code", "Afrikaans"));
    countryCodeMap.insert( QLatin1String("AM"), i18nc("@info: country code", "Amharic"));
    countryCodeMap.insert( QLatin1String("AR"), i18nc("@info: country code", "Arabic"));
    countryCodeMap.insert( QLatin1String("AS"), i18nc("@info: country code", "Assamese"));
    countryCodeMap.insert( QLatin1String("AY"), i18nc("@info: country code", "Aymara"));
    countryCodeMap.insert( QLatin1String("AZ"), i18nc("@info: country code", "Azerbaijani"));
    countryCodeMap.insert( QLatin1String("BA"), i18nc("@info: country code", "Bashkir"));
    countryCodeMap.insert( QLatin1String("BE"), i18nc("@info: country code", "Byelorussian"));
    countryCodeMap.insert( QLatin1String("BG"), i18nc("@info: country code", "Bulgarian"));
    countryCodeMap.insert( QLatin1String("BH"), i18nc("@info: country code", "Bihari"));
    countryCodeMap.insert( QLatin1String("BI"), i18nc("@info: country code", "Bislama"));
    countryCodeMap.insert( QLatin1String("BN"), i18nc("@info: country code", "Bengali;Bangla") );
    countryCodeMap.insert( QLatin1String("BO"), i18nc("@info: country code", "Tibetan"));
    countryCodeMap.insert( QLatin1String("BR"), i18nc("@info: country code", "Breton"));
    countryCodeMap.insert( QLatin1String("CA"), i18nc("@info: country code", "Catalan"));
    countryCodeMap.insert( QLatin1String("CO"), i18nc("@info: country code", "Corsican"));
    countryCodeMap.insert( QLatin1String("CS"), i18nc("@info: country code", "Czech"));
    countryCodeMap.insert( QLatin1String("CY"), i18nc("@info: country code", "Welsh"));
    countryCodeMap.insert( QLatin1String("DA"), i18nc("@info: country code", "Danish"));
    countryCodeMap.insert( QLatin1String("DE"), i18nc("@info: country code", "German"));
    countryCodeMap.insert( QLatin1String("DZ"), i18nc("@info: country code", "Dzongkha"));
    countryCodeMap.insert( QLatin1String("EL"), i18nc("@info: country code", "Greek"));
    countryCodeMap.insert( QLatin1String("EN"), i18nc("@info: country code", "English"));
    countryCodeMap.insert( QLatin1String("EO"), i18nc("@info: country code", "Esperanto"));
    countryCodeMap.insert( QLatin1String("ES"), i18nc("@info: country code", "Spanish"));
    countryCodeMap.insert( QLatin1String("ET"), i18nc("@info: country code", "Estonian"));
    countryCodeMap.insert( QLatin1String("EU"), i18nc("@info: country code", "Basque"));
    countryCodeMap.insert( QLatin1String("FA"), i18nc("@info: country code", "Persian(farsi)") );
    countryCodeMap.insert( QLatin1String("FI"), i18nc("@info: country code", "Finnish"));
    countryCodeMap.insert( QLatin1String("FJ"), i18nc("@info: country code", "Fijan"));
    countryCodeMap.insert( QLatin1String("FO"), i18nc("@info: country code", "Faroese") );
    countryCodeMap.insert( QLatin1String("FR"), i18nc("@info: country code", "French") );
    countryCodeMap.insert( QLatin1String("FY"), i18nc("@info: country code", "Frisian") );
    countryCodeMap.insert( QLatin1String("GA"), i18nc("@info: country code", "Irish") );
    countryCodeMap.insert( QLatin1String("GD"), i18nc("@info: country code", "Scotsgaelic") );
    countryCodeMap.insert( QLatin1String("GL"), i18nc("@info: country code", "Galician") );
    countryCodeMap.insert( QLatin1String("GN"), i18nc("@info: country code", "Guarani") );
    countryCodeMap.insert( QLatin1String("GU"), i18nc("@info: country code", "Gujarati") );
    countryCodeMap.insert( QLatin1String("HA"), i18nc("@info: country code", "Hausa") );
    countryCodeMap.insert( QLatin1String("HE"), i18nc("@info: country code", "Hebrew") );
    countryCodeMap.insert( QLatin1String("HI"), i18nc("@info: country code", "Hindi") );
    countryCodeMap.insert( QLatin1String("HR"), i18nc("@info: country code", "Croatian") );
    countryCodeMap.insert( QLatin1String("HU"), i18nc("@info: country code", "Hungarian") );
    countryCodeMap.insert( QLatin1String("HY"), i18nc("@info: country code", "Armenian") );
    countryCodeMap.insert( QLatin1String("IA"), i18nc("@info: country code", "Interlingua") );
    countryCodeMap.insert( QLatin1String("IE"), i18nc("@info: country code", "Interlingue") );
    countryCodeMap.insert( QLatin1String("IK"), i18nc("@info: country code", "Inupiak") );
    countryCodeMap.insert( QLatin1String("ID"), i18nc("@info: country code", "Indonesian") );
    countryCodeMap.insert( QLatin1String("IS"), i18nc("@info: country code", "Icelandic") );
    countryCodeMap.insert( QLatin1String("IT"), i18nc("@info: country code", "Italian") );
    countryCodeMap.insert( QLatin1String("IU"), i18nc("@info: country code", "Inuktitut") );
    countryCodeMap.insert( QLatin1String("JA"), i18nc("@info: country code", "Japanese") );
    countryCodeMap.insert( QLatin1String("JV"), i18nc("@info: country code", "Javanese") );
    countryCodeMap.insert( QLatin1String("KA"), i18nc("@info: country code", "Georgian") );
    countryCodeMap.insert( QLatin1String("KK"), i18nc("@info: country code", "Kazakh") );
    countryCodeMap.insert( QLatin1String("KL"), i18nc("@info: country code", "Greenlandic") );
    countryCodeMap.insert( QLatin1String("KM"), i18nc("@info: country code", "Cambodian") );
    countryCodeMap.insert( QLatin1String("KN"), i18nc("@info: country code", "Kannada") );
    countryCodeMap.insert( QLatin1String("KO"), i18nc("@info: country code", "Korean") );
    countryCodeMap.insert( QLatin1String("KS"), i18nc("@info: country code", "Kashmiri") );
    countryCodeMap.insert( QLatin1String("KU"), i18nc("@info: country code", "Kurdish") );
    countryCodeMap.insert( QLatin1String("KY"), i18nc("@info: country code", "Kirghiz") );
    countryCodeMap.insert( QLatin1String("LA"), i18nc("@info: country code", "Latin") );
    countryCodeMap.insert( QLatin1String("LN"), i18nc("@info: country code", "Lingala") );
    countryCodeMap.insert( QLatin1String("LO"), i18nc("@info: country code", "Laothian") );
    countryCodeMap.insert( QLatin1String("LT"), i18nc("@info: country code", "Lithuanian") );
    countryCodeMap.insert( QLatin1String("LV"), i18nc("@info: country code", "Latvian;Lettish") );
    countryCodeMap.insert( QLatin1String("MG"), i18nc("@info: country code", "Malagasy") );
    countryCodeMap.insert( QLatin1String("MI"), i18nc("@info: country code", "Maori") );
    countryCodeMap.insert( QLatin1String("MK"), i18nc("@info: country code", "Macedonian") );
    countryCodeMap.insert( QLatin1String("ML"), i18nc("@info: country code", "Malayalam") );
    countryCodeMap.insert( QLatin1String("MN"), i18nc("@info: country code", "Mongolian") );
    countryCodeMap.insert( QLatin1String("MR"), i18nc("@info: country code", "Marathi") );
    countryCodeMap.insert( QLatin1String("MS"), i18nc("@info: country code", "Malay") );
    countryCodeMap.insert( QLatin1String("MT"), i18nc("@info: country code", "Maltese") );
    countryCodeMap.insert( QLatin1String("MY"), i18nc("@info: country code", "Burmese") );
    countryCodeMap.insert( QLatin1String("NA"), i18nc("@info: country code", "Nauru") );
    countryCodeMap.insert( QLatin1String("NE"), i18nc("@info: country code", "Nepali") );
    countryCodeMap.insert( QLatin1String("NL"), i18nc("@info: country code", "Dutch") );
    countryCodeMap.insert( QLatin1String("NO"), i18nc("@info: country code", "Norwegian") );
    countryCodeMap.insert( QLatin1String("OC"), i18nc("@info: country code", "Occitan") );
    countryCodeMap.insert( QLatin1String("OM"), i18nc("@info: country code", "Oromo") );
    countryCodeMap.insert( QLatin1String("OR"), i18nc("@info: country code", "Oriya") );
    countryCodeMap.insert( QLatin1String("PA"), i18nc("@info: country code", "Punjabi") );
    countryCodeMap.insert( QLatin1String("PL"), i18nc("@info: country code", "Polish") );
    countryCodeMap.insert( QLatin1String("PS"), i18nc("@info: country code", "Pashto;Pushto") );
    countryCodeMap.insert( QLatin1String("PT"), i18nc("@info: country code", "Portuguese") );
    countryCodeMap.insert( QLatin1String("QU"), i18nc("@info: country code", "Quechua") );
    countryCodeMap.insert( QLatin1String("RM"), i18nc("@info: country code", "Rhaeto-romance") );
    countryCodeMap.insert( QLatin1String("RN"), i18nc("@info: country code", "Kurundi") );
    countryCodeMap.insert( QLatin1String("RO"), i18nc("@info: country code", "Romanian") );
    countryCodeMap.insert( QLatin1String("RU"), i18nc("@info: country code", "Russian") );
    countryCodeMap.insert( QLatin1String("RW"), i18nc("@info: country code", "Kinyarwanda") );
    countryCodeMap.insert( QLatin1String("SA"), i18nc("@info: country code", "Sanskrit") );
    countryCodeMap.insert( QLatin1String("SD"), i18nc("@info: country code", "Sindhi") );
    countryCodeMap.insert( QLatin1String("SG"), i18nc("@info: country code", "Sangho") );
    countryCodeMap.insert( QLatin1String("SI"), i18nc("@info: country code", "Singhalese") );
    countryCodeMap.insert( QLatin1String("SK"), i18nc("@info: country code", "Slovak") );
    countryCodeMap.insert( QLatin1String("SL"), i18nc("@info: country code", "Slovenian") );
    countryCodeMap.insert( QLatin1String("SM"), i18nc("@info: country code", "Samoan") );
    countryCodeMap.insert( QLatin1String("SN"), i18nc("@info: country code", "Shona") );
    countryCodeMap.insert( QLatin1String("SO"), i18nc("@info: country code", "Somali") );
    countryCodeMap.insert( QLatin1String("SQ"), i18nc("@info: country code", "Albanian") );
    countryCodeMap.insert( QLatin1String("SR"), i18nc("@info: country code", "Serbian") );
    countryCodeMap.insert( QLatin1String("SS"), i18nc("@info: country code", "Siswati") );
    countryCodeMap.insert( QLatin1String("ST"), i18nc("@info: country code", "Sesotho") );
    countryCodeMap.insert( QLatin1String("SU"), i18nc("@info: country code", "Sundanese") );
    countryCodeMap.insert( QLatin1String("SV"), i18nc("@info: country code", "Swedish") );
    countryCodeMap.insert( QLatin1String("SW"), i18nc("@info: country code", "Swahili") );
    countryCodeMap.insert( QLatin1String("TA"), i18nc("@info: country code", "Tamil") );
    countryCodeMap.insert( QLatin1String("TE"), i18nc("@info: country code", "Telugu") );
    countryCodeMap.insert( QLatin1String("TG"), i18nc("@info: country code", "Tajik") );
    countryCodeMap.insert( QLatin1String("TH"), i18nc("@info: country code", "Thai") );
    countryCodeMap.insert( QLatin1String("TI"), i18nc("@info: country code", "Tigrinya") );
    countryCodeMap.insert( QLatin1String("TK"), i18nc("@info: country code", "Turkmen") );
    countryCodeMap.insert( QLatin1String("TL"), i18nc("@info: country code", "Tagalog") );
    countryCodeMap.insert( QLatin1String("TN"), i18nc("@info: country code", "Setswana") );
    countryCodeMap.insert( QLatin1String("TO"), i18nc("@info: country code", "Tonga") );
    countryCodeMap.insert( QLatin1String("TR"), i18nc("@info: country code", "Turkish") );
    countryCodeMap.insert( QLatin1String("TS"), i18nc("@info: country code", "Tsonga") );
    countryCodeMap.insert( QLatin1String("TT"), i18nc("@info: country code", "Tatar") );
    countryCodeMap.insert( QLatin1String("TW"), i18nc("@info: country code", "Twi") );
    countryCodeMap.insert( QLatin1String("UG"), i18nc("@info: country code", "Uigur") );
    countryCodeMap.insert( QLatin1String("UK"), i18nc("@info: country code", "Ukrainian") );
    countryCodeMap.insert( QLatin1String("UR"), i18nc("@info: country code", "Urdu") );
    countryCodeMap.insert( QLatin1String("UZ"), i18nc("@info: country code", "Uzbek") );
    countryCodeMap.insert( QLatin1String("VI"), i18nc("@info: country code", "Vietnamese") );
    countryCodeMap.insert( QLatin1String("VO"), i18nc("@info: country code", "Volapuk") );
    countryCodeMap.insert( QLatin1String("WO"), i18nc("@info: country code", "Wolof") );
    countryCodeMap.insert( QLatin1String("XH"), i18nc("@info: country code", "Xhosa") );
    countryCodeMap.insert( QLatin1String("YI"), i18nc("@info: country code", "Yiddish") );
    countryCodeMap.insert( QLatin1String("YO"), i18nc("@info: country code", "Yoruba") );
    countryCodeMap.insert( QLatin1String("ZA"), i18nc("@info: country code", "Zhuang") );
    countryCodeMap.insert( QLatin1String("ZH"), i18nc("@info: country code", "Chinese") );
    countryCodeMap.insert( QLatin1String("ZU"), i18nc("@info: country code", "Zulu") );

    return countryCodeMap;
}

DMetadata::CountryCodeMap DMetadata::countryCodeMap2()
{
    CountryCodeMap countryCodeMap;

    // All ISO 639-2 language code based on 2 characters
    // https://en.wikipedia.org/wiki/List_of_ISO_639-2_codes

    countryCodeMap.insert( QLatin1String("aar"), i18nc("@info: country code", "Afar"));
    countryCodeMap.insert( QLatin1String("abk"), i18nc("@info: country code", "Abkhazian"));
    countryCodeMap.insert( QLatin1String("afr"), i18nc("@info: country code", "Afrikaans"));
    countryCodeMap.insert( QLatin1String("amh"), i18nc("@info: country code", "Amharic"));
    countryCodeMap.insert( QLatin1String("ara"), i18nc("@info: country code", "Arabic"));
    countryCodeMap.insert( QLatin1String("asm"), i18nc("@info: country code", "Assamese"));
    countryCodeMap.insert( QLatin1String("aym"), i18nc("@info: country code", "Aymara"));
    countryCodeMap.insert( QLatin1String("aze"), i18nc("@info: country code", "Azerbaijani"));
    countryCodeMap.insert( QLatin1String("bak"), i18nc("@info: country code", "Bashkir"));
    countryCodeMap.insert( QLatin1String("bel"), i18nc("@info: country code", "Byelorussian"));
    countryCodeMap.insert( QLatin1String("bul"), i18nc("@info: country code", "Bulgarian"));
    countryCodeMap.insert( QLatin1String("bih"), i18nc("@info: country code", "Bihari"));
    countryCodeMap.insert( QLatin1String("bis"), i18nc("@info: country code", "Bislama"));
    countryCodeMap.insert( QLatin1String("ben"), i18nc("@info: country code", "Bengali;Bangla") );
    countryCodeMap.insert( QLatin1String("bod"), i18nc("@info: country code", "Tibetan"));
    countryCodeMap.insert( QLatin1String("bre"), i18nc("@info: country code", "Breton"));
    countryCodeMap.insert( QLatin1String("cat"), i18nc("@info: country code", "Catalan"));
    countryCodeMap.insert( QLatin1String("cos"), i18nc("@info: country code", "Corsican"));
    countryCodeMap.insert( QLatin1String("ces"), i18nc("@info: country code", "Czech"));
    countryCodeMap.insert( QLatin1String("cym"), i18nc("@info: country code", "Welsh"));
    countryCodeMap.insert( QLatin1String("dan"), i18nc("@info: country code", "Danish"));
    countryCodeMap.insert( QLatin1String("deu"), i18nc("@info: country code", "German"));
    countryCodeMap.insert( QLatin1String("dzo"), i18nc("@info: country code", "Dzongkha"));
    countryCodeMap.insert( QLatin1String("ell"), i18nc("@info: country code", "Greek"));
    countryCodeMap.insert( QLatin1String("eng"), i18nc("@info: country code", "English"));
    countryCodeMap.insert( QLatin1String("epo"), i18nc("@info: country code", "Esperanto"));
    countryCodeMap.insert( QLatin1String("spa"), i18nc("@info: country code", "Spanish"));
    countryCodeMap.insert( QLatin1String("est"), i18nc("@info: country code", "Estonian"));
    countryCodeMap.insert( QLatin1String("eus"), i18nc("@info: country code", "Basque"));
    countryCodeMap.insert( QLatin1String("fas"), i18nc("@info: country code", "Persian(farsi)") );
    countryCodeMap.insert( QLatin1String("fin"), i18nc("@info: country code", "Finnish"));
    countryCodeMap.insert( QLatin1String("fij"), i18nc("@info: country code", "Fijan"));
    countryCodeMap.insert( QLatin1String("fao"), i18nc("@info: country code", "Faroese") );
    countryCodeMap.insert( QLatin1String("fra"), i18nc("@info: country code", "French") );
    countryCodeMap.insert( QLatin1String("fry"), i18nc("@info: country code", "Frisian") );
    countryCodeMap.insert( QLatin1String("gle"), i18nc("@info: country code", "Irish") );
    countryCodeMap.insert( QLatin1String("gla"), i18nc("@info: country code", "Scotsgaelic") );
    countryCodeMap.insert( QLatin1String("glg"), i18nc("@info: country code", "Galician") );
    countryCodeMap.insert( QLatin1String("grn"), i18nc("@info: country code", "Guarani") );
    countryCodeMap.insert( QLatin1String("guj"), i18nc("@info: country code", "Gujarati") );
    countryCodeMap.insert( QLatin1String("hau"), i18nc("@info: country code", "Hausa") );
    countryCodeMap.insert( QLatin1String("heb"), i18nc("@info: country code", "Hebrew") );
    countryCodeMap.insert( QLatin1String("hin"), i18nc("@info: country code", "Hindi") );
    countryCodeMap.insert( QLatin1String("hrv"), i18nc("@info: country code", "Croatian") );
    countryCodeMap.insert( QLatin1String("hun"), i18nc("@info: country code", "Hungarian") );
    countryCodeMap.insert( QLatin1String("hye"), i18nc("@info: country code", "Armenian") );
    countryCodeMap.insert( QLatin1String("ina"), i18nc("@info: country code", "Interlingua") );
    countryCodeMap.insert( QLatin1String("ile"), i18nc("@info: country code", "Interlingue") );
    countryCodeMap.insert( QLatin1String("ipk"), i18nc("@info: country code", "Inupiak") );
    countryCodeMap.insert( QLatin1String("ind"), i18nc("@info: country code", "Indonesian") );
    countryCodeMap.insert( QLatin1String("isl"), i18nc("@info: country code", "Icelandic") );
    countryCodeMap.insert( QLatin1String("ita"), i18nc("@info: country code", "Italian") );
    countryCodeMap.insert( QLatin1String("iku"), i18nc("@info: country code", "Inuktitut") );
    countryCodeMap.insert( QLatin1String("jpn"), i18nc("@info: country code", "Japanese") );
    countryCodeMap.insert( QLatin1String("jav"), i18nc("@info: country code", "Javanese") );
    countryCodeMap.insert( QLatin1String("kat"), i18nc("@info: country code", "Georgian") );
    countryCodeMap.insert( QLatin1String("kaz"), i18nc("@info: country code", "Kazakh") );
    countryCodeMap.insert( QLatin1String("kal"), i18nc("@info: country code", "Greenlandic") );
    countryCodeMap.insert( QLatin1String("khm"), i18nc("@info: country code", "Cambodian") );
    countryCodeMap.insert( QLatin1String("kan"), i18nc("@info: country code", "Kannada") );
    countryCodeMap.insert( QLatin1String("kor"), i18nc("@info: country code", "Korean") );
    countryCodeMap.insert( QLatin1String("kas"), i18nc("@info: country code", "Kashmiri") );
    countryCodeMap.insert( QLatin1String("kur"), i18nc("@info: country code", "Kurdish") );
    countryCodeMap.insert( QLatin1String("kir"), i18nc("@info: country code", "Kirghiz") );
    countryCodeMap.insert( QLatin1String("lat"), i18nc("@info: country code", "Latin") );
    countryCodeMap.insert( QLatin1String("lin"), i18nc("@info: country code", "Lingala") );
    countryCodeMap.insert( QLatin1String("lao"), i18nc("@info: country code", "Laothian") );
    countryCodeMap.insert( QLatin1String("lit"), i18nc("@info: country code", "Lithuanian") );
    countryCodeMap.insert( QLatin1String("lav"), i18nc("@info: country code", "Latvian;Lettish") );
    countryCodeMap.insert( QLatin1String("mlg"), i18nc("@info: country code", "Malagasy") );
    countryCodeMap.insert( QLatin1String("mri"), i18nc("@info: country code", "Maori") );
    countryCodeMap.insert( QLatin1String("mkd"), i18nc("@info: country code", "Macedonian") );
    countryCodeMap.insert( QLatin1String("mal"), i18nc("@info: country code", "Malayalam") );
    countryCodeMap.insert( QLatin1String("mon"), i18nc("@info: country code", "Mongolian") );
    countryCodeMap.insert( QLatin1String("mar"), i18nc("@info: country code", "Marathi") );
    countryCodeMap.insert( QLatin1String("msa"), i18nc("@info: country code", "Malay") );
    countryCodeMap.insert( QLatin1String("mlt"), i18nc("@info: country code", "Maltese") );
    countryCodeMap.insert( QLatin1String("mya"), i18nc("@info: country code", "Burmese") );
    countryCodeMap.insert( QLatin1String("nau"), i18nc("@info: country code", "Nauru") );
    countryCodeMap.insert( QLatin1String("nep"), i18nc("@info: country code", "Nepali") );
    countryCodeMap.insert( QLatin1String("nld"), i18nc("@info: country code", "Dutch") );
    countryCodeMap.insert( QLatin1String("nor"), i18nc("@info: country code", "Norwegian") );
    countryCodeMap.insert( QLatin1String("oci"), i18nc("@info: country code", "Occitan") );
    countryCodeMap.insert( QLatin1String("orm"), i18nc("@info: country code", "Oromo") );
    countryCodeMap.insert( QLatin1String("ori"), i18nc("@info: country code", "Oriya") );
    countryCodeMap.insert( QLatin1String("pan"), i18nc("@info: country code", "Punjabi") );
    countryCodeMap.insert( QLatin1String("pol"), i18nc("@info: country code", "Polish") );
    countryCodeMap.insert( QLatin1String("pus"), i18nc("@info: country code", "Pashto;Pushto") );
    countryCodeMap.insert( QLatin1String("por"), i18nc("@info: country code", "Portuguese") );
    countryCodeMap.insert( QLatin1String("que"), i18nc("@info: country code", "Quechua") );
    countryCodeMap.insert( QLatin1String("roh"), i18nc("@info: country code", "Rhaeto-romance") );
    countryCodeMap.insert( QLatin1String("run"), i18nc("@info: country code", "Kurundi") );
    countryCodeMap.insert( QLatin1String("ron"), i18nc("@info: country code", "Romanian") );
    countryCodeMap.insert( QLatin1String("rus"), i18nc("@info: country code", "Russian") );
    countryCodeMap.insert( QLatin1String("kin"), i18nc("@info: country code", "Kinyarwanda") );
    countryCodeMap.insert( QLatin1String("san"), i18nc("@info: country code", "Sanskrit") );
    countryCodeMap.insert( QLatin1String("snd"), i18nc("@info: country code", "Sindhi") );
    countryCodeMap.insert( QLatin1String("sag"), i18nc("@info: country code", "Sangho") );
    countryCodeMap.insert( QLatin1String("sin"), i18nc("@info: country code", "Singhalese") );
    countryCodeMap.insert( QLatin1String("slk"), i18nc("@info: country code", "Slovak") );
    countryCodeMap.insert( QLatin1String("slv"), i18nc("@info: country code", "Slovenian") );
    countryCodeMap.insert( QLatin1String("smo"), i18nc("@info: country code", "Samoan") );
    countryCodeMap.insert( QLatin1String("sna"), i18nc("@info: country code", "Shona") );
    countryCodeMap.insert( QLatin1String("som"), i18nc("@info: country code", "Somali") );
    countryCodeMap.insert( QLatin1String("sqi"), i18nc("@info: country code", "Albanian") );
    countryCodeMap.insert( QLatin1String("srp"), i18nc("@info: country code", "Serbian") );
    countryCodeMap.insert( QLatin1String("ssw"), i18nc("@info: country code", "Siswati") );
    countryCodeMap.insert( QLatin1String("sot"), i18nc("@info: country code", "Sesotho") );
    countryCodeMap.insert( QLatin1String("sun"), i18nc("@info: country code", "Sundanese") );
    countryCodeMap.insert( QLatin1String("swe"), i18nc("@info: country code", "Swedish") );
    countryCodeMap.insert( QLatin1String("swa"), i18nc("@info: country code", "Swahili") );
    countryCodeMap.insert( QLatin1String("tam"), i18nc("@info: country code", "Tamil") );
    countryCodeMap.insert( QLatin1String("tel"), i18nc("@info: country code", "Telugu") );
    countryCodeMap.insert( QLatin1String("tgk"), i18nc("@info: country code", "Tajik") );
    countryCodeMap.insert( QLatin1String("tha"), i18nc("@info: country code", "Thai") );
    countryCodeMap.insert( QLatin1String("tir"), i18nc("@info: country code", "Tigrinya") );
    countryCodeMap.insert( QLatin1String("tuk"), i18nc("@info: country code", "Turkmen") );
    countryCodeMap.insert( QLatin1String("tgl"), i18nc("@info: country code", "Tagalog") );
    countryCodeMap.insert( QLatin1String("tsn"), i18nc("@info: country code", "Setswana") );
    countryCodeMap.insert( QLatin1String("ton"), i18nc("@info: country code", "Tonga") );
    countryCodeMap.insert( QLatin1String("tur"), i18nc("@info: country code", "Turkish") );
    countryCodeMap.insert( QLatin1String("tso"), i18nc("@info: country code", "Tsonga") );
    countryCodeMap.insert( QLatin1String("tat"), i18nc("@info: country code", "Tatar") );
    countryCodeMap.insert( QLatin1String("twi"), i18nc("@info: country code", "Twi") );
    countryCodeMap.insert( QLatin1String("uig"), i18nc("@info: country code", "Uigur") );
    countryCodeMap.insert( QLatin1String("ukr"), i18nc("@info: country code", "Ukrainian") );
    countryCodeMap.insert( QLatin1String("urd"), i18nc("@info: country code", "Urdu") );
    countryCodeMap.insert( QLatin1String("uzb"), i18nc("@info: country code", "Uzbek") );
    countryCodeMap.insert( QLatin1String("vie"), i18nc("@info: country code", "Vietnamese") );
    countryCodeMap.insert( QLatin1String("vol"), i18nc("@info: country code", "Volapuk") );
    countryCodeMap.insert( QLatin1String("wol"), i18nc("@info: country code", "Wolof") );
    countryCodeMap.insert( QLatin1String("xho"), i18nc("@info: country code", "Xhosa") );
    countryCodeMap.insert( QLatin1String("yid"), i18nc("@info: country code", "Yiddish") );
    countryCodeMap.insert( QLatin1String("yor"), i18nc("@info: country code", "Yoruba") );
    countryCodeMap.insert( QLatin1String("zha"), i18nc("@info: country code", "Zhuang") );
    countryCodeMap.insert( QLatin1String("zho"), i18nc("@info: country code", "Chinese") );
    countryCodeMap.insert( QLatin1String("zul"), i18nc("@info: country code", "Zulu") );

    return countryCodeMap;
}

} // namespace Digikam
