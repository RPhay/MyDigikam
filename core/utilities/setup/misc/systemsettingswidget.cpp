/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-27-07
 * Description : system settings widget
 *
 * SPDX-FileCopyrightText: 2020-2023 by Maik Qualmann <metzpinguin at gmail dot com>
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
#include "ui_proxysettingswidget.h"

namespace Digikam
{

class Q_DECL_HIDDEN SystemSettingsWidget::Private
{

public:

    Private() = default;

    QCheckBox*              useHighDpiScalingCheck = nullptr;
    QCheckBox*              useHighDpiPixmapsCheck = nullptr;
    QCheckBox*              disableOpenCLCheck     = nullptr;
    QCheckBox*              enableLoggingCheck     = nullptr;

    QPushButton*            filesDownloadButton    = nullptr;

    FilesDownloader*        filesDownloader        = nullptr;

    Ui::ProxySettingsWidget uiProxySettings;
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

    d->filesDownloadButton    = new QPushButton(i18n("Download required binary data..."), this);

    // Proxy Settings

    QWidget* const proxySettings = new QWidget(this);
    d->uiProxySettings.setupUi(proxySettings);
    proxySettings->setContentsMargins(QMargins(0, 0, 0 ,0));

    // ---

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
    layout->addWidget(proxySettings,             5, 0, 1, 1);
    layout->addWidget(systemNote,                6, 0, 1, 2);
    layout->setContentsMargins(spacing, spacing, spacing, spacing);
    layout->setRowStretch(7, 10);

    connect(d->filesDownloadButton, &QPushButton::pressed,
            this, &SystemSettingsWidget::slotBinaryDownload);
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

    // Proxy Settings

    d->uiProxySettings.kcfg_proxyUrl->setText(system.proxyUrl);
    d->uiProxySettings.kcfg_proxyPort->setValue(system.proxyPort);
    d->uiProxySettings.kcfg_proxyUser->setText(system.proxyUser);
    d->uiProxySettings.kcfg_proxyPass->setText(system.proxyPass);
    d->uiProxySettings.kcfg_proxyType->setCurrentIndex(system.proxyType);
    d->uiProxySettings.kcfg_proxyAuth->setChecked(system.proxyAuth);
}

void SystemSettingsWidget::saveSettings()
{
    SystemSettings system(qApp->applicationName());

    system.useHighDpiScaling = d->useHighDpiScalingCheck->isChecked();
    system.useHighDpiPixmaps = d->useHighDpiPixmapsCheck->isChecked();
    system.enableLogging     = d->enableLoggingCheck->isChecked();
    system.disableOpenCL     = d->disableOpenCLCheck->isChecked();

    system.proxyUrl          = d->uiProxySettings.kcfg_proxyUrl->text();
    system.proxyPort         = d->uiProxySettings.kcfg_proxyPort->value();
    system.proxyType         = d->uiProxySettings.kcfg_proxyType->currentIndex();

    if (d->uiProxySettings.kcfg_proxyAuth->isChecked())
    {
        system.proxyAuth = true;
        system.proxyUser = d->uiProxySettings.kcfg_proxyUser->text();
        system.proxyPass = d->uiProxySettings.kcfg_proxyPass->text();
    }
    else
    {
        system.proxyAuth = false;
    }

    system.saveSettings();
}

void SystemSettingsWidget::slotBinaryDownload()
{
    d->filesDownloader->startDownload();
}

} // namespace Digikam

#include "moc_systemsettingswidget.cpp"
