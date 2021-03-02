/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-14
 * Description : database setup tab
 *
 * Copyright (C) 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "setupdatabase.h"

// Qt includes

#include <QApplication>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QHelpEvent>
#include <QGroupBox>
#include <QCursor>
#include <QStyle>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "albummanager.h"
#include "applicationsettings.h"
#include "coredbschemaupdater.h"
#include "databaseserverstarter.h"
#include "dbengineparameters.h"
#include "dbsettingswidget.h"
#include "digikam_debug.h"
#include "scancontroller.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupDatabase::Private
{
public:

    explicit Private()
      : databaseWidget(nullptr),
        updateBox(nullptr),
        hashesButton(nullptr)
    {
    }

    DatabaseSettingsWidget* databaseWidget;
    QGroupBox*              updateBox;
    QPushButton*            hashesButton;
};

SetupDatabase::SetupDatabase(QWidget* const parent)
    : QScrollArea(parent),
      d(new Private)
{
    const int spacing    = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);

    QWidget* const panel = new QWidget(viewport());
    setWidget(panel);
    setWidgetResizable(true);

    // --------------------------------------------------------

    QVBoxLayout* const layout = new QVBoxLayout(panel);
    d->databaseWidget         = new DatabaseSettingsWidget(panel);
    layout->addWidget(d->databaseWidget);

    if (!CoreDbSchemaUpdater::isUniqueHashUpToDate())
    {
        createUpdateBox();
        layout->addStretch(10);
        layout->addWidget(d->updateBox);
    }

    layout->setContentsMargins(spacing, spacing, spacing, spacing);
    layout->setSpacing(spacing);

    // --------------------------------------------------------

    readSettings();
    adjustSize();
}

SetupDatabase::~SetupDatabase()
{
    delete d;
}

void SetupDatabase::applySettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    if (d->databaseWidget->getDbEngineParameters() == d->databaseWidget->orgDatabasePrm())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "No DB settings changes. Do nothing...";
        return;
    }

    if (!d->databaseWidget->checkDatabaseSettings())
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DB settings check invalid. Do nothing...";
        return;
    }

    switch (d->databaseWidget->databaseType())
    {
        case DatabaseSettingsWidget::SQlite:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Switch to SQlite DB config...";
            DbEngineParameters params = d->databaseWidget->getDbEngineParameters();
            settings->setDbEngineParameters(params);
            settings->saveSettings();
            AlbumManager::instance()->changeDatabase(params);
            break;
        }

        case DatabaseSettingsWidget::MysqlInternal:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Switch to Mysql Internal DB config...";
            DbEngineParameters params = d->databaseWidget->getDbEngineParameters();
            settings->setDbEngineParameters(params);
            settings->saveSettings();
            AlbumManager::instance()->changeDatabase(params);
            break;
        }

        default: // DatabaseSettingsWidget::MysqlServer
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "Switch to Mysql server DB config...";
            DbEngineParameters params = d->databaseWidget->getDbEngineParameters();
            settings->setDbEngineParameters(params);
            settings->saveSettings();
            AlbumManager::instance()->changeDatabase(params);
            break;
        }
    }
}

void SetupDatabase::readSettings()
{
    ApplicationSettings* const settings = ApplicationSettings::instance();

    if (!settings)
    {
        return;
    }

    d->databaseWidget->setParametersFromSettings(settings);
}

void SetupDatabase::upgradeUniqueHashes()
{
    int result = QMessageBox::warning(this, qApp->applicationName(),
                                      i18nc("@info",
                                            "The process of updating the file hashes takes a few minutes.\n"
                                            "Please ensure that any important collections on removable media are connected.\n"
                                            "After the upgrade you cannot use your database with a digiKam version prior to 2.0.\n"
                                            "Do you want to begin the update?"),
                                            QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes)
    {
        ScanController::instance()->updateUniqueHash();
    }
}

void SetupDatabase::createUpdateBox()
{
    d->updateBox                    = new QGroupBox(i18nc("@title:group", "Updates"));
    QGridLayout* const updateLayout = new QGridLayout;

    d->hashesButton                 = new QPushButton(i18nc("@action:button", "Update File Hashes"));
    d->hashesButton->setWhatsThis(i18nc("@info:tooltip",
                                        "File hashes are used to identify identical files and to display thumbnails. "
                                        "A new, improved algorithm to create the hash is now used. "
                                        "The old algorithm, though, still works quite well, so it is recommended to "
                                        "carry out this upgrade, but not required.\n"
                                        "After the upgrade you cannot use your database with a digiKam version "
                                        "prior to 2.0."));

    QPushButton* const infoHash     = new QPushButton;
    infoHash->setIcon(QIcon::fromTheme(QLatin1String("dialog-information")));
    infoHash->setToolTip(i18nc("@info:tooltip", "Get information about <interface>Update File Hashes</interface>"));

    updateLayout->addWidget(d->hashesButton, 0, 0);
    updateLayout->addWidget(infoHash,        0, 1);
    updateLayout->setColumnStretch(2, 1);

    d->updateBox->setLayout(updateLayout);

    connect(d->hashesButton, SIGNAL(clicked()),
            this, SLOT(upgradeUniqueHashes()));

    connect(infoHash, SIGNAL(clicked()),
            this, SLOT(showHashInformation()));
}

void SetupDatabase::showHashInformation()
{
    qApp->postEvent(d->hashesButton, new QHelpEvent(QEvent::WhatsThis, QPoint(0, 0), QCursor::pos()));
}

} // namespace Digikam
