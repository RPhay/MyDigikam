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
#include "backendgooglemaps.h"
#include "backendmarble.h"
#include "mapwidget.h"
#include "marblesettings.h"
#include "MarbleGlobal.h"
#include "MarbleWidget.h"
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

    QVector<MapWidget*>     widgets;

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

void MarbleSettings::registerWidget(MapWidget* const widget)
{
    d->widgets << widget;
}

void MarbleSettings::applySettingsToWidgets(const MarbleSettingsContainer& settings)
{
    Q_FOREACH (MapWidget* const w, d->widgets)
    {
        if (w)
        {
            Q_FOREACH (MapBackend* const b, w->backends())
            {
                BackendMarble* const mb = dynamic_cast<BackendMarble*>(b);

                if (mb)
                {
                    MarbleWidget* const mw = static_cast<MarbleWidget*>(mb->mapWidget());
                    qCDebug(DIGIKAM_MARBLE_LOG) << "Marble Widget:" << mb;

                    MarbleGlobal::getInstance()->locale()->setMeasurementSystem(settings.distanceUnit);
                    mw->model()->setPersistentTileCacheLimit(settings.persistentTileCacheLimit * 1024);
                    mw->setDefaultFont(settings.mapFont);
                    mw->setMapQualityForViewContext(settings.stillQuality,     Marble::Still);
                    mw->setMapQualityForViewContext(settings.animationQuality, Marble::Animation);
                    mw->setDefaultAngleUnit(settings.angleUnit);
                    mw->inputHandler()->setInertialEarthRotationEnabled(settings.inertialRotation);
                    mw->inputHandler()->setMouseViewRotationEnabled(settings.mouseRotation);
                    mw->setVolatileTileCacheLimit(settings.volatileTileCacheLimit * 1024);
                    mw->update();
                }
            }
        }
    }
}

MarbleWidget* MarbleSettings::mainMarbleWidget() const
{
    Q_FOREACH (MapWidget* const w, d->widgets)
    {
        if (w)
        {
            Q_FOREACH (MapBackend* const b, w->backends())
            {
                BackendMarble* const mb = dynamic_cast<BackendMarble*>(b);

                if (mb)
                {
                    return (static_cast<MarbleWidget*>(mb->mapWidget()));
                }
            }
        }
    }

    return nullptr;
}

void MarbleSettings::reloadGoogleMaps()
{
    Q_FOREACH (MapWidget* const w, d->widgets)
    {
        if (w)
        {
            Q_FOREACH (MapBackend* const b, w->backends())
            {
                BackendGoogleMaps* const gb = dynamic_cast<BackendGoogleMaps*>(b);

                if (gb)
                {
                    gb->reload();
                }
            }
        }
    }
}

void MarbleSettings::googleMapsApiKeyChanged()
{
    Q_FOREACH (MapWidget* const w, d->widgets)
    {
        if (w)
        {
            Q_FOREACH (MapBackend* const b, w->backends())
            {
                BackendGoogleMaps* const gb = dynamic_cast<BackendGoogleMaps*>(b);

                if (gb)
                {
                    gb->setApiKeyChanged();
                }
            }
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

void MarbleSettings::openSetupGeolocation(SetupGeolocation::GeolocationTab tab)
{
    Q_EMIT signalSetupGeolocation(tab);
}

} // namespace Digikam

#include "moc_marblesettings.cpp"
