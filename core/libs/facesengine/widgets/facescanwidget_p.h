/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Widget to choose options for face scanning
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "facescanwidget.h"

// Qt includes

#include <QApplication>
#include <QButtonGroup>
#include <QGroupBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QRadioButton>
#include <QToolButton>
#include <QIcon>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStyle>
#include <QList>
#include <QStandardItemModel>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_globals.h"
#include "digikam_opencv.h"
#include "dlayoutbox.h"
#include "dnuminput.h"
#include "digikam_debug.h"
#include "albummodel.h"
#include "albumselectors.h"
#include "albummanager.h"
#include "squeezedcombobox.h"
#include "dexpanderbox.h"
#include "facedbaccess.h"
#include "facedb.h"
#include "facesengine.h"

namespace Digikam
{

class Q_DECL_HIDDEN FaceScanWidget::Private
{
public:

    Private() = default;

public:

    QWidget*          workflowWidget                    = nullptr;

    SqueezedComboBox* alreadyScannedBox                 = nullptr;
    QRadioButton*     detectButton                      = nullptr;
    QRadioButton*     detectAndRecognizeButton          = nullptr;
    QRadioButton*     reRecognizeButton                 = nullptr;
    QPushButton*      helpButton                        = nullptr;

    // ---

    AlbumSelectors*   albumSelectors                    = nullptr;

    // ---

    QWidget*          settingsTab                       = nullptr;

    SqueezedComboBox* detectSizeBox                     = nullptr;
    DIntNumInput*     detectAccuracyInput               = nullptr;
    DIntNumInput*     recognizeAccuracyInput            = nullptr;

    // ---

    QCheckBox*        useFullCpuButton                  = nullptr;

    const QString     configName                        = QLatin1String("Face Management Settings");

    bool              settingsConflicted                = false;

    FacesEngine*      facesDetector                     = nullptr;
};

} // namespace Digikam
