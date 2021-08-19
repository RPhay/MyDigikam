/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-21
 * Description : MJPEG Stream Server configuration dialog
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
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
        Effect
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
    MjpegStreamSettings settings;
};

} // namespace DigikamGenericMjpegStreamPlugin

#endif // DIGIKAM_MJPEG_STREAM_DLG_P_H
