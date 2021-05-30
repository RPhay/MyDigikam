/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-14
 * Description : database settings widget - private container
 *
 * Copyright (C) 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DB_SETTINGS_WIDGET_P_H
#define DIGIKAM_DB_SETTINGS_WIDGET_P_H

#include "dbsettingswidget.h"

// Qt includes

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QFileInfo>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>
#include <QString>
#include <QStyle>
#include <QTabWidget>
#include <QTemporaryFile>
#include <QTextBrowser>
#include <QTimer>
#include <QVBoxLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "digikam_globals.h"
#include "applicationsettings.h"
#include "dfileselector.h"
#include "dbengineparameters.h"
#include "dbinarysearch.h"
#include "dexpanderbox.h"
#include "dlayoutbox.h"
#include "mysqladminbinary.h"
#include "mysqlinitbinary.h"
#include "mysqlservbinary.h"
#include "albummanager.h"

namespace Digikam
{

class Q_DECL_HIDDEN DatabaseSettingsWidget::Private
{

public:

    explicit Private()
      : mysqlCmdBox             (nullptr),
        dbNameCore              (nullptr),
        dbNameThumbs            (nullptr),
        dbNameFace              (nullptr),
        dbNameSimilarity        (nullptr),
        hostName                (nullptr),
        connectOpts             (nullptr),
        userName                (nullptr),
        password                (nullptr),
        hostPort                (nullptr),
        dbType                  (nullptr),
        dbPathLabel             (nullptr),
        sqlInit                 (nullptr),
        expertSettings          (nullptr),
        dbNoticeBox             (nullptr),
        dbDetailsBox            (nullptr),
        tab                     (nullptr),
        dbPathEdit              (nullptr),
        dbBinariesWidget        (nullptr),
        ignoreDirectoriesBox    (nullptr),
        ignoreDirectoriesEdit   (nullptr),
        ignoreDirectoriesLabel  (nullptr)
    {
    }

    DVBox*             mysqlCmdBox;

    QLineEdit*         dbNameCore;
    QLineEdit*         dbNameThumbs;
    QLineEdit*         dbNameFace;
    QLineEdit*         dbNameSimilarity;
    QLineEdit*         hostName;
    QLineEdit*         connectOpts;
    QLineEdit*         userName;
    QLineEdit*         password;

    QSpinBox*          hostPort;

    QComboBox*         dbType;
    QLabel*            dbPathLabel;
    QTextBrowser*      sqlInit;
    QGroupBox*         expertSettings;
    QGroupBox*         dbNoticeBox;
    QGroupBox*         dbDetailsBox;
    QTabWidget*        tab;

    DFileSelector*     dbPathEdit;

    DBinarySearch*     dbBinariesWidget;

    MysqlInitBinary    mysqlInitBin;
    MysqlAdminBinary   mysqlAdminBin;
    MysqlServBinary    mysqlServBin;

    DbEngineParameters orgPrms;

    QMap<int, int>     dbTypeMap;

    QGroupBox*         ignoreDirectoriesBox;
    QLineEdit*         ignoreDirectoriesEdit;
    QLabel*            ignoreDirectoriesLabel;
};

} // namespace Digikam

#endif // DIGIKAM_DB_SETTINGS_WIDGET_P_H
