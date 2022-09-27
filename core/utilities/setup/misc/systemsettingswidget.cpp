/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-27-07
 * Description : system settings widget
 *
 * SPDX-FileCopyrightText: 2020 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "systemsettingswidget.h"

// Qt includes

#include <QApplication>
#include <QPushButton>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "systemsettings.h"
#include "filesdownloader.h"

namespace Digikam
{

class Q_DECL_HIDDEN SystemSettingsWidget::Private
{

public:

    explicit Private()
      : useHighDpiScalingCheck(nullptr),
        useHighDpiPixmapsCheck(nullptr),
        disableOpenCLCheck    (nullptr),
        enableLoggingCheck    (nullptr),
        filesDownloadButton   (nullptr),
        filesDownloader       (nullptr)
    {
    }

    QCheckBox*       useHighDpiScalingCheck;
    QCheckBox*       useHighDpiPixmapsCheck;
    QCheckBox*       disableOpenCLCheck;
    QCheckBox*       enableLoggingCheck;

    QPushButton*     filesDownloadButton;

    FilesDownloader* filesDownloader;
};

SystemSettingsWidget::SystemSettingsWidget(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing         = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                                     QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    QGridLayout* const layout = new QGridLayout(this);

    d->filesDownloader        = new FilesDownloader(this);

    d->useHighDpiScalingCheck = new QCheckBox(i18n("Use high DPI scaling from the screen factor"), this);
    d->useHighDpiPixmapsCheck = new QCheckBox(i18n("Use pixmaps with high DPI resolution"), this);
    d->disableOpenCLCheck     = new QCheckBox(i18n("Disable hardware acceleration OpenCL"), this);
    d->enableLoggingCheck     = new QCheckBox(i18n("Enable internal debug logging"), this);

    d->filesDownloadButton    = new QPushButton(i18n("Download Face Engine Data..."), this);
    d->filesDownloadButton->setEnabled(!d->filesDownloader->checkDownloadFiles());

    if (qApp->applicationName() == QLatin1String("showfoto"))
    {
        d->disableOpenCLCheck->hide();
    }

    QLabel* const systemNote  = new QLabel(i18n("<b>Note: All changes to these settings only take effect "
                                                "after the restart. Some settings are hardware dependent "
                                                "and may have no effect.</b>"), this);
    systemNote->setWordWrap(true);
    systemNote->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    layout->addWidget(d->useHighDpiScalingCheck, 0, 0, 1, 1);
    layout->addWidget(d->useHighDpiPixmapsCheck, 1, 0, 1, 1);
    layout->addWidget(d->disableOpenCLCheck,     2, 0, 1, 1);
    layout->addWidget(d->enableLoggingCheck,     3, 0, 1, 1);
    layout->addWidget(d->filesDownloadButton,    4, 0, 1, 1);
    layout->addWidget(systemNote,                5, 0, 1, 2);
    layout->setContentsMargins(spacing, spacing, spacing, spacing);
    layout->setRowStretch(6, 10);

    connect(d->filesDownloadButton, &QPushButton::pressed,
            this, &SystemSettingsWidget::slotFaceDownload);
}

SystemSettingsWidget::~SystemSettingsWidget()
{
    delete d;
}

void SystemSettingsWidget::readSettings()
{
    SystemSettings system(qApp->applicationName());

    d->useHighDpiScalingCheck->setChecked(system.useHighDpiScaling);
    d->useHighDpiPixmapsCheck->setChecked(system.useHighDpiPixmaps);
    d->enableLoggingCheck->setChecked(system.enableLogging);
    d->disableOpenCLCheck->setChecked(system.disableOpenCL);
}

void SystemSettingsWidget::saveSettings()
{
    SystemSettings system(qApp->applicationName());

    system.useHighDpiScaling = d->useHighDpiScalingCheck->isChecked();
    system.useHighDpiPixmaps = d->useHighDpiPixmapsCheck->isChecked();
    system.enableLogging     = d->enableLoggingCheck->isChecked();
    system.disableOpenCL     = d->disableOpenCLCheck->isChecked();

    system.saveSettings();
}

void SystemSettingsWidget::slotFaceDownload()
{
    d->filesDownloader->startDownload();
    d->filesDownloadButton->setEnabled(!d->filesDownloader->checkDownloadFiles());
}

} // namespace Digikam
