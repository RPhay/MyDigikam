/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-30
 * Description : maintenance dialog
 *
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "maintenancedlg.h"

// Qt includes

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QComboBox>
#include <QScrollArea>
#include <QIcon>
#include <QStandardPaths>
#include <QDialogButtonBox>
#include <QVBoxLayout>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_config.h"
#include "digikam_globals.h"
#include "dlayoutbox.h"
#include "dexpanderbox.h"
#include "dnuminput.h"
#include "setup.h"
#include "albumselectors.h"
#include "facescansettings.h"
#include "imagequalitycontainer.h"
#include "imagequalityconfselector.h"
#include "metadatasynchronizer.h"
#include "dxmlguiwindow.h"
#include "applicationsettings.h"
#include "drangebox.h"
#include "autotagsassignment.h"
#include "autotagsassign.h"

namespace Digikam
{

class Q_DECL_HIDDEN MaintenanceDlg::Private
{
public:

    enum Operation
    {
        Options = 0,
        NewItems,
        DbCleanup,
        Thumbnails,
        FingerPrints,
        Duplicates,
        FaceManagement,
        AutotagsAssignment,
        ImageQualitySorter,
        MetadataSync,
        Stretch
    };

public:

    Private();

public:

    static const QString configGroupName;
    static const QString configUseLastSettings;
    static const QString configUseMutiCoreCPU;
    static const QString configNewItems;
    static const QString configThumbnails;
    static const QString configScanThumbs;
    static const QString configFingerPrints;
    static const QString configScanFingerPrints;
    static const QString configDuplicates;
    static const QString configMinSimilarity;
    static const QString configMaxSimilarity;
    static const QString configDuplicatesRestriction;
    static const QString configFaceManagement;
    static const QString configFaceScannedHandling;
    static const QString configAutotagsAssignment;
    static const QString configAutotaggingScanMode;
    static const QString configModelSelectionMode;
    static const QString configImageQualitySorter;
    static const QString configQualityScanMode;
    static const QString configQualitySettingsSelected;
    static const QString configMetadataSync;
    static const QString configCleanupDatabase;
    static const QString configCleanupThumbDatabase;
    static const QString configCleanupFacesDatabase;
    static const QString configCleanupSimilarityDatabase;
    static const QString configShrinkDatabases;
    static const QString configSyncDirection;

public:

    QDialogButtonBox*         buttons;
    QLabel*                   logo;
    QLabel*                   title;
    QCheckBox*                scanThumbs;
    QCheckBox*                scanFingerPrints;
    QCheckBox*                useLastSettings;
    QCheckBox*                useMutiCoreCPU;
    QCheckBox*                cleanThumbsDb;
    QCheckBox*                cleanFacesDb;
    QCheckBox*                cleanSimilarityDb;
    QCheckBox*                retrainAllFaces;
    QCheckBox*                shrinkDatabases;
    QComboBox*                qualityScanMode;
    QComboBox*                autotaggingScanMode;
    QComboBox*                modelSelectionMode;
    QPushButton*              metadataSetup;
    ImageQualityConfSelector* qualitySelector;
    QComboBox*                syncDirection;
    DHBox*                    similarityRangeBox;
    DHBox*                    dupeRestrictionBox;
    DVBox*                    vbox;
    DVBox*                    vbox2;
    DVBox*                    vbox3;
    DVBox*                    vbox4;
    DVBox*                    vbox5;
    DVBox*                    duplicatesBox;
    DIntRangeBox*             similarityRange;
    QComboBox*                faceScannedHandling;
    QComboBox*                searchResultRestriction;
    DExpanderBox*             expanderBox;
    AlbumSelectors*           albumSelectors;
};

} // namespace Digikam
