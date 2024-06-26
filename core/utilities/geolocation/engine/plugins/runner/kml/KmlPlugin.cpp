// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>

#include "KmlPlugin.h"

#include <klocalizedstring.h>

#include "KmlRunner.h"

namespace Marble
{

KmlPlugin::KmlPlugin(QObject* const parent)
    : ParseRunnerPlugin( parent )
{
}

QString KmlPlugin::name() const
{
    return i18n( "KML File Parser" );
}

QString KmlPlugin::nameId() const
{
    return QStringLiteral("Kml");
}

QString KmlPlugin::version() const
{
    return QStringLiteral("1.2");
}

QString KmlPlugin::description() const
{
    return i18n( "A plugin to load Geolocation Data from the KML and KMZ data file format." );
}

QString KmlPlugin::copyrightYears() const
{
    return QStringLiteral("2011, 2013, 2015");
}

QVector<PluginAuthor> KmlPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"))
            << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"));
}

QString KmlPlugin::fileFormatDescription() const
{
    return i18n( "Google Earth KML" );
}

QStringList KmlPlugin::fileExtensions() const
{
    return QStringList() << QStringLiteral("kml")
                         << QStringLiteral("kmz");
}

ParsingRunner* KmlPlugin::newRunner() const
{
    return new KmlRunner;
}

} // namespace Marble

#include "moc_KmlPlugin.cpp"
