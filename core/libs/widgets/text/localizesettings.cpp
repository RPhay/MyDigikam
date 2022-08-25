/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-16
 * Description : central place for Spell-check and localize settings
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "localizesettings.h"

// Qt includes

#include <QMutex>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN LocalizeSettings::Private
{
public:

    explicit Private ()
        : mutex      (),
          configGroup(QLatin1String("Spell Check Settings"))
    {
    }

    LocalizeContainer settings;
    QMutex              mutex;

    const QString       configGroup;

public:

    LocalizeContainer readFromConfig() const;
    void                writeToConfig()  const;
    void                setSettings(const LocalizeContainer& s);
};

LocalizeContainer LocalizeSettings::Private::readFromConfig() const
{
    LocalizeContainer s;
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroup);
    s.readFromConfig(group);

    return s;
}

void LocalizeSettings::Private::writeToConfig() const
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroup);
    settings.writeToConfig(group);
}

void LocalizeSettings::Private::setSettings(const LocalizeContainer& s)
{
    QMutexLocker lock(&mutex);
    settings = s;
}

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN LocalizeSettingsCreator
{
public:

    LocalizeSettings object;
};

Q_GLOBAL_STATIC(LocalizeSettingsCreator, speckCheckSettingsCreator)

// -----------------------------------------------------------------------------------------------

LocalizeSettings* LocalizeSettings::instance()
{
    return &speckCheckSettingsCreator->object;
}

LocalizeSettings::LocalizeSettings()
    : d(new Private)
{
    readFromConfig();
    qRegisterMetaType<LocalizeContainer>("LocalizeContainer");
}

LocalizeSettings::~LocalizeSettings()
{
    delete d;
}

void LocalizeSettings::openLocalizeSetup()
{
    Q_EMIT signalOpenLocalizeSetup();
}

LocalizeContainer LocalizeSettings::settings() const
{
    QMutexLocker lock(&d->mutex);
    LocalizeContainer s(d->settings);

    return s;
}

void LocalizeSettings::setSettings(const LocalizeContainer& settings)
{
    d->setSettings(settings);

    Q_EMIT signalSettingsChanged();

    d->writeToConfig();
}

void LocalizeSettings::readFromConfig()
{
    d->settings = d->readFromConfig();

    Q_EMIT signalSettingsChanged();
}

} // namespace Digikam

#include "moc_localizesettings.cpp"
