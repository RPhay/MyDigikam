/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-05
 * Description : Online version dialog.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "onlineversiondlg.h"

// Qt includes

#include <QUrl>
#include <QList>
#include <QFile>
#include <QFileDevice>
#include <QStyle>
#include <QIcon>
#include <QLabel>
#include <QImage>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QPushButton>
#include <QProgressBar>
#include <QProcess>
#include <QDialogButtonBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "onlineversionchecker.h"
#include "onlineversiondwnl.h"
#include "dfileoperations.h"

namespace Digikam
{

class Q_DECL_HIDDEN OnlineVersionDlg::Private
{
public:

    explicit Private()
      : bar      (nullptr),
        label    (nullptr),
        logo     (nullptr),
        buttons  (nullptr),
        checker  (nullptr),
        dwnloader(nullptr)
    {
    }

    QString               newVersion;
    QProgressBar*         bar;
    QLabel*               label;
    QLabel*               logo;
    QDialogButtonBox*     buttons;

    OnlineVersionChecker* checker;
    OnlineVersionDwnl*    dwnloader;
};

OnlineVersionDlg::OnlineVersionDlg(QWidget* const parent, const QString& version)
    : QDialog(parent),
      d      (new Private)
{
    setModal(true);
    setWindowTitle(i18n("Online Version Checker"));

    d->checker   = new OnlineVersionChecker(this);
    d->checker->setCurrentVersion(version);
    d->dwnloader = new OnlineVersionDwnl(this);

    connect(d->checker, SIGNAL(signalNewVersionAvailable(QString)),
            this, SLOT(slotNewVersionAvailable(QString)));

    connect(d->checker, SIGNAL(signalNewVersionCheckError(QString)),
            this, SLOT(slotNewVersionCheckError(QString)));

    connect(d->dwnloader, SIGNAL(signalDownloadError(QString)),
            this, SLOT(slotDownloadError(QString)));

    connect(d->dwnloader, SIGNAL(signalDownloadProgress(qint64,qint64)),
            this, SLOT(slotDownloadProgress(qint64,qint64)));

    QWidget* const page     = new QWidget(this);
    QGridLayout* const grid = new QGridLayout(page);

    d->label   = new QLabel(page);
    d->label->setText(i18n("Check for new version available, please wait..."));

    d->logo    = new QLabel(page);

    if (QApplication::applicationName() == QLatin1String("digikam"))
    {
        d->logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(48,48)));
    }
    else
    {
        d->logo->setPixmap(QIcon::fromTheme(QLatin1String("showfoto")).pixmap(QSize(48,48)));
    }

    d->bar     = new QProgressBar(page);
    d->bar->setMaximum(0);
    d->bar->setMinimum(0);
    d->bar->setValue(0);

    d->buttons = new QDialogButtonBox(QDialogButtonBox::Apply | QDialogButtonBox::Cancel, page);
    d->buttons->button(QDialogButtonBox::Cancel)->setDefault(true);
    d->buttons->button(QDialogButtonBox::Apply)->setVisible(false);

    grid->addWidget(d->logo,  0, 0, 3, 1);
    grid->addWidget(d->label, 0, 1, 1, 2);
    grid->addWidget(d->bar,   1, 1, 1, 2);
    grid->setSpacing(style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    grid->setContentsMargins(QMargins());
    grid->setColumnStretch(2, 10);

    QVBoxLayout* const vbx = new QVBoxLayout(this);
    vbx->addWidget(page);
    vbx->addWidget(d->buttons);
    setLayout(vbx);

    connect(d->buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()),
            this, SLOT(close()));

    adjustSize();

    d->buttons->button(QDialogButtonBox::Apply)->setVisible(false);
    d->buttons->button(QDialogButtonBox::Apply)->setEnabled(false);

    d->checker->checkForNewVersion();
}

OnlineVersionDlg::~OnlineVersionDlg()
{
    d->checker->cancelCheck();
    d->dwnloader->cancelDownload();
    delete d;
}

void OnlineVersionDlg::slotNewVersionAvailable(const QString& version)
{
    d->newVersion = version;
    d->bar->hide();
    d->buttons->button(QDialogButtonBox::Apply)->setVisible(true);
    d->buttons->button(QDialogButtonBox::Apply)->setEnabled(true);

    d->buttons->button(QDialogButtonBox::Apply)->setText(i18n("Download"));
    d->buttons->button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QLatin1String("download")));

    disconnect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), 0, 0);

    connect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
            this, SLOT(slotDownloadInstaller()));

    d->label->setText(i18n("A new version %1 is available.\nPress \"Download\" to get file...", version));
}

void OnlineVersionDlg::slotNewVersionCheckError(const QString& error)
{
    d->bar->hide();
    d->buttons->button(QDialogButtonBox::Apply)->setVisible(false);

    if (error.isEmpty())
    {
        d->label->setText(i18n("Your software is up-to-date.\n%1 %2 is the most recent version available.",
                          qApp->applicationName(), QString::fromLatin1(digikam_version_short)));
    }
    else
    {
        d->label->setText(i18n("Error while trying to check for new version:\n\"%1\"", error));
    }
}

void OnlineVersionDlg::slotDownloadInstaller()
{
    d->bar->setMaximum(1);
    d->bar->setMinimum(0);
    d->bar->setValue(0);

    d->label->setText(i18n("Downloading new software version in progress, please wait..."));
    d->buttons->button(QDialogButtonBox::Apply)->setEnabled(false);
    d->bar->show();
    d->dwnloader->startDownload(d->newVersion);
}

void OnlineVersionDlg::slotDownloadError(const QString& error)
{
    d->bar->hide();

    if (error.isEmpty())
    {

#ifdef Q_OS_LINUX

        d->buttons->button(QDialogButtonBox::Apply)->setText(i18n("Open..."));
        d->buttons->button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QLatin1String("system-file-manager")));
        d->buttons->button(QDialogButtonBox::Apply)->setEnabled(true);

        disconnect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), 0, 0);

        connect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
                this, SLOT(slotOpenInFileManager()));

        d->label->setText(i18n("The new software version is downloaded at:\n"
                               "%1\n"
                               "Press \"Open\" to show it in file-manager...",
                               d->dwnloader->downloadedPath()));

#else // Windows and macOS

        d->buttons->button(QDialogButtonBox::Apply)->setText(i18n("Run..."));
        d->buttons->button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QLatin1String("run-install")));
        d->buttons->button(QDialogButtonBox::Apply)->setEnabled(true);

        d->label->setText(i18n("The new software version is downloaded at:\n"
                               "%1\n"
                               "Press \"Run\" to close your software and continue...",
                               d->dwnloader->downloadedPath()));

        disconnect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), 0, 0);

        connect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
                this, SLOT(slotRunInstaller()));

#endif

    }
    else
    {
        d->buttons->button(QDialogButtonBox::Apply)->setVisible(false);
        d->buttons->button(QDialogButtonBox::Cancel)->setText(i18n("Close"));
        d->buttons->button(QDialogButtonBox::Cancel)->setIcon(QIcon::fromTheme(QLatin1String("close")));

        d->label->setText(i18n("Error while trying to download new software version:\n\"%1\"", error));
    }
}

void OnlineVersionDlg::slotDownloadProgress(qint64 recv, qint64 total)
{
    if (total == 0)
    {
        return;
    }

    d->bar->setMaximum(total);
    d->bar->setMinimum(0);
    d->bar->setValue(recv);
}

void OnlineVersionDlg::slotRunInstaller()
{
    QString path = d->dwnloader->downloadedPath();
    QProcess::startDetached(path, QStringList());
    close();
    qApp->quit();
}

void OnlineVersionDlg::slotOpenInFileManager()
{
    QList<QUrl> lst = QList<QUrl>() << QUrl::fromLocalFile(d->dwnloader->downloadedPath());
    DFileOperations::openInFileManager(lst);
    close();
}

} // namespace Digikam
