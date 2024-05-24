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
#include "digikam_config.h"
#include "digikam_globals.h"
#include "systemsettings.h"
#include "filesdownloader.h"
#include "ui_proxysettingswidget.h"

namespace Digikam
{

class Q_DECL_HIDDEN SystemSettingsWidget::Private
{

public:

    Private() = default;

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    QCheckBox*              useHighDpiScalingCheck = nullptr;
    QCheckBox*              useHighDpiPixmapsCheck = nullptr;

#endif

    QCheckBox*              softwareOpenGLCheck    = nullptr;
    QCheckBox*              disableOpenCLCheck     = nullptr;
    QCheckBox*              enableLoggingCheck     = nullptr;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    QCheckBox*              disableHWConvCheck     = nullptr;
    QComboBox*              videoBackendCBox       = nullptr;

#endif

    QPushButton*            filesDownloadButton    = nullptr;

    FilesDownloader*        filesDownloader        = nullptr;

    Ui::ProxySettingsWidget uiProxySettings;
};

SystemSettingsWidget::SystemSettingsWidget(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing         = layoutSpacing();


    QGridLayout* const layout = new QGridLayout(this);

    d->filesDownloader        = new FilesDownloader(this);

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    d->useHighDpiScalingCheck = new QCheckBox(i18n("Use high DPI scaling from the screen factor"), this);
    d->useHighDpiPixmapsCheck = new QCheckBox(i18n("Use pixmaps with high DPI resolution"), this);

#endif

    d->softwareOpenGLCheck    = new QCheckBox(i18n("Force use of software OpenGL rendering"), this);
    d->disableOpenCLCheck     = new QCheckBox(i18n("Disable hardware acceleration OpenCL"), this);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    d->disableHWConvCheck     = new QCheckBox(i18n("Fix AMD-GPU vdeo decoding issue"), this);

    QLabel* const videoLabel  = new QLabel(i18n("Used decoding backend for video:"), this);

    d->videoBackendCBox       = new QComboBox(this);
    d->videoBackendCBox->addItem(i18n("FFmpeg (Default)"), QLatin1String("ffmpeg"));
    d->videoBackendCBox->addItem(i18n("Windows (System)"), QLatin1String("windows"));

#endif

    d->enableLoggingCheck     = new QCheckBox(i18n("Enable internal debug logging"), this);
    d->filesDownloadButton    = new QPushButton(i18n("Download required binary data..."), this);

    // Proxy Settings

    QWidget* const proxySettings = new QWidget(this);
    d->uiProxySettings.setupUi(proxySettings);
    proxySettings->setContentsMargins(QMargins());

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

    int row = 0;

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    layout->addWidget(d->useHighDpiScalingCheck, row++, 0, 1, 2);
    layout->addWidget(d->useHighDpiPixmapsCheck, row++, 0, 1, 2);

#endif

    layout->addWidget(d->softwareOpenGLCheck,    row++, 0, 1, 2);
    layout->addWidget(d->disableOpenCLCheck,     row++, 0, 1, 2);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    layout->addWidget(d->disableHWConvCheck,     row++, 0, 1, 2);
    layout->addWidget(videoLabel,                row,   0, 1, 1);
    layout->addWidget(d->videoBackendCBox,       row++, 1, 1, 1); 

#endif

    layout->addWidget(d->enableLoggingCheck,     row++, 0, 1, 2);
    layout->addWidget(d->filesDownloadButton,    row++, 0, 1, 2);
    layout->addWidget(proxySettings,             row++, 0, 1, 2);
    layout->addWidget(systemNote,                row++, 0, 1, 2);
    layout->setContentsMargins(spacing, spacing, spacing, spacing);
    layout->setRowStretch(row, 10);

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

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    d->useHighDpiScalingCheck->setChecked(system.useHighDpiScaling);
    d->useHighDpiPixmapsCheck->setChecked(system.useHighDpiPixmaps);

#endif

    d->softwareOpenGLCheck->setChecked(system.softwareOpenGL);
    d->enableLoggingCheck->setChecked(system.enableLogging);
    d->disableOpenCLCheck->setChecked(system.disableOpenCL);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    d->disableHWConvCheck->setChecked(system.disableHWConv);
    d->videoBackendCBox->setCurrentIndex(d->videoBackendCBox->findData(system.videoBackend));

#endif

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

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

    system.useHighDpiScaling = d->useHighDpiScalingCheck->isChecked();
    system.useHighDpiPixmaps = d->useHighDpiPixmapsCheck->isChecked();

#endif

    system.softwareOpenGL    = d->softwareOpenGLCheck->isChecked();
    system.enableLogging     = d->enableLoggingCheck->isChecked();
    system.disableOpenCL     = d->disableOpenCLCheck->isChecked();

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)) && defined(Q_OS_WIN)

    system.disableHWConv     = d->disableHWConvCheck->isChecked();
    system.videoBackend      = d->videoBackendCBox->currentData().toString();

#endif

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
