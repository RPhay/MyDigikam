/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-14
 * Description : database settings widget - private container
 *
 * SPDX-FileCopyrightText: 2009-2010 by Holger Foerster <Hamsi2k at freenet dot de>
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DB_SETTINGS_WIDGET_P_H
#define DIGIKAM_DB_SETTINGS_WIDGET_P_H

#include "dbsettingswidget.h"

// Qt includes

#include <QApplication>
#include <QCheckBox>
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
        dbNameFace              (nullptr),
        dbNameSimilarity        (nullptr),
        hostName                (nullptr),
        connectOpts             (nullptr),
        userName                (nullptr),
        password                (nullptr),
        hostPort                (nullptr),
        dbType                  (nullptr),
        dbPathLabel             (nullptr),
        dbThumbsLabel           (nullptr),
        walLabel                (nullptr),
        sqlInit                 (nullptr),
        expertSettings          (nullptr),
        dbNoticeBox             (nullptr),
        dbDetailsBox            (nullptr),
        tab                     (nullptr),
        dbPathEdit              (nullptr),
        dbNameThumbs            (nullptr),
        walModeCheck            (nullptr),
        dbBinariesWidget        (nullptr),
        ignoreDirectoriesBox    (nullptr),
        ignoreDirectoriesEdit   (nullptr),
        ignoreDirectoriesLabel  (nullptr)
    {
    }

    DVBox*             mysqlCmdBox;

    QLineEdit*         dbNameCore;
    QLineEdit*         dbNameFace;
    QLineEdit*         dbNameSimilarity;
    QLineEdit*         hostName;
    QLineEdit*         connectOpts;
    QLineEdit*         userName;
    QLineEdit*         password;

    QSpinBox*          hostPort;

    QComboBox*         dbType;
    QLabel*            dbPathLabel;
    QLabel*            dbThumbsLabel;
    QLabel*            walLabel;
    QTextBrowser*      sqlInit;
    QGroupBox*         expertSettings;
    QGroupBox*         dbNoticeBox;
    QGroupBox*         dbDetailsBox;
    QTabWidget*        tab;

    DFileSelector*     dbPathEdit;
    DFileSelector*     dbNameThumbs;

    QCheckBox*         walModeCheck;

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
