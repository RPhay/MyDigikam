/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings View.
 *
 * SPDX-FileCopyrightText: 2009      by Bastian Holst <bastianholst at gmx dot de>
 * SPDX-FileCopyrightText: 2012      by Mohammed Nafees <nafees dot technocool at gmail dot com>
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#include "MarbleConfigView.h"

// Qt includes

#include <QApplication>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QTimer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "MarbleDirs.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "RenderPluginModel.h"
#include "MarbleClock.h"
#include "MarblePluginSettingsWidget.h"
#include "ui_MarbleViewSettingsWidget.h"
#include "geolocationsettings.h"

#include "digikam_debug.h"

using namespace Digikam;

namespace Marble
{

class Q_DECL_HIDDEN MarbleConfigView::Private
{

public:

    explicit Private(MarbleWidget* const marbleWidget)
        : viewSettings(),
          marbleWidget(marbleWidget),
          pluginModel ()
    {
    }

    Ui::MarbleViewSettingsWidget       viewSettings;
    MarblePluginSettingsWidget*        pluginSettings = nullptr;

    MarbleWidget* const                marbleWidget;

    RenderPluginModel                  pluginModel;
};

MarbleConfigView::MarbleConfigView(MarbleWidget* const marbleWidget,
                                   QWidget* const parent)
    : QTabWidget(parent),
      d         (new Private(marbleWidget))
{
    // View page

    QWidget* const page = new QWidget(this);
    d->viewSettings.setupUi(page);
    addTab(page, i18n("Marble View"));

    connect(d->viewSettings.button_clearVolatileCache, SIGNAL(clicked()),
            this, SIGNAL(clearVolatileCacheClicked()));

    connect(d->viewSettings.button_clearPersistentCache, SIGNAL(clicked()),
            this, SIGNAL(clearPersistentCacheClicked()));

    // Plugins page

    if (d->marbleWidget)
    {
        d->pluginModel.setRenderPlugins(d->marbleWidget->renderPlugins());
    }

    d->pluginSettings = new MarblePluginSettingsWidget(this);
    d->pluginSettings->setModel(&d->pluginModel);
    d->pluginSettings->setObjectName(QLatin1String("plugin_page"));
    addTab(d->pluginSettings, i18n("Marble Plugins"));

    // Setting the icons for the plugin view.

    d->pluginSettings->setConfigIcon(QIcon::fromTheme(QLatin1String("configure")));
    d->pluginSettings->setAboutIcon(QIcon::fromTheme(QLatin1String("help-about")));
}

MarbleConfigView::~MarbleConfigView()
{
    delete d;
}

void MarbleConfigView::cancel()
{
    d->pluginModel.retrievePluginState();
}

void MarbleConfigView::readSettings()
{
    // Read settings from config file.

    GeolocationSettingsContainer settings = GeolocationSettings::instance()->settings();

    // View

    d->viewSettings.kcfg_distanceUnit->setCurrentIndex(settings.distanceUnit);
    d->viewSettings.kcfg_angleUnit->setCurrentIndex(settings.angleUnit);
    d->viewSettings.kcfg_stillQuality->setCurrentIndex(settings.stillQuality);
    d->viewSettings.kcfg_animationQuality->setCurrentIndex(settings.animationQuality);
    d->viewSettings.kcfg_mapFont->setCurrentFont(settings.mapFont);
    d->viewSettings.kcfg_inertialEarthRotation->setChecked(settings.inertialRotation);
    d->viewSettings.kcfg_mouseViewRotation->setChecked(settings.mouseRotation);
    d->viewSettings.kcfg_volatileTileCacheLimit->setValue(settings.volatileTileCacheLimit);
    d->viewSettings.kcfg_persistentTileCacheLimit->setValue(settings.persistentTileCacheLimit);

    // Read the settings of the plugins

    if (d->marbleWidget)
    {
        d->marbleWidget->readPluginSettings();
    }
}

void MarbleConfigView::applySettings()
{
    d->pluginModel.applyPluginState();

    GeolocationSettingsContainer settings;

    settings.distanceUnit             = (Marble::MarbleLocale::MeasurementSystem)d->viewSettings.kcfg_distanceUnit->currentIndex();
    settings.angleUnit                = (Marble::AngleUnit)d->viewSettings.kcfg_angleUnit->currentIndex();
    settings.stillQuality             = (Marble::MapQuality)d->viewSettings.kcfg_stillQuality->currentIndex();
    settings.animationQuality         = (Marble::MapQuality)d->viewSettings.kcfg_animationQuality->currentIndex();
    settings.mapFont                  = d->viewSettings.kcfg_mapFont->currentFont();
    settings.inertialRotation         = d->viewSettings.kcfg_inertialEarthRotation->isChecked();
    settings.mouseRotation            = d->viewSettings.kcfg_mouseViewRotation->isChecked();
    settings.volatileTileCacheLimit   = d->viewSettings.kcfg_volatileTileCacheLimit->value();
    settings.persistentTileCacheLimit = d->viewSettings.kcfg_persistentTileCacheLimit->value();

    GeolocationSettings::instance()->setSettings(settings);

    // Plugins

    if (d->marbleWidget)
    {
        d->marbleWidget->writePluginSettings();
    }
}

} // namespace Marble

#include "moc_MarbleConfigView.cpp"
