/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-08-03
 * Description : setup Metadata tab.
 *
 * Copyright (C) 2003-2004 by Ralf Holzer <ralf at well dot com>
 * Copyright (C) 2003-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2017      by Simon Frei <freisim93 at gmail dot com>
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

#ifndef DIGIKAM_SETUP_METADATA_P_H
#define DIGIKAM_SETUP_METADATA_P_H

#include "setupmetadata.h"

// Qt includes

#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPointer>
#include <QRadioButton>
#include <QStandardPaths>
#include <QStyle>
#include <QToolButton>
#include <QVBoxLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "advancedmetadatatab.h"
#include "applicationsettings.h"
#include "dactivelabel.h"
#include "digikam_config.h"
#include "digikam_debug.h"
#include "metaengine.h"
#include "metadatapanel.h"
#include "metaenginesettings.h"
#include "setuputils.h"
#include "exiftoolconfpanel.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupMetadata::Private
{
public:

    explicit Private();

    void readSettings();

public:

    bool                 exifAutoRotateOriginal;
    bool                 exifAutoRotateShowedInfo;
    bool                 clearMetadataShowedInfo;

    QGroupBox*           fieldsGroup;
    QGroupBox*           readWriteGroup;
    QGroupBox*           rotationGroup;
    QGroupBox*           rotationAdvGroup;

    QCheckBox*           saveTagsBox;
    QCheckBox*           saveCommentsBox;
    QCheckBox*           saveRatingBox;
    QCheckBox*           savePickLabelBox;
    QCheckBox*           saveColorLabelBox;
    QCheckBox*           saveDateTimeBox;
    QCheckBox*           saveTemplateBox;
    QCheckBox*           saveFaceTags;
    QCheckBox*           savePosition;

    QCheckBox*           useLazySync;
    QCheckBox*           writeDngFilesBox;
    QCheckBox*           writeRawFilesBox;
    QCheckBox*           writeXMPSidecarBox;
    QCheckBox*           readXMPSidecarBox;
    QCheckBox*           sidecarFileNameBox;
    QCheckBox*           updateFileTimeStampBox;
    QCheckBox*           rescanImageIfModifiedBox;
    QCheckBox*           clearMetadataIfRescanBox;
    QComboBox*           writingModeCombo;

    QRadioButton*        rotateByFlag;
    QRadioButton*        rotateByContents;
    QCheckBox*           allowRotateByMetadata;
    QCheckBox*           allowLossyRotate;
    QCheckBox*           exifRotateBox;
    QCheckBox*           exifSetOrientationBox;

    QCheckBox*           saveToBalooBox;
    QCheckBox*           readFromBalooBox;

    QTabWidget*          tab;
    QTabWidget*          displaySubTab;

    MetadataPanel*       tagsCfgPanel;
    AdvancedMetadataTab* advTab;

    ExifToolConfPanel*   exifToolView;

    QLineEdit*           extensionsEdit;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_METADATA_P_H
