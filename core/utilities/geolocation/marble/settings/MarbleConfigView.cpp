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
        : ui_viewSettings (),
          m_marbleWidget  (marbleWidget),
          m_pluginModel   ()
    {
    }

    Ui::MarbleViewSettingsWidget       ui_viewSettings;
    MarblePluginSettingsWidget*        w_pluginSettings = nullptr;

    MarbleWidget* const                m_marbleWidget;

    RenderPluginModel                  m_pluginModel;
};

MarbleConfigView::MarbleConfigView(MarbleWidget* const marbleWidget,
                                   QWidget* const parent)
    : QTabWidget(parent),
      d      (new Private(marbleWidget))
{
    // View page

    QWidget* const w_viewSettings = new QWidget( this );

    d->ui_viewSettings.setupUi( w_viewSettings );
    addTab( w_viewSettings, i18n( "View" ) );

    connect( d->ui_viewSettings.button_clearVolatileCache,
             SIGNAL(clicked()), SIGNAL(clearVolatileCacheClicked()) );

    connect( d->ui_viewSettings.button_clearPersistentCache,
             SIGNAL(clicked()), SIGNAL(clearPersistentCacheClicked()) );

    // Plugins page

    d->m_pluginModel.setRenderPlugins( d->m_marbleWidget->renderPlugins() );
    d->w_pluginSettings = new MarblePluginSettingsWidget( this );
    d->w_pluginSettings->setModel( &d->m_pluginModel );
    d->w_pluginSettings->setObjectName( QLatin1String("plugin_page") );
    addTab( d->w_pluginSettings, i18n( "Plugins" ) );

    // Setting the icons for the plugin dialog.

    d->w_pluginSettings->setConfigIcon(QIcon::fromTheme(QLatin1String("configure")));

    connect( this, SIGNAL(rejected()),
             &d->m_pluginModel, SLOT(retrievePluginState()) );

    connect( this, SIGNAL(accepted()),
             &d->m_pluginModel, SLOT(applyPluginState()) );

    // When the settings have been changed, write to disk.

    connect( this, SIGNAL(settingsChanged()),
             this, SLOT(syncSettings()) );
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

    d->ui_viewSettings.kcfg_distanceUnit->setCurrentIndex(settings.distanceUnit);
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex(settings.angleUnit);
    d->ui_viewSettings.kcfg_stillQuality->setCurrentIndex(settings.stillQuality);
    d->ui_viewSettings.kcfg_animationQuality->setCurrentIndex(settings.animationQuality);
    d->ui_viewSettings.kcfg_mapFont->setCurrentFont(settings.mapFont);
    d->ui_viewSettings.kcfg_inertialEarthRotation->setChecked(settings.inertialRotation);
    d->ui_viewSettings.kcfg_mouseViewRotation->setChecked(settings.mouseRotation);
    d->ui_viewSettings.kcfg_volatileTileCacheLimit->setValue(settings.volatileTileCacheLimit);
    d->ui_viewSettings.kcfg_persistentTileCacheLimit->setValue(settings.persistentTileCacheLimit);

    // Read the settings of the plugins

    if (d->m_marbleWidget)
    {
        d->m_marbleWidget->readPluginSettings();
    }
}

void MarbleConfigView::applySettings()
{
    MarbleSettingsContainer settings;

    settings.distanceUnit             = (Marble::MarbleLocale::MeasurementSystem)d->ui_viewSettings.kcfg_distanceUnit->currentIndex();
    settings.angleUnit                = (Marble::AngleUnit)d->ui_viewSettings.kcfg_angleUnit->currentIndex();
    settings.stillQuality             = (Marble::MapQuality)d->ui_viewSettings.kcfg_stillQuality->currentIndex();
    settings.animationQuality         = (Marble::MapQuality)d->ui_viewSettings.kcfg_animationQuality->currentIndex();
    settings.mapFont                  = d->ui_viewSettings.kcfg_mapFont->currentFont();
    settings.inertialRotation         = d->ui_viewSettings.kcfg_inertialEarthRotation->isChecked();
    settings.mouseRotation            = d->ui_viewSettings.kcfg_mouseViewRotation->isChecked();
    settings.volatileTileCacheLimit   = d->ui_viewSettings.kcfg_volatileTileCacheLimit->value();
    settings.persistentTileCacheLimit = d->ui_viewSettings.kcfg_persistentTileCacheLimit->value();

    MarbleSettings::instance()->setSettings(settings);

    // Plugins

    if (d->m_marbleWidget)
    {
        d->m_marbleWidget->writePluginSettings();
    }
}

} // namespace Marble

#include "moc_MarbleConfigView.cpp"
