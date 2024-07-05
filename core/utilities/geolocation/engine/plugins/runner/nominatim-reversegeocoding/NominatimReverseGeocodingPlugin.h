/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

#include "ReverseGeocodingRunnerPlugin.h"

namespace Marble
{

class NominatimPlugin : public ReverseGeocodingRunnerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.NominatimReverseGeocodingPlugin")
    Q_INTERFACES( Marble::ReverseGeocodingRunnerPlugin )

public:

    explicit NominatimPlugin(QObject * const parent = nullptr);

    QString name() const override;

    QString guiString() const override;

    QString nameId() const override;

    QString version() const override;

    QString description() const override;

    QString copyrightYears() const override;

    QVector<PluginAuthor> pluginAuthors() const override;

    ReverseGeocodingRunner* newRunner() const override;
};

} // namespace Marble
