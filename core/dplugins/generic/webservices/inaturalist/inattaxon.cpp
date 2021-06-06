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

#include "inattaxon.h"

// Local includes

#include "inatutils.h"

namespace DigikamGenericINatPlugin
{

class Q_DECL_HIDDEN Taxon::Private
{
public:

    Private()
        : id       (-1),
          parentId (-1),
          rankLevel(-1.0)
    {
    }

    int          id;
    int          parentId;
    QString      name;
    QString      rank;
    double       rankLevel;
    QString      commonName;
    QString      matchedTerm;
    QUrl         squareUrl;
    QList<Taxon> ancestors;
};

Taxon::Taxon(int id,
             int parentId,
             const QString& name,
             const QString& rank,
             double rankLevel,
             const QString& commonName,
             const QString& matchedTerm,
             const QUrl& squareUrl,
             const QList<Taxon>& ancestors)
    : d(new Private)
{
    d->id          = id;
    d->parentId    = parentId;
    d->name        = name;
    d->rank        = rank;
    d->rankLevel   = rankLevel;
    d->commonName  = commonName;
    d->matchedTerm = matchedTerm;
    d->squareUrl   = squareUrl;
    d->ancestors   = ancestors;
}

Taxon::Taxon()
    : d(new Private)
{
}

Taxon::Taxon(const Taxon& other)
    : d(new Private)
{
    *d = *other.d;
}

Taxon::~Taxon()
{
    delete d;
}

Taxon& Taxon::operator=(const Taxon& other)
{
    *d = *other.d;

    return *this;
}

int Taxon::id() const
{
    return d->id;
}

int Taxon::parentId() const
{
    return d->parentId;
}

const QString& Taxon::name() const
{
    return d->name;
}

/**
 * return italicized scientific name
 */
QString Taxon::htmlName() const
{
    static const QString species    = QLatin1String("species");
    static const QString subspecies = QLatin1String("subspecies");
    static const QString variety    = QLatin1String("variety");
    static const QString hybrid     = QLatin1String("hybrid");

    QString result;

    if ((rank() != species) && (rank() != subspecies) && (rank() != variety))
    {
        result    = localizedTaxonomicRank(rank()) + QLatin1Char(' ');
        result[0] = result[0].toTitleCase();
    }

    if ((rank() == subspecies) || (rank() == variety) || (rank() == hybrid))
    {
        // The scientific name is italisized but ssp, var, x aren't.

        QStringList split = name().split(QLatin1Char(' '));

        if (split.count() == 3)
        {
            QString txt = (rank() == subspecies) ? QLatin1String(" ssp. ")
                                                 : (rank() == variety) ? QLatin1String(" var. ")
                                                                       : QLatin1String(" x ");
            result += QLatin1String("<i>") + split[0] + QLatin1Char(' ') +
                      split[1] + QLatin1String("</i>") + txt +
                      QLatin1String("<i>") + split[2] + QLatin1String("</i>");
        }
        else
        {
            Q_ASSERT(split.count() == 4);
            result += QLatin1String("<i>") + split[0] + QLatin1Char(' ') +
                      split[1] + QLatin1String("</i>") + QLatin1Char(' ') +
                      split[2] + QLatin1Char(' ') +
                      QLatin1String("<i>") + split[3] + QLatin1String("</i>");
        }
    }
    else
    {
        result += QLatin1String("<i>") + name() + QLatin1String("</i>");
    }

    return result;
}

const QString& Taxon::rank() const
{
    return d->rank;
}

double Taxon::rankLevel() const
{
    return d->rankLevel;
}

const QString& Taxon::matchedTerm() const
{
    return d->matchedTerm;
}

const QUrl& Taxon::squareUrl() const
{
    return d->squareUrl;
}

const QString& Taxon::commonName() const
{
    return d->commonName;
}

const QList<Taxon>& Taxon::ancestors() const
{
    return d->ancestors;
}

bool Taxon::isValid() const
{
    return (d->id != -1);
}

bool Taxon::operator==(const Taxon& other) const
{
    return (d->id == other.d->id);
}

bool Taxon::operator!=(const Taxon& other) const
{
    return (d->id != other.d->id);
}

} // namespace DigikamGenericINatPlugin
