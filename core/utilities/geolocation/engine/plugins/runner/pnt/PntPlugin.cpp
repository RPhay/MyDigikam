// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>

#include "PntPlugin.h"

#include <klocalizedstring.h>

#include "PntRunner.h"

namespace Marble
{

PntPlugin::PntPlugin(QObject * const parent)
    : ParseRunnerPlugin(parent)
{
}

QString PntPlugin::name() const
{
    return i18n( "Pnt File Parser" );
}

QString PntPlugin::nameId() const
{
    return QStringLiteral("Pnt");
}

QString PntPlugin::version() const
{
    return QStringLiteral("1.0");
}

QString PntPlugin::description() const
{
    return i18n( "A plugin to load Geolocation Data from the Micro World Database II Files format" );
}

QString PntPlugin::copyrightYears() const
{
    return QStringLiteral("2011");
}

QVector<PluginAuthor> PntPlugin::pluginAuthors() const
{
    return QVector<PluginAuthor>()
            << PluginAuthor(QStringLiteral("Thibaut Gridel"), QStringLiteral("tgridel@free.fr"));
}

QString PntPlugin::fileFormatDescription() const
{
    return i18n( "Micro World Database II" );
}

QStringList PntPlugin::fileExtensions() const
{
    return QStringList(QStringLiteral("pnt"));
}

ParsingRunner* PntPlugin::newRunner() const
{
    return new PntRunner;
}

} // namespace Marble

#include "moc_PntPlugin.cpp"
