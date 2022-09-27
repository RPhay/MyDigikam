/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-21
 * Description : MJPEG Stream Server configuration dialog - Private container
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021 by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MJPEG_STREAM_DLG_P_H
#define DIGIKAM_MJPEG_STREAM_DLG_P_H

// Qt includes

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QStyle>
#include <QIcon>
#include <QCheckBox>
#include <QGroupBox>
#include <QTabWidget>
#include <QDesktopServices>

// KDE includes

#include <klocalizedstring.h>
#include <ksharedconfig.h>
#include <kconfiggroup.h>

// Local includes

#include "mjpegstreamdlg.h"
#include "dinfointerface.h"
#include "ditemslist.h"
#include "dnuminput.h"
#include "dlayoutbox.h"
#include "dxmlguiwindow.h"
#include "workingwidget.h"
#include "mjpegservermngr.h"
#include "vidslidesettings.h"
#include "transitionpreview.h"
#include "effectpreview.h"
#include "dcombobox.h"
#include "dfontselect.h"

namespace DigikamGenericMjpegStreamPlugin
{

class Q_DECL_HIDDEN MjpegStreamDlg::Private
{

public:

    enum TabView
    {
        Server = 0,
        Stream,
        Transition,
        Effect,
        OSD             ///< On Screen Display options
    };

public:

    explicit Private();
    ~Private();

public:

    bool                dirty;
    MjpegServerMngr*    mngr;
    QPushButton*        srvButton;
    QLabel*             srvStatus;
    QPushButton*        srvPreview;
    WorkingWidget*      progress;
    QLabel*             aStats;
    QLabel*             separator;
    QLabel*             iStats;
    QCheckBox*          startOnStartup;
    int                 spacing;
    bool                albumSupport;
    QWidget*            albumSelector;
    DItemsList*         listView;
    QDialogButtonBox*   buttons;
    QWidget*            streamSettings;
    DIntNumInput*       srvPort;
    DIntNumInput*       delay;
    DIntNumInput*       rate;
    DIntNumInput*       quality;
    QCheckBox*          streamLoop;
    DComboBox*          typeVal;
    DComboBox*          effVal;
    EffectPreview*      effPreview;
    DComboBox*          transVal;
    TransitionPreview*  transPreview;
    QTabWidget*         tabView;
    QCheckBox*          showName;
    QCheckBox*          showDate;
    QCheckBox*          showApertureFocal;
    QCheckBox*          showExpoSensitivity;
    QCheckBox*          showMakeModel;
    QCheckBox*          showLensModel;
    QCheckBox*          showComment;
    QCheckBox*          showTitle;
    QCheckBox*          showTags;
    QCheckBox*          showRating;
    QCheckBox*          showCapIfNoTitle;
    DFontSelect*        osdFont;

    MjpegStreamSettings settings;
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_STREAM_DLG_P_H
