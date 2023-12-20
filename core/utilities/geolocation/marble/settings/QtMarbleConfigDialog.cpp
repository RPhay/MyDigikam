// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

// Own
#include "QtMarbleConfigDialog.h"

#include "ui_MarbleCacheSettingsWidget.h"
#include "ui_MarbleViewSettingsWidget.h"
#include "ui_MarbleNavigationSettingsWidget.h"
#include "ui_MarbleTimeSettingsWidget.h"
#include "ui_MarbleCloudSyncSettingsWidget.h"

// Qt
#include <QSettings>
#include <QNetworkProxy>
#include <QApplication>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QTimer>

#include <klocalizedstring.h>

// Marble
#include "MarbleDirs.h"
#include "MarblePluginSettingsWidget.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "RenderPlugin.h"
#include "RenderPluginModel.h"
#include "MarbleClock.h"
#include "BookmarkSyncManager.h"
#include "CloudSyncManager.h"

#include "digikam_debug.h"

namespace Marble
{

class QtMarbleConfigDialogPrivate
{
 public:
    QtMarbleConfigDialogPrivate( MarbleWidget *marbleWidget, CloudSyncManager *cloudSyncManager )
        : ui_viewSettings(),
          ui_navigationSettings(),
          ui_timeSettings(),
          ui_cacheSettings(),
          w_pluginSettings( nullptr ),
          m_cloudSyncStatusLabel( nullptr ),
          m_marbleWidget( marbleWidget ),
          m_syncManager( cloudSyncManager ? cloudSyncManager->bookmarkSyncManager() : nullptr ),
          m_cloudSyncManager(cloudSyncManager),
          m_pluginModel()
    {
    }

    Ui::MarbleViewSettingsWidget       ui_viewSettings;
    Ui::MarbleNavigationSettingsWidget ui_navigationSettings;
    Ui::MarbleTimeSettingsWidget       ui_timeSettings;
    Ui::MarbleCacheSettingsWidget      ui_cacheSettings;
    Ui::MarbleCloudSyncSettingsWidget  ui_cloudSyncSettings;
    MarblePluginSettingsWidget         *w_pluginSettings;

    QSettings m_settings;

    QLabel *m_cloudSyncStatusLabel;

    MarbleWidget *const m_marbleWidget;
    BookmarkSyncManager *const m_syncManager;
    CloudSyncManager *const m_cloudSyncManager;

    RenderPluginModel m_pluginModel;

    QHash< int, int > m_timezone;
};

QtMarbleConfigDialog::QtMarbleConfigDialog(MarbleWidget *marbleWidget, CloudSyncManager *cloudSyncManager,
                                           QWidget *parent )
    : QDialog( parent ),
      d( new QtMarbleConfigDialogPrivate( marbleWidget, cloudSyncManager ) )
{
    QTabWidget *tabWidget = new QTabWidget( this );
    QDialogButtonBox *buttons =
    new QDialogButtonBox( QDialogButtonBox::Ok
                        | QDialogButtonBox::Apply
                        | QDialogButtonBox::Cancel,
                          Qt::Horizontal,
                          this );

    // Connect the signals of the ButtonBox
    // to the corresponding slots of the dialog.
    connect( buttons, SIGNAL(accepted()), this, SLOT(accept()) ); // Ok
    connect( buttons, SIGNAL(rejected()), this, SLOT(reject()) ); // Cancel
    connect( buttons->button( QDialogButtonBox::Apply ),SIGNAL(clicked()),
             this, SLOT(writeSettings()) );                         // Apply
    // If the dialog is accepted. Save the settings.
    connect( this, SIGNAL(accepted()), this, SLOT(writeSettings()) );

    // view page
    QWidget *w_viewSettings = new QWidget( this );

    d->ui_viewSettings.setupUi( w_viewSettings );
    tabWidget->addTab( w_viewSettings, i18n( "View" ) );

    d->ui_viewSettings.kcfg_labelLocalization->hide();
    d->ui_viewSettings.label_labelLocalization->hide();

    // navigation page
    QWidget *w_navigationSettings = new QWidget( this );

    d->ui_navigationSettings.setupUi( w_navigationSettings );
    tabWidget->addTab( w_navigationSettings, i18n( "Navigation" ) );
    d->ui_navigationSettings.kcfg_dragLocation->hide();
    d->ui_navigationSettings.label_dragLocation->hide();

    // cache page
    QWidget *w_cacheSettings = new QWidget( this );

    d->ui_cacheSettings.setupUi( w_cacheSettings );
    tabWidget->addTab( w_cacheSettings, i18n( "Cache and Proxy" ) );
    // Forwarding clear button signals
    connect( d->ui_cacheSettings.button_clearVolatileCache, SIGNAL(clicked()), SIGNAL(clearVolatileCacheClicked()) );
    connect( d->ui_cacheSettings.button_clearPersistentCache, SIGNAL(clicked()), SIGNAL(clearPersistentCacheClicked()) );

    // time page
    QWidget *w_timeSettings = new QWidget( this );
    d->ui_timeSettings.setupUi( w_timeSettings );
    tabWidget->addTab( w_timeSettings, i18n( "Date and Time" ) );

    // plugin page
    d->m_pluginModel.setRenderPlugins( d->m_marbleWidget->renderPlugins() );
    d->w_pluginSettings = new MarblePluginSettingsWidget( this );
    d->w_pluginSettings->setModel( &d->m_pluginModel );
    d->w_pluginSettings->setObjectName( QLatin1String("plugin_page") );
    tabWidget->addTab( d->w_pluginSettings, i18n( "Plugins" ) );

    // Setting the icons for the plugin dialog.
    d->w_pluginSettings->setConfigIcon(QIcon(QStringLiteral(":/icons/settings-configure.png")));

    connect( this, SIGNAL(rejected()), &d->m_pluginModel, SLOT(retrievePluginState()) );
    connect( this, SIGNAL(accepted()), &d->m_pluginModel, SLOT(applyPluginState()) );

    QWidget *w_cloudSyncSettings = new QWidget( this );
    d->ui_cloudSyncSettings.setupUi( w_cloudSyncSettings );
    tabWidget->addTab( w_cloudSyncSettings, i18n( "Synchronization" ) );
    d->ui_cloudSyncSettings.button_syncNow->setEnabled( syncBookmarks() );
    d->m_cloudSyncStatusLabel = d->ui_cloudSyncSettings.cloudSyncStatus;
    connect( d->ui_cloudSyncSettings.button_syncNow, SIGNAL(clicked()), SIGNAL(syncNowClicked()) );
    connect( d->ui_cloudSyncSettings.testLoginButton, SIGNAL(clicked()), this, SLOT(updateCloudSyncCredentials()) );

    if ( d->m_syncManager ) {
        connect(d->m_syncManager, SIGNAL(syncComplete()), this, SLOT(updateLastSync()));
        updateLastSync();
    }
    if ( d->m_cloudSyncManager ) {
        connect( d->m_cloudSyncManager, SIGNAL(statusChanged(QString)),
                 this, SLOT(updateCloudSyncStatus(QString)));
    }

    // Layout
    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->addWidget( tabWidget );
    layout->addWidget( buttons );

    this->setLayout( layout );

    // When the settings have been changed, write to disk.
    connect( this, SIGNAL(settingsChanged()), this, SLOT(syncSettings()) );

    initializeCustomTimezone();
}

QtMarbleConfigDialog::~QtMarbleConfigDialog()
{
    delete d;
}

void QtMarbleConfigDialog::syncSettings()
{
    d->m_settings.sync();

    QNetworkProxy proxy;

    // Make sure that no proxy is used for an empty string or the default value:
    if (proxyUrl().isEmpty() || proxyUrl() == QLatin1String("http://")) {
        proxy.setType( QNetworkProxy::NoProxy );
    } else {
        if ( proxyType() == Marble::Socks5Proxy ) {
            proxy.setType( QNetworkProxy::Socks5Proxy );
        }
        else if ( proxyType() == Marble::HttpProxy ) {
            proxy.setType( QNetworkProxy::HttpProxy );
        }
        else {
            qCDebug(DIGIKAM_MARBLE_LOG) << "Unknown proxy type! Using Http Proxy instead.";
            proxy.setType( QNetworkProxy::HttpProxy );
        }
    }

    proxy.setHostName( proxyUrl() );
    proxy.setPort( proxyPort() );

    if ( proxyAuth() ) {
        proxy.setUser( proxyUser() );
        proxy.setPassword( proxyPass() );
    }

    QNetworkProxy::setApplicationProxy(proxy);
}

void QtMarbleConfigDialog::updateCloudSyncCredentials()
{
    if ( d->m_cloudSyncManager ) {
        d->m_cloudSyncManager->setOwncloudCredentials(
                    d->ui_cloudSyncSettings.kcfg_owncloudServer->text(),
                    d->ui_cloudSyncSettings.kcfg_owncloudUsername->text(),
                    d->ui_cloudSyncSettings.kcfg_owncloudPassword->text() );
    }
}

void QtMarbleConfigDialog::disableSyncNow()
{
    if ( !d->m_syncManager ) {
        return;
    }

    d->ui_cloudSyncSettings.button_syncNow->setDisabled(true);

    QTimer *timeoutTimer = new QTimer(this);
    connect(timeoutTimer, SIGNAL(timeout()),
            timeoutTimer, SLOT(stop()));
    connect(timeoutTimer, SIGNAL(timeout()),
            this, SLOT(enableSyncNow()));

    connect(d->m_syncManager, SIGNAL(syncComplete()),
            this, SLOT(enableSyncNow()));
    connect(d->m_syncManager, SIGNAL(syncComplete()),
            timeoutTimer, SLOT(stop()));
    connect(d->m_syncManager, SIGNAL(syncComplete()),
            timeoutTimer, SLOT(deleteLater()));

    timeoutTimer->start(30*1000); // 30 sec
}

void QtMarbleConfigDialog::enableSyncNow()
{
    if ( !d->m_syncManager ) {
        return;
    }

    d->ui_cloudSyncSettings.button_syncNow->setEnabled(true);
}

void QtMarbleConfigDialog::updateLastSync()
{
    if ( !d->m_syncManager ) {
        return;
    }

    if (!d->m_syncManager->lastSync().isValid()) {
        d->ui_cloudSyncSettings.labelLastSync->setText(i18n("Never synchronized."));
        return;
    }

    const QString title = i18n("Last synchronization: %1")
            .arg(d->m_syncManager->lastSync().toString());

    d->ui_cloudSyncSettings.labelLastSync->setText(title);
}

void QtMarbleConfigDialog::readSettings()
{
    // Sync settings to make sure that we read the current settings.
    syncSettings();

    // View
    d->ui_viewSettings.kcfg_distanceUnit->setCurrentIndex( measurementSystem() );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
    d->ui_viewSettings.kcfg_stillQuality->setCurrentIndex( stillQuality() );
    d->ui_viewSettings.kcfg_animationQuality->setCurrentIndex( animationQuality() );
    d->ui_viewSettings.kcfg_labelLocalization->setCurrentIndex( Marble::Native );
    d->ui_viewSettings.kcfg_mapFont->setCurrentFont( mapFont() );

    // Navigation
    d->ui_navigationSettings.kcfg_dragLocation->setCurrentIndex( Marble::KeepAxisVertically );
    d->ui_navigationSettings.kcfg_onStartup->setCurrentIndex( onStartup() );
    d->ui_navigationSettings.kcfg_inertialEarthRotation->setChecked( inertialEarthRotation() );
    d->ui_navigationSettings.kcfg_mouseViewRotation->setChecked( mouseViewRotation() );
    d->ui_navigationSettings.kcfg_animateTargetVoyage->setChecked( animateTargetVoyage() );
    int editorIndex = 0;
    if (externalMapEditor() == QLatin1String("potlatch")) {
        editorIndex = 1;
    } else if (externalMapEditor() == QLatin1String("josm")) {
        editorIndex = 2;
    } else if (externalMapEditor() == QLatin1String("merkaartor")) {
        editorIndex = 3;
    }
    d->ui_navigationSettings.kcfg_externalMapEditor->setCurrentIndex( editorIndex );

    // Cache
    d->ui_cacheSettings.kcfg_volatileTileCacheLimit->setValue( volatileTileCacheLimit() );
    d->ui_cacheSettings.kcfg_persistentTileCacheLimit->setValue( persistentTileCacheLimit() );
    d->ui_cacheSettings.kcfg_proxyUrl->setText( proxyUrl() );
    d->ui_cacheSettings.kcfg_proxyPort->setValue( proxyPort() );
    d->ui_cacheSettings.kcfg_proxyUser->setText( proxyUser() );
    d->ui_cacheSettings.kcfg_proxyPass->setText( proxyPass() );
    d->ui_cacheSettings.kcfg_proxyType->setCurrentIndex( proxyType() );
    d->ui_cacheSettings.kcfg_proxyAuth->setChecked( proxyAuth() );

    // Time
    d->ui_timeSettings.kcfg_systemTimezone->setChecked( systemTimezone() );
    d->ui_timeSettings.kcfg_customTimezone->setChecked( customTimezone() );
    d->ui_timeSettings.kcfg_chosenTimezone->setCurrentIndex( chosenTimezone() );
    d->ui_timeSettings.kcfg_utc->setChecked( UTC() );
    d->ui_timeSettings.kcfg_systemTime->setChecked( systemTime() );
    d->ui_timeSettings.kcfg_lastSessionTime->setChecked( lastSessionTime() );
    if( systemTimezone() == true  )
    {
        QDateTime localTime = QDateTime::currentDateTime().toLocalTime();
        localTime.setTimeSpec( Qt::UTC );
        d->m_marbleWidget->model()->setClockTimezone( QDateTime::currentDateTime().toUTC().secsTo( localTime ) );
    }
    else if( UTC() == true )
    {
        d->m_marbleWidget->model()->setClockTimezone( 0 );
    }
    else if( customTimezone() == true )
    {
        d->m_marbleWidget->model()->setClockTimezone( d->m_timezone.value( chosenTimezone() ) );
    }

    // ownCloud
    d->ui_cloudSyncSettings.kcfg_enableSync->setChecked( syncEnabled() );
    d->ui_cloudSyncSettings.kcfg_syncBookmarks->setChecked( syncBookmarks() );
    d->ui_cloudSyncSettings.kcfg_syncRoutes->setChecked( syncRoutes() );
    d->ui_cloudSyncSettings.kcfg_owncloudServer->setText( owncloudServer() );
    d->ui_cloudSyncSettings.kcfg_owncloudUsername->setText( owncloudUsername() );
    d->ui_cloudSyncSettings.kcfg_owncloudPassword->setText( owncloudPassword() );

    // Read the settings of the plugins
    d->m_marbleWidget->readPluginSettings( d->m_settings );

    // The settings loaded in the config dialog have been changed.
    Q_EMIT settingsChanged();
}

void QtMarbleConfigDialog::updateCloudSyncStatus( const QString &status )
{
    d->m_cloudSyncStatusLabel->setText(status);
    CloudSyncManager::Status status_type =
            d->m_cloudSyncManager ? d->m_cloudSyncManager->status() : CloudSyncManager::Unknown;
    switch (status_type) {
        case CloudSyncManager::Success:
            d->m_cloudSyncStatusLabel->setStyleSheet(QString::fromUtf8("QLabel { color : green; }"));
            break;
        case CloudSyncManager::Error:
            d->m_cloudSyncStatusLabel->setStyleSheet(QString::fromUtf8("QLabel { color : red; }"));
            break;
        case CloudSyncManager::Unknown:
            d->m_cloudSyncStatusLabel->setStyleSheet(QString::fromUtf8("QLabel { color : grey; }"));
            break;
    }
}

void QtMarbleConfigDialog::writeSettings()
{
    syncSettings();

    d->m_settings.beginGroup( QLatin1String("View") );
    d->m_settings.setValue( QLatin1String("distanceUnit"), d->ui_viewSettings.kcfg_distanceUnit->currentIndex() );
    d->m_settings.setValue( QLatin1String("angleUnit"), d->ui_viewSettings.kcfg_angleUnit->currentIndex() );
    d->m_settings.setValue( QLatin1String("stillQuality"), d->ui_viewSettings.kcfg_stillQuality->currentIndex() );
    d->m_settings.setValue( QLatin1String("animationQuality"), d->ui_viewSettings.kcfg_animationQuality->currentIndex() );
    d->m_settings.setValue( QLatin1String("mapFont"), d->ui_viewSettings.kcfg_mapFont->currentFont() );
    d->m_settings.endGroup();

    d->m_settings.beginGroup( QLatin1String("Navigation") );
    d->m_settings.setValue( QLatin1String("onStartup"), d->ui_navigationSettings.kcfg_onStartup->currentIndex() );
    d->m_settings.setValue( QLatin1String("inertialEarthRotation"), d->ui_navigationSettings.kcfg_inertialEarthRotation->isChecked() );
    d->m_settings.setValue( QLatin1String("mouseViewRotation"), d->ui_navigationSettings.kcfg_mouseViewRotation->isChecked() );
    d->m_settings.setValue( QLatin1String("animateTargetVoyage"), d->ui_navigationSettings.kcfg_animateTargetVoyage->isChecked() );
    if( d->ui_navigationSettings.kcfg_externalMapEditor->currentIndex() == 0 ) {
        d->m_settings.setValue( QLatin1String("externalMapEditor"), QString::fromUtf8("") );
    } else if( d->ui_navigationSettings.kcfg_externalMapEditor->currentIndex() == 1 ) {
        d->m_settings.setValue( QLatin1String("externalMapEditor"), QString::fromUtf8("potlatch") );
    } else if( d->ui_navigationSettings.kcfg_externalMapEditor->currentIndex() == 2 ) {
        d->m_settings.setValue( QLatin1String("externalMapEditor"), QString::fromUtf8("josm") );
    } else if( d->ui_navigationSettings.kcfg_externalMapEditor->currentIndex() == 3 ) {
        d->m_settings.setValue( QLatin1String("externalMapEditor"), QString::fromUtf8("merkaartor") );
    } else {
        Q_ASSERT( false && "Unexpected index of the external editor setting" );
    }
    d->m_settings.endGroup();

    d->m_settings.beginGroup( QLatin1String("Cache") );
    d->m_settings.setValue( QLatin1String("volatileTileCacheLimit"), d->ui_cacheSettings.kcfg_volatileTileCacheLimit->value() );
    d->m_settings.setValue( QLatin1String("persistentTileCacheLimit"), d->ui_cacheSettings.kcfg_persistentTileCacheLimit->value() );
    d->m_settings.setValue( QLatin1String("proxyUrl"), d->ui_cacheSettings.kcfg_proxyUrl->text() );
    d->m_settings.setValue( QLatin1String("proxyPort"), d->ui_cacheSettings.kcfg_proxyPort->value() );
    d->m_settings.setValue( QLatin1String("proxyType"), d->ui_cacheSettings.kcfg_proxyType->currentIndex() );
    if ( d->ui_cacheSettings.kcfg_proxyAuth->isChecked() ) {
        d->m_settings.setValue( QLatin1String("proxyAuth"), true );
        d->m_settings.setValue( QLatin1String("proxyUser"), d->ui_cacheSettings.kcfg_proxyUser->text() );
        d->m_settings.setValue( QLatin1String("proxyPass"), d->ui_cacheSettings.kcfg_proxyPass->text() );
    } else {
        d->m_settings.setValue( QLatin1String("proxyAuth"), false );
    }
    d->m_settings.endGroup();

    d->m_settings.beginGroup( QLatin1String("Time") );
    d->m_settings.setValue( QLatin1String("systemTimezone"), d->ui_timeSettings.kcfg_systemTimezone->isChecked() );
    d->m_settings.setValue( QLatin1String("UTC"), d->ui_timeSettings.kcfg_utc->isChecked() );
    d->m_settings.setValue( QLatin1String("customTimezone"), d->ui_timeSettings.kcfg_customTimezone->isChecked() );
    d->m_settings.setValue( QLatin1String("systemTime"), d->ui_timeSettings.kcfg_systemTime->isChecked() );
    d->m_settings.setValue( QLatin1String("lastSessionTime"), d->ui_timeSettings.kcfg_lastSessionTime->isChecked() );
    d->m_settings.setValue( QLatin1String("chosenTimezone"), d->ui_timeSettings.kcfg_chosenTimezone->currentIndex() );
    d->m_settings.endGroup();

    d->m_settings.beginGroup( QLatin1String("CloudSync") );
    d->m_settings.setValue( QLatin1String("enableSync"), d->ui_cloudSyncSettings.kcfg_enableSync->isChecked() );
    d->m_settings.setValue( QLatin1String("syncBackend"), QString::fromUtf8("owncloud") );
    d->m_settings.setValue( QLatin1String("syncBookmarks"), d->ui_cloudSyncSettings.kcfg_syncBookmarks->isChecked() );
    d->m_settings.setValue( QLatin1String("syncRoutes"), d->ui_cloudSyncSettings.kcfg_syncRoutes->isChecked() );
    d->m_settings.setValue( QLatin1String("owncloudServer"), d->ui_cloudSyncSettings.kcfg_owncloudServer->text() );
    d->m_settings.setValue( QLatin1String("owncloudUsername"), d->ui_cloudSyncSettings.kcfg_owncloudUsername->text() );
    d->m_settings.setValue( QLatin1String("owncloudPassword"), d->ui_cloudSyncSettings.kcfg_owncloudPassword->text() );
    d->m_settings.endGroup();

    // Plugins
    d->m_marbleWidget->writePluginSettings( d->m_settings );

    Q_EMIT settingsChanged();
}

MarbleLocale::MeasurementSystem QtMarbleConfigDialog::measurementSystem() const
{
    if( d->m_settings.contains( QString::fromUtf8("View/distanceUnit") ) ) {
        return (MarbleLocale::MeasurementSystem)d->m_settings.value( QString::fromUtf8("View/distanceUnit") ).toInt();
    }

    MarbleLocale *locale = MarbleGlobal::getInstance()->locale();
    return locale->measurementSystem();
}

Marble::AngleUnit QtMarbleConfigDialog::angleUnit() const
{
    return (Marble::AngleUnit) d->m_settings.value( QLatin1String("View/angleUnit"), Marble::DMSDegree ).toInt();
}

void QtMarbleConfigDialog::setAngleUnit(Marble::AngleUnit unit)
{
    d->m_settings.setValue( QLatin1String("View/angleUnit"), (int)unit );
    d->ui_viewSettings.kcfg_angleUnit->setCurrentIndex( angleUnit() );
    Q_EMIT settingsChanged();
}

Marble::MapQuality QtMarbleConfigDialog::stillQuality() const
{
    return (Marble::MapQuality) d->m_settings.value( QLatin1String("View/stillQuality"),
                                Marble::HighQuality ).toInt();
}

Marble::MapQuality QtMarbleConfigDialog::animationQuality() const
{
    return (Marble::MapQuality) d->m_settings.value( QLatin1String("View/animationQuality"),
                                Marble::LowQuality ).toInt();
}

QFont QtMarbleConfigDialog::mapFont() const
{
    return d->m_settings.value( QLatin1String("View/mapFont"), QApplication::font() ).value<QFont>();
}

int QtMarbleConfigDialog::onStartup() const
{
    bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    int defaultValue = smallScreen ? Marble::LastLocationVisited : Marble::ShowHomeLocation;
    return d->m_settings.value( QLatin1String("Navigation/onStartup"), defaultValue ).toInt();
}

QString QtMarbleConfigDialog::externalMapEditor() const
{
    return d->m_settings.value( QLatin1String("Navigation/externalMapEditor"), QString::fromUtf8("") ).toString();
}

bool QtMarbleConfigDialog::animateTargetVoyage() const
{
    const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    return d->m_settings.value( QLatin1String("Navigation/animateTargetVoyage"), smallScreen ).toBool();
}

bool QtMarbleConfigDialog::inertialEarthRotation() const
{
    return d->m_settings.value( QLatin1String("Navigation/inertialEarthRotation"), true ).toBool();
}

bool QtMarbleConfigDialog::mouseViewRotation() const
{
    return d->m_settings.value( QLatin1String("Navigation/mouseViewRotation"), true ).toBool();
}

int QtMarbleConfigDialog::volatileTileCacheLimit() const
{
    int defaultValue = (MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen) ? 6 : 100;
    return d->m_settings.value( QLatin1String("Cache/volatileTileCacheLimit"), defaultValue ).toInt();
}

int QtMarbleConfigDialog::persistentTileCacheLimit() const
{
    return d->m_settings.value( QLatin1String("Cache/persistentTileCacheLimit"), 0 ).toInt(); // default to unlimited
}

QString QtMarbleConfigDialog::proxyUrl() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyUrl"), QString::fromUtf8("") ).toString();
}

int QtMarbleConfigDialog::proxyPort() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyPort"), 8080 ).toInt();
}

QString QtMarbleConfigDialog::proxyUser() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyUser"), QString::fromUtf8("") ).toString();
}

QString QtMarbleConfigDialog::proxyPass() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyPass"), QString::fromUtf8("") ).toString();
}

bool QtMarbleConfigDialog::proxyType() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyType"), Marble::HttpProxy ).toInt();
}

bool QtMarbleConfigDialog::proxyAuth() const
{
    return d->m_settings.value( QLatin1String("Cache/proxyAuth"), false ).toBool();
}

bool QtMarbleConfigDialog::systemTimezone() const
{
    return d->m_settings.value( QLatin1String("Time/systemTimezone"), true ).toBool();
}

bool QtMarbleConfigDialog::customTimezone() const
{
    return d->m_settings.value( QLatin1String("Time/customTimezone"), false ).toBool();
}

bool QtMarbleConfigDialog::UTC() const
{
    return d->m_settings.value( QLatin1String("Time/UTC"), false ).toBool();
}

bool QtMarbleConfigDialog::systemTime() const
{
    return d->m_settings.value( QLatin1String("Time/systemTime"), true ).toBool();
}

bool QtMarbleConfigDialog::lastSessionTime() const
{
    return d->m_settings.value( QLatin1String("Time/lastSessionTime"), false ).toBool();
}

int QtMarbleConfigDialog::chosenTimezone() const
{
    return d->m_settings.value( QLatin1String("Time/chosenTimezone"), 0 ).toInt();
}

void QtMarbleConfigDialog::initializeCustomTimezone()
{
    if( d->m_timezone.count() == 0)
    {
        d->m_timezone.insert( 0, 0 );
        d->m_timezone.insert( 1, 3600 );
        d->m_timezone.insert( 2, 7200 );
        d->m_timezone.insert( 3, 7200 );
        d->m_timezone.insert( 4, 10800 );
        d->m_timezone.insert( 5, 12600 );
        d->m_timezone.insert( 6, 14400 );
        d->m_timezone.insert( 7, 18000 );
        d->m_timezone.insert( 8, 19800 );
        d->m_timezone.insert( 9, 21600 );
        d->m_timezone.insert( 10, 25200 );
        d->m_timezone.insert( 11, 28800 );
        d->m_timezone.insert( 12, 32400 );
        d->m_timezone.insert( 13, 34200 );
        d->m_timezone.insert( 14, 36000 );
        d->m_timezone.insert( 15, 39600 );
        d->m_timezone.insert( 16, 43200 );
        d->m_timezone.insert( 17, -39600 );
        d->m_timezone.insert( 18, -36000 );
        d->m_timezone.insert( 19, -32400 );
        d->m_timezone.insert( 20, -28800 );
        d->m_timezone.insert( 21, -25200 );
        d->m_timezone.insert( 22, -25200 );
        d->m_timezone.insert( 23, -21600 );
        d->m_timezone.insert( 24, -18000 );
        d->m_timezone.insert( 25, -18000 );
        d->m_timezone.insert( 26, -14400 );
        d->m_timezone.insert( 27, -12600 );
        d->m_timezone.insert( 28, -10800 );
        d->m_timezone.insert( 29, -10800 );
        d->m_timezone.insert( 30, -3600 );
    }
}

bool QtMarbleConfigDialog::syncEnabled() const
{
    return d->m_settings.value( QLatin1String("CloudSync/enableSync"), false ).toBool();
}

QString QtMarbleConfigDialog::syncBackend() const
{
    return d->m_settings.value( QLatin1String("CloudSync/syncBackend"), QString::fromUtf8("") ).toString();
}

bool QtMarbleConfigDialog::syncBookmarks() const
{
    return d->m_settings.value( QLatin1String("CloudSync/syncBookmarks"), true ).toBool();
}

bool QtMarbleConfigDialog::syncRoutes() const
{
    return d->m_settings.value( QLatin1String("CloudSync/syncRoutes"), true ).toBool();
}

QString QtMarbleConfigDialog::owncloudServer() const
{
    return d->m_settings.value( QLatin1String("CloudSync/owncloudServer"), QString::fromUtf8("") ).toString();
}

QString QtMarbleConfigDialog::owncloudUsername() const
{
    return d->m_settings.value( QLatin1String("CloudSync/owncloudUsername"), QString::fromUtf8("") ).toString();
}

QString QtMarbleConfigDialog::owncloudPassword() const
{
    return d->m_settings.value( QLatin1String("CloudSync/owncloudPassword"), QString::fromUtf8("") ).toString();
}

}

#include "moc_QtMarbleConfigDialog.cpp"
