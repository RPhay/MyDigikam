/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-02-23
 * Description : item metadata interface - Iptc helpers.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2011      by Leif Huhn <leif at dkstat dot com>
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

    if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrCity", info.city))
    {
        return false;
    }

    if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrCtry", info.country))
    {
        return false;
    }

    if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrExtadr", info.address))
    {
        return false;
    }

    if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrPcode", info.postalCode))
    {
        return false;
    }

    if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiAdrRegion", info.provinceState))
    {
        return false;
    }

    if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiEmailWork", info.email))
    {
        return false;
    }

    if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiTelWork", info.phone))
    {
        return false;
    }

    if (!setXmpTagString("Xmp.iptc.CreatorContactInfo/Iptc4xmpCore:CiUrlWork", info.webUrl))
    {
        return false;
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
        if (!setXmpTagString("Xmp.photoshop.Country", location.country))
        {
            return false;
        }

        if (!setXmpTagString("Xmp.iptc.CountryCode", location.countryCode))
        {
            return false;
        }

        if (!setXmpTagString("Xmp.photoshop.City", location.city))
        {
            return false;
        }

        if (!setXmpTagString("Xmp.iptc.Location", location.location))
        {
            return false;
        }

        if (!setXmpTagString("Xmp.photoshop.State", location.provinceState))
        {
            return false;
        }
    }

    if (!setIptcTag(location.country,       64,  "Country",        "Iptc.Application2.CountryName"))
    {
        return false;
    }

    if (!setIptcTag(location.countryCode,    3,  "Country Code",   "Iptc.Application2.CountryCode"))
    {
        return false;
    }

    if (!setIptcTag(location.city,          32,  "City",           "Iptc.Application2.City"))
    {
        return false;
    }

    if (!setIptcTag(location.location,      32,  "SubLocation",    "Iptc.Application2.SubLocation"))
    {
        return false;
    }

    if (!setIptcTag(location.provinceState, 32,  "Province/State", "Iptc.Application2.ProvinceState"))
    {
        return false;
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

    if (str.isNull())
    {
        return QVariant(QVariant::Map);
    }

    QMap<QString, QVariant> map;
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

    // All ISO 639 language code based on 2 characters
    // xml.coverpages.org/iso639a.html

    countryCodeMap.insert( QLatin1String("AA"), i18nc("@info: contry code", "Afar"));
    countryCodeMap.insert( QLatin1String("AB"), i18nc("@info: contry code", "Abkhazian"));
    countryCodeMap.insert( QLatin1String("AF"), i18nc("@info: contry code", "Afrikaans"));
    countryCodeMap.insert( QLatin1String("AM"), i18nc("@info: contry code", "Amharic"));
    countryCodeMap.insert( QLatin1String("AR"), i18nc("@info: contry code", "Arabic"));
    countryCodeMap.insert( QLatin1String("AS"), i18nc("@info: contry code", "Assamese"));
    countryCodeMap.insert( QLatin1String("AY"), i18nc("@info: contry code", "Aymara"));
    countryCodeMap.insert( QLatin1String("AZ"), i18nc("@info: contry code", "Azerbaijani"));
    countryCodeMap.insert( QLatin1String("BA"), i18nc("@info: contry code", "Bashkir"));
    countryCodeMap.insert( QLatin1String("BE"), i18nc("@info: contry code", "Byelorussian"));
    countryCodeMap.insert( QLatin1String("BG"), i18nc("@info: contry code", "Bulgarian"));
    countryCodeMap.insert( QLatin1String("BH"), i18nc("@info: contry code", "Bihari"));
    countryCodeMap.insert( QLatin1String("BI"), i18nc("@info: contry code", "Bislama"));
    countryCodeMap.insert( QLatin1String("BN"), i18nc("@info: contry code", "Bengali;Bangla") );
    countryCodeMap.insert( QLatin1String("BO"), i18nc("@info: contry code", "Tibetan"));
    countryCodeMap.insert( QLatin1String("BR"), i18nc("@info: contry code", "Breton"));
    countryCodeMap.insert( QLatin1String("CA"), i18nc("@info: contry code", "Catalan"));
    countryCodeMap.insert( QLatin1String("CO"), i18nc("@info: contry code", "Corsican"));
    countryCodeMap.insert( QLatin1String("CS"), i18nc("@info: contry code", "Czech"));
    countryCodeMap.insert( QLatin1String("CY"), i18nc("@info: contry code", "Welsh"));
    countryCodeMap.insert( QLatin1String("DA"), i18nc("@info: contry code", "Danish"));
    countryCodeMap.insert( QLatin1String("DE"), i18nc("@info: contry code", "German"));
    countryCodeMap.insert( QLatin1String("DZ"), i18nc("@info: contry code", "Bhutani"));
    countryCodeMap.insert( QLatin1String("EL"), i18nc("@info: contry code", "Greek"));
    countryCodeMap.insert( QLatin1String("EN"), i18nc("@info: contry code", "English"));
    countryCodeMap.insert( QLatin1String("EO"), i18nc("@info: contry code", "Esperanto"));
    countryCodeMap.insert( QLatin1String("ES"), i18nc("@info: contry code", "Spanish"));
    countryCodeMap.insert( QLatin1String("ET"), i18nc("@info: contry code", "Estonian"));
    countryCodeMap.insert( QLatin1String("EU"), i18nc("@info: contry code", "Basque"));
    countryCodeMap.insert( QLatin1String("FA"), i18nc("@info: contry code", "Persian(farsi)") );
    countryCodeMap.insert( QLatin1String("FI"), i18nc("@info: contry code", "Finnish"));
    countryCodeMap.insert( QLatin1String("FJ"), i18nc("@info: contry code", "Fiji"));
    countryCodeMap.insert( QLatin1String("FO"), i18nc("@info: contry code", "Faroese") );
    countryCodeMap.insert( QLatin1String("FR"), i18nc("@info: contry code", "French") );
    countryCodeMap.insert( QLatin1String("FY"), i18nc("@info: contry code", "Frisian") );
    countryCodeMap.insert( QLatin1String("GA"), i18nc("@info: contry code", "Irish") );
    countryCodeMap.insert( QLatin1String("GD"), i18nc("@info: contry code", "Scotsgaelic") );
    countryCodeMap.insert( QLatin1String("GL"), i18nc("@info: contry code", "Galician") );
    countryCodeMap.insert( QLatin1String("GN"), i18nc("@info: contry code", "Guarani") );
    countryCodeMap.insert( QLatin1String("GU"), i18nc("@info: contry code", "Gujarati") );
    countryCodeMap.insert( QLatin1String("HA"), i18nc("@info: contry code", "Hausa") );
    countryCodeMap.insert( QLatin1String("HE"), i18nc("@info: contry code", "Hebrew") );
    countryCodeMap.insert( QLatin1String("HI"), i18nc("@info: contry code", "Hindi") );
    countryCodeMap.insert( QLatin1String("HR"), i18nc("@info: contry code", "Croatian") );
    countryCodeMap.insert( QLatin1String("HU"), i18nc("@info: contry code", "Hungarian") );
    countryCodeMap.insert( QLatin1String("HY"), i18nc("@info: contry code", "Armenian") );
    countryCodeMap.insert( QLatin1String("IA"), i18nc("@info: contry code", "Interlingua") );
    countryCodeMap.insert( QLatin1String("IE"), i18nc("@info: contry code", "Interlingue") );
    countryCodeMap.insert( QLatin1String("IK"), i18nc("@info: contry code", "Inupiak") );
    countryCodeMap.insert( QLatin1String("ID"), i18nc("@info: contry code", "Indonesian") );
    countryCodeMap.insert( QLatin1String("IS"), i18nc("@info: contry code", "Icelandic") );
    countryCodeMap.insert( QLatin1String("IT"), i18nc("@info: contry code", "Italian") );
    countryCodeMap.insert( QLatin1String("IU"), i18nc("@info: contry code", "Inuktitut") );
    countryCodeMap.insert( QLatin1String("JA"), i18nc("@info: contry code", "Japanese") );
    countryCodeMap.insert( QLatin1String("JV"), i18nc("@info: contry code", "Javanese") );
    countryCodeMap.insert( QLatin1String("KA"), i18nc("@info: contry code", "Georgian") );
    countryCodeMap.insert( QLatin1String("KK"), i18nc("@info: contry code", "Kazakh") );
    countryCodeMap.insert( QLatin1String("KL"), i18nc("@info: contry code", "Greenlandic") );
    countryCodeMap.insert( QLatin1String("KM"), i18nc("@info: contry code", "Cambodian") );
    countryCodeMap.insert( QLatin1String("KN"), i18nc("@info: contry code", "Kannada") );
    countryCodeMap.insert( QLatin1String("KO"), i18nc("@info: contry code", "Korean") );
    countryCodeMap.insert( QLatin1String("KS"), i18nc("@info: contry code", "Kashmiri") );
    countryCodeMap.insert( QLatin1String("KU"), i18nc("@info: contry code", "Kurdish") );
    countryCodeMap.insert( QLatin1String("KY"), i18nc("@info: contry code", "Kirghiz") );
    countryCodeMap.insert( QLatin1String("LA"), i18nc("@info: contry code", "Latin") );
    countryCodeMap.insert( QLatin1String("LN"), i18nc("@info: contry code", "Lingala") );
    countryCodeMap.insert( QLatin1String("LO"), i18nc("@info: contry code", "Laothian") );
    countryCodeMap.insert( QLatin1String("LT"), i18nc("@info: contry code", "Lithuanian") );
    countryCodeMap.insert( QLatin1String("LV"), i18nc("@info: contry code", "Latvian;Lettish") );
    countryCodeMap.insert( QLatin1String("MG"), i18nc("@info: contry code", "Malagasy") );
    countryCodeMap.insert( QLatin1String("MI"), i18nc("@info: contry code", "Maori") );
    countryCodeMap.insert( QLatin1String("MK"), i18nc("@info: contry code", "Macedonian") );
    countryCodeMap.insert( QLatin1String("ML"), i18nc("@info: contry code", "Malayalam") );
    countryCodeMap.insert( QLatin1String("MN"), i18nc("@info: contry code", "Mongolian") );
    countryCodeMap.insert( QLatin1String("MO"), i18nc("@info: contry code", "Moldavian") );
    countryCodeMap.insert( QLatin1String("MR"), i18nc("@info: contry code", "Marathi") );
    countryCodeMap.insert( QLatin1String("MS"), i18nc("@info: contry code", "Malay") );
    countryCodeMap.insert( QLatin1String("MT"), i18nc("@info: contry code", "Maltese") );
    countryCodeMap.insert( QLatin1String("MY"), i18nc("@info: contry code", "Burmese") );
    countryCodeMap.insert( QLatin1String("NA"), i18nc("@info: contry code", "Nauru") );
    countryCodeMap.insert( QLatin1String("NE"), i18nc("@info: contry code", "Nepali") );
    countryCodeMap.insert( QLatin1String("NL"), i18nc("@info: contry code", "Dutch") );
    countryCodeMap.insert( QLatin1String("NO"), i18nc("@info: contry code", "Norwegian") );
    countryCodeMap.insert( QLatin1String("OC"), i18nc("@info: contry code", "Occitan") );
    countryCodeMap.insert( QLatin1String("OM"), i18nc("@info: contry code", "Afan(oromo)") );
    countryCodeMap.insert( QLatin1String("OR"), i18nc("@info: contry code", "Oriya") );
    countryCodeMap.insert( QLatin1String("PA"), i18nc("@info: contry code", "Punjabi") );
    countryCodeMap.insert( QLatin1String("PL"), i18nc("@info: contry code", "Polish") );
    countryCodeMap.insert( QLatin1String("PS"), i18nc("@info: contry code", "Pashto;Pushto") );
    countryCodeMap.insert( QLatin1String("PT"), i18nc("@info: contry code", "Portuguese") );
    countryCodeMap.insert( QLatin1String("QU"), i18nc("@info: contry code", "Quechua") );
    countryCodeMap.insert( QLatin1String("RM"), i18nc("@info: contry code", "Rhaeto-romance") );
    countryCodeMap.insert( QLatin1String("RN"), i18nc("@info: contry code", "Kurundi") );
    countryCodeMap.insert( QLatin1String("RO"), i18nc("@info: contry code", "Romanian") );
    countryCodeMap.insert( QLatin1String("RU"), i18nc("@info: contry code", "Russian") );
    countryCodeMap.insert( QLatin1String("RW"), i18nc("@info: contry code", "Kinyarwanda") );
    countryCodeMap.insert( QLatin1String("SA"), i18nc("@info: contry code", "Sanskrit") );
    countryCodeMap.insert( QLatin1String("SD"), i18nc("@info: contry code", "Sindhi") );
    countryCodeMap.insert( QLatin1String("SG"), i18nc("@info: contry code", "Sangho") );
    countryCodeMap.insert( QLatin1String("SH"), i18nc("@info: contry code", "Serbo-croatian") );
    countryCodeMap.insert( QLatin1String("SI"), i18nc("@info: contry code", "Singhalese") );
    countryCodeMap.insert( QLatin1String("SK"), i18nc("@info: contry code", "Slovak") );
    countryCodeMap.insert( QLatin1String("SL"), i18nc("@info: contry code", "Slovenian") );
    countryCodeMap.insert( QLatin1String("SM"), i18nc("@info: contry code", "Samoan") );
    countryCodeMap.insert( QLatin1String("SN"), i18nc("@info: contry code", "Shona") );
    countryCodeMap.insert( QLatin1String("SO"), i18nc("@info: contry code", "Somali") );
    countryCodeMap.insert( QLatin1String("SQ"), i18nc("@info: contry code", "Albanian") );
    countryCodeMap.insert( QLatin1String("SR"), i18nc("@info: contry code", "Serbian") );
    countryCodeMap.insert( QLatin1String("SS"), i18nc("@info: contry code", "Siswati") );
    countryCodeMap.insert( QLatin1String("ST"), i18nc("@info: contry code", "Sesotho") );
    countryCodeMap.insert( QLatin1String("SU"), i18nc("@info: contry code", "Sundanese") );
    countryCodeMap.insert( QLatin1String("SV"), i18nc("@info: contry code", "Swedish") );
    countryCodeMap.insert( QLatin1String("SW"), i18nc("@info: contry code", "Swahili") );
    countryCodeMap.insert( QLatin1String("TA"), i18nc("@info: contry code", "Tamil") );
    countryCodeMap.insert( QLatin1String("TE"), i18nc("@info: contry code", "Telugu") );
    countryCodeMap.insert( QLatin1String("TG"), i18nc("@info: contry code", "Tajik") );
    countryCodeMap.insert( QLatin1String("TH"), i18nc("@info: contry code", "Thai") );
    countryCodeMap.insert( QLatin1String("TI"), i18nc("@info: contry code", "Tigrinya") );
    countryCodeMap.insert( QLatin1String("TK"), i18nc("@info: contry code", "Turkmen") );
    countryCodeMap.insert( QLatin1String("TL"), i18nc("@info: contry code", "Tagalog") );
    countryCodeMap.insert( QLatin1String("TN"), i18nc("@info: contry code", "Setswana") );
    countryCodeMap.insert( QLatin1String("TO"), i18nc("@info: contry code", "Tonga") );
    countryCodeMap.insert( QLatin1String("TR"), i18nc("@info: contry code", "Turkish") );
    countryCodeMap.insert( QLatin1String("TS"), i18nc("@info: contry code", "Tsonga") );
    countryCodeMap.insert( QLatin1String("TT"), i18nc("@info: contry code", "Tatar") );
    countryCodeMap.insert( QLatin1String("TW"), i18nc("@info: contry code", "Twi") );
    countryCodeMap.insert( QLatin1String("UG"), i18nc("@info: contry code", "Uigur") );
    countryCodeMap.insert( QLatin1String("UK"), i18nc("@info: contry code", "Ukrainian") );
    countryCodeMap.insert( QLatin1String("UR"), i18nc("@info: contry code", "Urdu") );
    countryCodeMap.insert( QLatin1String("UZ"), i18nc("@info: contry code", "Uzbek") );
    countryCodeMap.insert( QLatin1String("VI"), i18nc("@info: contry code", "Vietnamese") );
    countryCodeMap.insert( QLatin1String("VO"), i18nc("@info: contry code", "Volapuk") );
    countryCodeMap.insert( QLatin1String("WO"), i18nc("@info: contry code", "Wolof") );
    countryCodeMap.insert( QLatin1String("XH"), i18nc("@info: contry code", "Xhosa") );
    countryCodeMap.insert( QLatin1String("YI"), i18nc("@info: contry code", "Yiddish") );
    countryCodeMap.insert( QLatin1String("YO"), i18nc("@info: contry code", "Yoruba") );
    countryCodeMap.insert( QLatin1String("ZA"), i18nc("@info: contry code", "Zhuang") );
    countryCodeMap.insert( QLatin1String("ZH"), i18nc("@info: contry code", "Chinese") );
    countryCodeMap.insert( QLatin1String("ZU"), i18nc("@info: contry code", "Zulu") );

    return countryCodeMap;
}

} // namespace Digikam
