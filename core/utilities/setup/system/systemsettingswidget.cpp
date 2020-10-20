/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-27-07
 * Description : system settings widget
 *
 * Copyright (C) 2020 by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "systemsettingswidget.h"

// Qt includes

#include <QApplication>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "systemsettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN SystemSettingsWidget::Private
{

public:

    explicit Private()
      : useHighDpiScalingCheck(nullptr),
        useHighDpiPixmapsCheck(nullptr),
        disableOpenCLCheck    (nullptr)
    {
    }

    QCheckBox* useHighDpiScalingCheck;
    QCheckBox* useHighDpiPixmapsCheck;
    QCheckBox* disableOpenCLCheck;
};

SystemSettingsWidget::SystemSettingsWidget(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing         = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    QVBoxLayout* const layout = new QVBoxLayout(this);

    d->useHighDpiScalingCheck = new QCheckBox(i18n("Use high DPI scaling from the screen factor"), this);
    d->useHighDpiPixmapsCheck = new QCheckBox(i18n("Use pixmaps with high DPI resolution"), this);
    d->disableOpenCLCheck     = new QCheckBox(i18n("Disable hardware acceleration OpenCL"), this);

    if (qApp->applicationName() == QLatin1String("showfoto"))
    {
        d->disableOpenCLCheck->hide();
    }

    QLabel* const systemNote  = new QLabel(i18n("<b>Note: All changes to these settings only take effect "
                                                "after the restart. Some settings are hardware dependent "
                                                "and may have no effect.</b>"), this);
    systemNote->setWordWrap(true);
    systemNote->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    layout->addWidget(d->useHighDpiScalingCheck);
    layout->addWidget(d->useHighDpiPixmapsCheck);
    layout->addWidget(d->disableOpenCLCheck);
    layout->addWidget(systemNote);
    layout->setContentsMargins(spacing, spacing, spacing, spacing);
    layout->setSpacing(spacing);
    layout->addStretch();
}

SystemSettingsWidget::~SystemSettingsWidget()
{
    delete d;
}

void SystemSettingsWidget::readSettings()
{
    SystemSettings system(qApp->applicationName());
    system.readSettings();

    d->useHighDpiScalingCheck->setChecked(system.useHighDpiScaling);
    d->useHighDpiPixmapsCheck->setChecked(system.useHighDpiPixmaps);
    d->disableOpenCLCheck->setChecked(system.disableOpenCL);
}

void SystemSettingsWidget::saveSettings()
{
    SystemSettings system(qApp->applicationName());

    system.useHighDpiScaling = d->useHighDpiScalingCheck->isChecked();
    system.useHighDpiPixmaps = d->useHighDpiPixmapsCheck->isChecked();
    system.disableOpenCL     = d->disableOpenCLCheck->isChecked();

    system.saveSettings();
}

} // namespace Digikam
