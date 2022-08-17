/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-16
 * Description : central place for Spell-check settings
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

#include "spellchecksettings.h"

// Qt includes

#include <QMutex>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN SpellCheckSettings::Private
{
public:

    explicit Private ()
        : mutex      (),
          configGroup(QLatin1String("Spell Check Settings"))
    {
    }

    SpellCheckContainer settings;
    QMutex              mutex;

    const QString       configGroup;

public:

    SpellCheckContainer readFromConfig() const;
    void                writeToConfig()  const;
    void                setSettings(const SpellCheckContainer& s);
};

SpellCheckContainer SpellCheckSettings::Private::readFromConfig() const
{
    SpellCheckContainer s;
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroup);
    s.readFromConfig(group);

    return s;
}

void SpellCheckSettings::Private::writeToConfig() const
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroup);
    settings.writeToConfig(group);
}

void SpellCheckSettings::Private::setSettings(const SpellCheckContainer& s)
{
    QMutexLocker lock(&mutex);
    settings = s;
}

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN SpellCheckSettingsCreator
{
public:

    SpellCheckSettings object;
};

Q_GLOBAL_STATIC(SpellCheckSettingsCreator, speckCheckSettingsCreator)

// -----------------------------------------------------------------------------------------------

SpellCheckSettings* SpellCheckSettings::instance()
{
    return &speckCheckSettingsCreator->object;
}

SpellCheckSettings::SpellCheckSettings()
    : d(new Private)
{
    readFromConfig();
    qRegisterMetaType<SpellCheckContainer>("SpellCheckContainer");
}

SpellCheckSettings::~SpellCheckSettings()
{
    delete d;
}

SpellCheckContainer SpellCheckSettings::settings() const
{
    QMutexLocker lock(&d->mutex);
    SpellCheckContainer s(d->settings);

    return s;
}

void SpellCheckSettings::setSettings(const SpellCheckContainer& settings)
{
    d->setSettings(settings);

    Q_EMIT signalSettingsChanged();

    d->writeToConfig();
}

void SpellCheckSettings::readFromConfig()
{
    d->settings = d->readFromConfig();

    Q_EMIT signalSettingsChanged();
}

} // namespace Digikam

#include "moc_spellchecksettings.cpp"
