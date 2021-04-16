/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a tool to export images to iNaturalist web service
 *
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "inatutils.h"

// Qt includes

#include <QHttpPart>
#include <QFile>
#include <QFileInfo>
#include <QtMath>

// KDE includes

#include <klocalizedstring.h>

namespace DigikamGenericINatPlugin
{

// Constants

const QLocale locale      = QLocale();
const bool    isEnglish   = (locale.language() == QLocale::English)     ||
                            (locale.language() == QLocale::C)           ||
                            (locale.language() == QLocale::AnyLanguage);

const double meterInFeet  = 3.28084;
const double meterInMiles = 0.00062137;

// Functions

QHttpMultiPart* getMultiPart(const QList<Parameter>& parameters,
                             const QString& imageName, const QString& imagePath)
{
    static const QString paramForm = QLatin1String("form-data; name=\"%1\"");
    static const QString imageForm = QLatin1String("form-data; name=\"%1\"; "
                                                   "filename=\"%2\"");

    QHttpMultiPart* const result = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    for (auto param : parameters)
    {
        QHttpPart part;
        part.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant(paramForm.arg(param.first)));
        part.setBody(param.second.toUtf8());
        result->append(part);
    }

    if (!imageName.isEmpty() && !imagePath.isEmpty())
    {
        QHttpPart imagePart;
        QFileInfo fileInfo(imagePath);
        imagePart.setHeader(QNetworkRequest::ContentTypeHeader,
                            QVariant(QString(QLatin1String("image/%1")).
                                     arg(fileInfo.suffix().toLower())));
        imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant(imageForm.arg(imageName,
                                     fileInfo.baseName())));
        QFile* file = new QFile(imagePath);
        file->open(QIODevice::ReadOnly);
        imagePart.setBodyDevice(file);
        file->setParent(result);
        result->append(imagePart);
    }

    return result;
}

/**
 * converts degree to radian
 */
static inline double deg2rad(double deg)
{
    return deg * M_PI / 180;
}

/**
 * returns distance in meters between two coordinates, Haversine formula
 */
double distanceBetween(double latitude1, double longitude1,
                       double latitude2, double longitude2)
{
    static const double earthRadiusInMeters = 6.371e6;

    double lat1r = deg2rad(latitude1);
    double lon1r = deg2rad(longitude1);
    double lat2r = deg2rad(latitude2);
    double lon2r = deg2rad(longitude2);
    double u     = sin((lat2r - lat1r) / 2);
    double v     = sin((lon2r - lon1r) / 2);

    return (2.0 * earthRadiusInMeters *
            asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v)));
}

QString localizedTaxonomicRank(const QString& rank)
{
    if      (rank == QLatin1String("kingdom"))
    {
        return i18nc("taxonomic ranks", "kingdom");
    }
    else if (rank == QLatin1String("subkingdom"))
    {
        return i18nc("taxonomic ranks", "subkingdom");
    }
    else if (rank == QLatin1String("phylum"))
    {
        return i18nc("taxonomic ranks", "phylum");
    }
    else if (rank == QLatin1String("subphylum"))
    {
        return i18nc("taxonomic ranks", "subphylum");
    }
    else if (rank == QLatin1String("superorder"))
    {
        return i18nc("taxonomic ranks", "superorder");
    }
    else if (rank == QLatin1String("order"))
    {
        return i18nc("taxonomic ranks", "order");
    }
    else if (rank == QLatin1String("suborder"))
    {
        return i18nc("taxonomic ranks", "suborder");
    }
    else if (rank == QLatin1String("infraorder"))
    {
        return i18nc("taxonomic ranks", "infraorder");
    }
    else if (rank == QLatin1String("parvorder"))
    {
        return i18nc("taxonomic ranks", "parvorder");
    }
    else if (rank == QLatin1String("zoosection"))
    {
        return i18nc("taxonomic ranks", "zoosection");
    }
    else if (rank == QLatin1String("zoosubsection"))
    {
        return i18nc("taxonomic ranks", "zoosubsection");
    }
    else if (rank == QLatin1String("superfamily"))
    {
        return i18nc("taxonomic ranks", "superfamily");
    }
    else if (rank == QLatin1String("epifamily"))
    {
        return i18nc("taxonomic ranks", "epifamily");
    }
    else if (rank == QLatin1String("family"))
    {
        return i18nc("taxonomic ranks", "family");
    }
    else if (rank == QLatin1String("subfamily"))
    {
        return i18nc("taxonomic ranks", "subfamily");
    }
    else if (rank == QLatin1String("supertribe"))
    {
        return i18nc("taxonomic ranks", "supertribe");
    }
    else if (rank == QLatin1String("tribe"))
    {
        return i18nc("taxonomic ranks", "tribe");
    }
    else if (rank == QLatin1String("subtribe"))
    {
        return i18nc("taxonomic ranks", "subtribe");
    }
    else if (rank == QLatin1String("genus"))
    {
        return i18nc("taxonomic ranks", "genus");
    }
    else if (rank == QLatin1String("genushybrid"))
    {
        return i18nc("taxonomic ranks", "genushybrid");
    }
    else if (rank == QLatin1String("subgenus"))
    {
        return i18nc("taxonomic ranks", "subgenus");
    }
    else if (rank == QLatin1String("section"))
    {
        return i18nc("taxonomic ranks", "section");
    }
    else if (rank == QLatin1String("subsection"))
    {
        return i18nc("taxonomic ranks", "subsection");
    }
    else if (rank == QLatin1String("complex"))
    {
        return i18nc("taxonomic ranks", "complex");
    }
    else if (rank == QLatin1String("species"))
    {
        return i18nc("taxonomic ranks", "species");
    }
    else if (rank == QLatin1String("hybrid"))
    {
        return i18nc("taxonomic ranks", "hybrid");
    }
    else if (rank == QLatin1String("subspecies"))
    {
        return i18nc("taxonomic ranks", "subspecies");
    }
    else if (rank == QLatin1String("variety"))
    {
        return i18nc("taxonomic ranks", "variety");
    }
    else if (rank == QLatin1String("form"))
    {
        return i18nc("taxonomic ranks", "form");
    }
    else if (rank == QLatin1String("infrahybrid"))
    {
        return i18nc("taxonomic ranks", "infrahybrid");
    }
    else
    {
        return rank;
    }
}

QString localizedLocation(double latitude, double longitude, int precision)
{
    return (locale.toString(latitude, 'f', precision) +
            QLatin1String(", ") + locale.toString(longitude, 'f', precision));
}

QString localizedDistance(double distMeters, char format, int precision)
{
    if (locale.measurementSystem() == QLocale::ImperialUSSystem)
    {
        if (locale.toString(distMeters * meterInMiles, format, precision) ==
            locale.toString(0.0, format, precision))
        {
            return locale.toString(distMeters * meterInFeet, format, precision) +
                   QLatin1String(" ft");
        }
        else
        {
            return locale.toString(distMeters * meterInMiles, format,
                                   precision) + QLatin1String(" mi");
        }
    }
    else
    {
        if (distMeters >= 1000.0)
        {
            return locale.toString(distMeters / 1000.0, format, precision) +
                   QLatin1String(" km");
        }
        else
        {
            QString one    = locale.toString(1.0, format, precision);
            QString result = locale.toString(distMeters, format, precision);

            return (result + QLatin1Char(' ') + ((result == one) ? i18n("meter")
                                                                 : i18n("meters")));
        }
    }
}

QString localizedTimeDifference(quint64 diffSeconds)
{
    QString result;

    quint64 hours = diffSeconds / 3600;

    if (hours)
    {
        diffSeconds %= 3600;
        result       = QString::number(hours) + QLatin1Char(' ') +
                       (hours == 1 ? i18n("hour") : i18n("hours"));
    }

    quint64 minutes = diffSeconds / 60;

    if (minutes)
    {
        diffSeconds %= 60;

        if (!result.isEmpty())
        {
            result += QLatin1String(", ");
        }

        result += QString::number(minutes) + QLatin1Char(' ') +
                  (minutes == 1 ? i18n("minute") : i18n("minutes"));
    }

    if (diffSeconds || result.isEmpty())
    {
        if (!result.isEmpty())
        {
            result += QLatin1String(", ");
        }

        result += QString::number(diffSeconds) + QLatin1Char(' ') +
                  (diffSeconds == 1 ? i18n("second") : i18n("seconds"));
    }

    return result;
}

} // namespace DigikamGenericINatPlugin
