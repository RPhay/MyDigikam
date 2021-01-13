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
#include <QLocale>
#include <QProcess>
#include <QDialogButtonBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "onlineversionchecker.h"
#include "onlineversiondwnl.h"
#include "dfileoperations.h"
#include "dxmlguiwindow.h"

namespace Digikam
{

class Q_DECL_HIDDEN OnlineVersionDlg::Private
{
public:

    explicit Private()
      : preRelease(false),
        bar       (nullptr),
        label     (nullptr),
        logo      (nullptr),
        buttons   (nullptr),
        checker   (nullptr),
        dwnloader (nullptr)
    {
    }

    bool                  preRelease;

    QString               curVersion;
    QString               newVersion;       ///< For stable => version IDs ; for pre-release => build ISO date.
    QProgressBar*         bar;
    QLabel*               label;
    QLabel*               logo;
    QDialogButtonBox*     buttons;

    OnlineVersionChecker* checker;
    OnlineVersionDwnl*    dwnloader;
};

OnlineVersionDlg::OnlineVersionDlg(QWidget* const parent,
                                   const QString& version,
                                   bool checkPreRelease)
    : QDialog(parent),
      d      (new Private)
{
    setModal(true);

    d->curVersion = version;
    d->preRelease = checkPreRelease;
    d->checker    = new OnlineVersionChecker(this, d->preRelease);
    d->checker->setCurrentVersion(d->curVersion);
    d->dwnloader  = new OnlineVersionDwnl(this, d->preRelease);

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
    d->label                = new QLabel(page);

    if (d->preRelease)
    {
        setWindowTitle(i18n("Online Version Checker - Pre-Release"));
        d->label->setText(i18n("Check for new pre-release version available, please wait...\n\n"
                               "Note: from Setup/Misc panel, you can switch to check for stable release only.\n"
                               "These versions are safe to use in production."));
    }
    else
    {
        setWindowTitle(i18n("Online Version Checker - Stable Version"));
        d->label->setText(i18n("Check for new stable version available, please wait...\n\n"
                               "Note: from Setup/Misc panel, you can switch to check for weekly pre-release.\n"
                               "These versions are dedicated to test quickly new features but can includes bugs.\n"
                               "It's not recommended to use pre-release in production, unless you know what you are doing."));
    }

    d->logo                = new QLabel(page);

    if (QApplication::applicationName() == QLatin1String("digikam"))
    {
        d->logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(48, 48)));
    }
    else
    {
        d->logo->setPixmap(QIcon::fromTheme(QLatin1String("showfoto")).pixmap(QSize(48, 48)));
    }

    d->bar                 = new QProgressBar(page);
    d->bar->setMaximum(0);
    d->bar->setMinimum(0);
    d->bar->setValue(0);

    d->buttons             = new QDialogButtonBox(QDialogButtonBox::Help | QDialogButtonBox::Apply | QDialogButtonBox::Cancel, page);
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

    connect(d->buttons->button(QDialogButtonBox::Help), SIGNAL(clicked()),
            this, SLOT(slotHelp()));

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

    if (d->preRelease)
    {
        d->label->setText(i18n("Current %1 pre-release date is %2.\n"
                               "New pre-release built on %3 is available.\n"
                               "Press \"Download\" to get the file...",
                               qApp->applicationName(),
                               QLocale().toString(digiKamBuildDate(), QLocale::ShortFormat),
                               QLocale().toString(QDateTime::fromString(version, Qt::ISODate), QLocale::ShortFormat)));
    }
    else
    {
        d->label->setText(i18n("Current %1 version is %2\n"
                               "New version %3 is available.\n"
                               "Press \"Download\" to get the file...",
                               qApp->applicationName(),
                               d->curVersion,
                               version));
    }
}

void OnlineVersionDlg::slotNewVersionCheckError(const QString& error)
{
    d->bar->hide();
    d->buttons->button(QDialogButtonBox::Apply)->setVisible(false);
    d->buttons->button(QDialogButtonBox::Cancel)->setText(i18n("Close"));
    d->buttons->button(QDialogButtonBox::Cancel)->setIcon(QIcon::fromTheme(QLatin1String("close")));

    if (error.isEmpty())
    {
        if (d->preRelease)
        {
            d->label->setText(i18n("Your software is up-to-date.\n"
                                   "%1 built on %2 is the most recent version available.",
                                   qApp->applicationName(),
                                   QLocale().toString(digiKamBuildDate(), QLocale::ShortFormat)));
        }
        else
        {
            d->label->setText(i18n("Your software is up-to-date.\n"
                                   "%1 %2 is the most recent version available.",
                                   qApp->applicationName(),
                                   QString::fromLatin1(digikam_version_short)));
        }
    }
    else
    {
        d->label->setText(i18n("Error while trying to check for new %1 version:\n\"%2\"",
                               qApp->applicationName(),
                               error));
    }
}

void OnlineVersionDlg::slotDownloadInstaller()
{
    d->bar->setMaximum(1);
    d->bar->setMinimum(0);
    d->bar->setValue(0);

    if (d->preRelease)
    {
        d->label->setText(i18n("Downloading new %1 built on %2 in progress, please wait...",
                               qApp->applicationName(),
                               QLocale().toString(QDateTime::fromString(d->newVersion, Qt::ISODate), QLocale::ShortFormat)));
    }
    else
    {
        d->label->setText(i18n("Downloading new %1 version %2 in progress, please wait...",
                               qApp->applicationName(),
                               d->newVersion));
    }

    d->buttons->button(QDialogButtonBox::Apply)->setEnabled(false);
    d->bar->show();

    if (d->preRelease)
    {
        d->dwnloader->startDownload(d->checker->preReleaseFileName());
    }
    else
    {
        d->dwnloader->startDownload(d->newVersion);
    }
}

void OnlineVersionDlg::slotDownloadError(const QString& error)
{
    d->bar->hide();

    if (error.isEmpty())        // empty error want mean a complete download.
    {

#ifdef Q_OS_LINUX

        d->buttons->button(QDialogButtonBox::Apply)->setText(i18n("Open..."));
        d->buttons->button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QLatin1String("system-file-manager")));
        d->buttons->button(QDialogButtonBox::Apply)->setEnabled(true);

        disconnect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()), 0, 0);

        connect(d->buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
                this, SLOT(slotOpenInFileManager()));

        if (d->preRelease)
        {
            d->label->setText(i18n("The new %1 built on %2 have been downloaded at:\n"
                                   "%3\n"
                                   "Press \"Open\" to show the bundle in file-manager...",
                                   qApp->applicationName(),
                                   QLocale().toString(QDateTime::fromString(d->newVersion, Qt::ISODate), QLocale::ShortFormat),
                                   d->dwnloader->downloadedPath()));
        }
        else
        {
            d->label->setText(i18n("The new %1 version %2 have been downloaded at:\n"
                                   "%3\n"
                                   "Press \"Open\" to show the bundle in file-manager...",
                                   qApp->applicationName(),
                                   d->newVersion,
                                   d->dwnloader->downloadedPath()));
        }

#else // Windows and macOS

        d->buttons->button(QDialogButtonBox::Apply)->setEnabled(true);
        d->buttons->button(QDialogButtonBox::Apply)->setText(i18n("Install..."));
        d->buttons->button(QDialogButtonBox::Apply)->setIcon(QIcon::fromTheme(QLatin1String("run-install")));

        if (d->preRelease)
        {
            d->label->setText(i18n("The new %1 built on %2 have been downloaded at:\n"
                                   "%3\n"
                                   "Press \"Install\" to close current session and upgrade...",
                                   qApp->applicationName(),
                                   QLocale().toString(QDateTime::fromString(d->newVersion, Qt::ISODate), QLocale::ShortFormat),
                                   d->dwnloader->downloadedPath()));
        }
        else
        {
            d->label->setText(i18n("The new %1 version %2 have been downloaded at:\n"
                                   "%3\n"
                                   "Press \"Install\" to close current session and upgrade...",
                                   qApp->applicationName(),
                                   d->newVersion,
                                   d->dwnloader->downloadedPath()));
        }

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

        if (d->preRelease)
        {
            d->label->setText(i18n("Error while trying to download %1 built on %2:\n\"%3\"",
                                   qApp->applicationName(),
                                   QLocale().toString(QDateTime::fromString(d->newVersion, Qt::ISODate), QLocale::ShortFormat),
                                   error));
        }
        else
        {
            d->label->setText(i18n("Error while trying to download %1 version %2:\n\"%3\"",
                                   qApp->applicationName(),
                                   d->newVersion,
                                   error));
        }
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

void OnlineVersionDlg::slotHelp()
{
    DXmlGuiWindow::openHandbook();
}

} // namespace Digikam
