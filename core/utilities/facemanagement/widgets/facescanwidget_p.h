/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-10-09
 * Description : Widget to choose options for face scanning
 *
 * SPDX-FileCopyrightText: 2010-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FACE_SCAN_WIDGET_P_H
#define DIGIKAM_FACE_SCAN_WIDGET_P_H

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

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_config.h"
#include "digikam_globals.h"
#include "dlayoutbox.h"
#include "dnuminput.h"
#include "digikam_debug.h"
#include "albummodel.h"
#include "albumselectors.h"
#include "albummanager.h"
#include "applicationsettings.h"
#include "squeezedcombobox.h"
#include "dexpanderbox.h"
#include "facedbaccess.h"
#include "facedb.h"

namespace Digikam
{

class Q_DECL_HIDDEN FaceScanWidget::Private
{
public:

    explicit Private()
        : workflowWidget                    (nullptr),
          detectAndRecognizeButton          (nullptr),
          detectButton                      (nullptr),
          alreadyScannedBox                 (nullptr),
          reRecognizeButton                 (nullptr),
          helpButton                        (nullptr),
          tabWidget                         (nullptr),
          albumSelectors                    (nullptr),
          accuracyInput                     (nullptr),
          useYoloV3Button                   (nullptr),
          useFullCpuButton                  (nullptr),
          configName                        (QLatin1String("Face Management Settings")),
          configMainTask                    (QLatin1String("Face Scan Main Task")),
          configValueDetect                 (QLatin1String("Detect")),
          configValueDetectAndRecognize     (QLatin1String("Detect and Recognize Faces")),
          configValueRecognizedMarkedFaces  (QLatin1String("Recognize Marked Faces")),
          configAlreadyScannedHandling      (QLatin1String("Already Scanned Handling")),
          configUseFullCpu                  (QLatin1String("Use Full CPU")),
          settingsConflicted                (false)
    {
    }

    QWidget*          workflowWidget;
    QRadioButton*     detectAndRecognizeButton;
    QRadioButton*     detectButton;
    SqueezedComboBox* alreadyScannedBox;
    QRadioButton*     reRecognizeButton;
    QPushButton*      helpButton;

    QTabWidget*       tabWidget;

    AlbumSelectors*   albumSelectors;

    DIntNumInput*     accuracyInput;

    QCheckBox*        useYoloV3Button;
    QCheckBox*        useFullCpuButton;

    const QString     configName;
    const QString     configMainTask;
    const QString     configValueDetect;
    const QString     configValueDetectAndRecognize;
    const QString     configValueRecognizedMarkedFaces;
    const QString     configAlreadyScannedHandling;
    const QString     configUseFullCpu;

    bool              settingsConflicted;
};

} // namespace Digikam

#endif // DIGIKAM_FACE_SCAN_WIDGET_P_H
