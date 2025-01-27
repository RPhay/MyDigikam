/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality settings widget
 *
 * SPDX-FileCopyrightText: 2013-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

#include "imagequalitysettings.h"

// Qt includes

#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>
#include <QApplication>
#include <QStyle>
#include <QPushButton>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes

#include "albummodel.h"
#include "albumselectors.h"
#include "albummanager.h"
#include "dlayoutbox.h"
#include "picklabelwidget.h"
#include "dnuminput.h"

namespace Digikam
{

class Q_DECL_HIDDEN ImageQualitySettings::Private
{
public:

    enum DetectMethod
    {
        AESTHETIC = 0,
        BASICFACTORS
    };

public:

    Private() = default;

public:

    const QString                             configName            = QLatin1String("Image Quality Settings");

    AlbumSelectors*                           albumSelectors        = nullptr;

    QWidget*                                  rulesWidget           = nullptr;
    QWidget*                                  basicView             = nullptr;

    QCheckBox*                                detectBlur            = nullptr;
    QCheckBox*                                detectNoise           = nullptr;
    QCheckBox*                                detectCompression     = nullptr;
    QCheckBox*                                detectExposure        = nullptr;

    QButtonGroup*                             detectButtonGroup     = nullptr;
    QRadioButton*                             detectAesthetic       = nullptr;
    QRadioButton*                             detectBasicFactors    = nullptr;

    QCheckBox*                                setRejected           = nullptr;
    QCheckBox*                                setPending            = nullptr;
    QCheckBox*                                setAccepted           = nullptr;

    QLabel*                                   lbl2                  = nullptr;
    QLabel*                                   lbl3                  = nullptr;
    QLabel*                                   lbl4                  = nullptr;
    QLabel*                                   lbl5                  = nullptr;
    QLabel*                                   lbl6                  = nullptr;
    QLabel*                                   lbl7                  = nullptr;

    DIntNumInput*                             setRejectedThreshold  = nullptr;
    DIntNumInput*                             setPendingThreshold   = nullptr;
    DIntNumInput*                             setAcceptedThreshold  = nullptr;
    DIntNumInput*                             setBlurWeight         = nullptr;
    DIntNumInput*                             setNoiseWeight        = nullptr;
    DIntNumInput*                             setCompressionWeight  = nullptr;

    ImageQualitySettings::SettingsDisplayMode displayMode           = ImageQualitySettings::SettingsDisplayMode::Normal;
};

} // namespace Digikam
