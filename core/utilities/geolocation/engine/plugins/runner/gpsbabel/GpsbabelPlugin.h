// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>

#pragma once

#include "ParseRunnerPlugin.h"

namespace Marble
{

class GpsbabelPlugin : public ParseRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.GpsbabelPlugin")
    Q_INTERFACES( Marble::ParseRunnerPlugin )

public:

    explicit GpsbabelPlugin(QObject* const parent = nullptr);

    QString name() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    QString fileFormatDescription() const override;

    QStringList fileExtensions() const override;

    ParsingRunner* newRunner() const override;
};

} // namespace Marble
