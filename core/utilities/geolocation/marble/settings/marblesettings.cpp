/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings Container.
 *
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "marblesettings.h"

// Qt includes

#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QVector>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_debug.h"
#include "MarbleGlobal.h"
#include "MarbleModel.h"
#include "MarbleWidgetInputHandler.h"

namespace Digikam
{

class Q_DECL_HIDDEN MarbleSettings::Private
{
public:

    explicit Private ()
        : mutex      (),
          configGroup(QLatin1String("Marble Settings"))
    {
    }

    MarbleSettingsContainer settings;
    QMutex                  mutex;

    const QString           configGroup;

    QVector<MarbleWidget*>  widgets;

public:

    MarbleSettingsContainer readFromConfig() const;
    void                    writeToConfig()  const;
    MarbleSettingsContainer setSettings(const MarbleSettingsContainer& s);
};

MarbleSettingsContainer MarbleSettings::Private::readFromConfig() const
{
    MarbleSettingsContainer s;
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroup);
    s.readFromConfig(group);

    return s;
}

void MarbleSettings::Private::writeToConfig() const
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroup);
    settings.writeToConfig(group);
}

MarbleSettingsContainer MarbleSettings::Private::setSettings(const MarbleSettingsContainer& s)
{
    QMutexLocker lock(&mutex);
    MarbleSettingsContainer old;
    old      = settings;
    settings = s;

    return old;
}

// -----------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN MarbleSettingsCreator
{
public:

    MarbleSettings object;
};

Q_GLOBAL_STATIC(MarbleSettingsCreator, metaEngineSettingsCreator)

// -----------------------------------------------------------------------------------------------

MarbleSettings* MarbleSettings::instance()
{
    return &metaEngineSettingsCreator->object;
}

MarbleSettings::MarbleSettings()
    : d(new Private)
{
    readFromConfig();
    qRegisterMetaType<MarbleSettingsContainer>("MarbleSettingsContainer");
}

MarbleSettings::~MarbleSettings()
{
    delete d;
}

void MarbleSettings::registerWidget(MarbleWidget* const widget)
{
    d->widgets << widget;
}

void MarbleSettings::applySettingsToWidgets(const MarbleSettingsContainer& settings)
{
    Q_FOREACH (MarbleWidget* const w, d->widgets)
    {
        if (w)
        {
            qCDebug(DIGIKAM_MARBLE_LOG) << "Marble Widget:" << w;

            MarbleGlobal::getInstance()->locale()->setMeasurementSystem(settings.distanceUnit);
            w->model()->setPersistentTileCacheLimit(settings.persistentTileCacheLimit * 1024);
            w->setDefaultFont(settings.mapFont);
            w->setMapQualityForViewContext(settings.stillQuality,     Marble::Still);
            w->setMapQualityForViewContext(settings.animationQuality, Marble::Animation);
            w->setDefaultAngleUnit(settings.angleUnit);
            w->inputHandler()->setInertialEarthRotationEnabled(settings.inertialRotation);
            w->inputHandler()->setMouseViewRotationEnabled(settings.mouseRotation);
            w->setVolatileTileCacheLimit(settings.volatileTileCacheLimit * 1024);
            w->update();
        }
    }
}

MarbleSettingsContainer MarbleSettings::settings() const
{
    QMutexLocker lock(&d->mutex);
    MarbleSettingsContainer s(d->settings);

    return s;
}

void MarbleSettings::setSettings(const MarbleSettingsContainer& settings)
{
    MarbleSettingsContainer old = d->setSettings(settings);

    Q_EMIT signalMarbleSettingsChanged(settings, old);

    Q_EMIT signalSettingsChanged();

    applySettingsToWidgets(settings);
    d->writeToConfig();
}

void MarbleSettings::readFromConfig()
{
    MarbleSettingsContainer s   = d->readFromConfig();
    MarbleSettingsContainer old = d->setSettings(s);

    Q_EMIT signalMarbleSettingsChanged(s, old);

    Q_EMIT signalSettingsChanged();
}

} // namespace Digikam

#include "moc_marblesettings.cpp"
