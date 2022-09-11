/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-23
 * Description : mics configuration setup tab
 *
 * SPDX-FileCopyrightText: 2004      by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2005-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2017      by Simon Frei <freisim93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_MISC_P_H
#define DIGIKAM_SETUP_MISC_P_H

#include "setupmisc.h"

// Qt includes

#include <QApplication>
#include <QDirIterator>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QGroupBox>
#include <QHash>
#include <QLabel>
#include <QRadioButton>
#include <QSpinBox>
#include <QStyle>
#include <QStyleFactory>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QMessageBox>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "dexpanderbox.h"
#include "dlayoutbox.h"
#include "dfontselect.h"
#include "thememanager.h"
#include "metaenginesettings.h"
#include "applicationsettings.h"
#include "systemsettingswidget.h"
#include "spellcheckconfig.h"
#include "onlineversionchecker.h"
#include "setup.h"
#include "localizeconfig.h"

#ifdef HAVE_SONNET
#   include "spellcheckconfig.h"
#endif

namespace Digikam
{

class Q_DECL_HIDDEN SetupMisc::Private
{
public:

    explicit Private()
      : tab                                     (nullptr),
        updateTypeLabel                         (nullptr),
        updateWithDebug                         (nullptr),
        sidebarTypeLabel                        (nullptr),
        albumDateSourceLabel                    (nullptr),
        stringComparisonTypeLabel               (nullptr),
        applicationStyleLabel                   (nullptr),
        applicationIconLabel                    (nullptr),
        minSimilarityBoundLabel                 (nullptr),
        showSplashCheck                         (nullptr),
        showTrashDeleteDialogCheck              (nullptr),
        showPermanentDeleteDialogCheck          (nullptr),
        sidebarApplyDirectlyCheck               (nullptr),
        useNativeFileDialogCheck                (nullptr),
        drawFramesToGroupedCheck                (nullptr),
        expandNewCurrentItemCheck               (nullptr),
        scrollItemToCenterCheck                 (nullptr),
        showOnlyPersonTagsInPeopleSidebarCheck  (nullptr),
        scanAtStart                             (nullptr),
        useFastScan                             (nullptr),
        detectFaces                             (nullptr),
        cleanAtStart                            (nullptr),
        updateType                              (nullptr),
        sidebarType                             (nullptr),
        albumDateSource                         (nullptr),
        stringComparisonType                    (nullptr),
        applicationStyle                        (nullptr),
        applicationIcon                         (nullptr),
        applicationFont                         (nullptr),
        minimumSimilarityBound                  (nullptr),
        systemSettingsWidget                    (nullptr),

#ifdef HAVE_SONNET

        spellCheckWidget                        (nullptr),

#endif

        localizeWidget                          (nullptr),
        groupingButtons                         (QHash<int, QButtonGroup*>())
    {
    }

    QTabWidget*               tab;

    QLabel*                   updateTypeLabel;
    QCheckBox*                updateWithDebug;

    QLabel*                   sidebarTypeLabel;
    QLabel*                   albumDateSourceLabel;
    QLabel*                   stringComparisonTypeLabel;
    QLabel*                   applicationStyleLabel;
    QLabel*                   applicationIconLabel;
    QLabel*                   minSimilarityBoundLabel;

    QCheckBox*                showSplashCheck;
    QCheckBox*                showTrashDeleteDialogCheck;
    QCheckBox*                showPermanentDeleteDialogCheck;
    QCheckBox*                sidebarApplyDirectlyCheck;
    QCheckBox*                useNativeFileDialogCheck;
    QCheckBox*                drawFramesToGroupedCheck;
    QCheckBox*                expandNewCurrentItemCheck;
    QCheckBox*                scrollItemToCenterCheck;
    QCheckBox*                showOnlyPersonTagsInPeopleSidebarCheck;
    QCheckBox*                scanAtStart;
    QCheckBox*                useFastScan;
    QCheckBox*                detectFaces;
    QCheckBox*                cleanAtStart;

    QComboBox*                updateType;
    QComboBox*                sidebarType;
    QComboBox*                albumDateSource;
    QComboBox*                stringComparisonType;
    QComboBox*                applicationStyle;
    QComboBox*                applicationIcon;
    DFontSelect*              applicationFont;

    QSpinBox*                 minimumSimilarityBound;

    SystemSettingsWidget*     systemSettingsWidget;

#ifdef HAVE_SONNET

    SpellCheckConfig*         spellCheckWidget;

#endif

    LocalizeConfig*           localizeWidget;
    QHash<int, QButtonGroup*> groupingButtons;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_MISC_P_H
