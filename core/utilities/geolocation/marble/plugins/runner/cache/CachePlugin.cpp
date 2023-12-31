// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "CachePlugin.h"

#include <QIcon>

#include <klocalizedstring.h>

#include "CacheRunner.h"

namespace Marble
{

CachePlugin::CachePlugin(QObject* parent)
    : ParseRunnerPlugin(parent)
{
}

QIcon CachePlugin::icon() const
{
    return QIcon::fromTheme(QStringLiteral("media-memory"));
}

QString CachePlugin::name() const
{
    return i18n( "Cache File Parser" );
}

QString CachePlugin::nameId() const
{
    return QStringLiteral("Cache");
}

QString CachePlugin::version() const
{
    return QStringLiteral("1.0");
}

QString CachePlugin::description() const
{
    return i18n( "Create GeoDataDocument from Cache Files" );
}

QString CachePlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> CachePlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"));
}

QString CachePlugin::fileFormatDescription() const
{
    return i18n( "Marble Cache Files" );
}

QStringList CachePlugin::fileExtensions() const
{
    return QStringList(QStringLiteral("cache"));
}

ParsingRunner* CachePlugin::newRunner() const
{
    return new CacheRunner;
}

}

#include "moc_CachePlugin.cpp"
