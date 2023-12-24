// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

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
#include "marblesettings.h"

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

    connect(this, SIGNAL(rejected()),
            &d->pluginModel, SLOT(retrievePluginState()));

    connect(this, SIGNAL(accepted()),
            &d->pluginModel, SLOT(applyPluginState()));
}

MarbleConfigView::~MarbleConfigView()
{
    delete d;
}

void MarbleConfigView::readSettings()
{
    // Read settings from config file.

    MarbleSettingsContainer settings = MarbleSettings::instance()->settings();

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
    MarbleSettingsContainer settings;

    settings.distanceUnit             = (Marble::MarbleLocale::MeasurementSystem)d->viewSettings.kcfg_distanceUnit->currentIndex();
    settings.angleUnit                = (Marble::AngleUnit)d->viewSettings.kcfg_angleUnit->currentIndex();
    settings.stillQuality             = (Marble::MapQuality)d->viewSettings.kcfg_stillQuality->currentIndex();
    settings.animationQuality         = (Marble::MapQuality)d->viewSettings.kcfg_animationQuality->currentIndex();
    settings.mapFont                  = d->viewSettings.kcfg_mapFont->currentFont();
    settings.inertialRotation         = d->viewSettings.kcfg_inertialEarthRotation->isChecked();
    settings.mouseRotation            = d->viewSettings.kcfg_mouseViewRotation->isChecked();
    settings.volatileTileCacheLimit   = d->viewSettings.kcfg_volatileTileCacheLimit->value();
    settings.persistentTileCacheLimit = d->viewSettings.kcfg_persistentTileCacheLimit->value();

    MarbleSettings::instance()->setSettings(settings);

    // Plugins

    if (d->marbleWidget)
    {
        d->marbleWidget->writePluginSettings();
    }
}

} // namespace Marble

#include "moc_MarbleConfigView.cpp"
