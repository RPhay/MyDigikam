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
#include <QApplication>
#include <QPushButton>
#include <QProgressBar>
#include <QDesktopServices>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "onlineversionchecker.h"
#include "digikam_version.h"

namespace Digikam
{

class Q_DECL_HIDDEN OnlineVersionDlg::Private
{
public:

    explicit Private()
      : btn    (nullptr),
        bar    (nullptr),
        checker(nullptr)
    {
    }

    QPushButton*          btn;
    QProgressBar*         bar;
    OnlineVersionChecker* checker;
};

OnlineVersionDlg::OnlineVersionDlg(QWidget* const parent)
    : QProgressDialog(parent),
      d              (new Private)
{
    d->checker = new OnlineVersionChecker(this);

    connect(d->checker, SIGNAL(signalNewVersionAvailable(QString)),
            this, SLOT(slotNewVersionAvailable(QString)));

    connect(d->checker, SIGNAL(signalNewVersionCheckError(QString)),
            this, SLOT(slotNewVersionCheckError(QString)));

    d->btn = new QPushButton(i18n("Cancel"), this);
    d->bar = new QProgressBar(this);

    setCancelButton(d->btn);
    setBar(d->bar);
    setLabelText(i18n("Check for new version available, please wait..."));
    setMinimumDuration(0);
    setModal(true);
    setAutoClose(false);

    setMaximum(0);
    setMinimum(0);
    setValue(0);

    d->checker->checkForNewVersion();
}

OnlineVersionDlg::~OnlineVersionDlg()
{
    d->checker->cancelCheck();
    delete d;
}

void OnlineVersionDlg::slotNewVersionAvailable(const QString& version)
{
    d->bar->hide();
    d->btn->setText(i18n("Open in Browser..."));

    connect(d->btn, SIGNAL(pressed()),
            this, SLOT(slotOpenInBrowser()));

    setLabelText(i18n("A new version %1 is available.\nPress \"Open in Browser\" to access to download page...", version));
}

void OnlineVersionDlg::slotNewVersionCheckError(const QString& error)
{
    d->bar->hide();
    d->btn->setText(i18n("Close"));

    if (error.isEmpty())
    {
        setLabelText(i18n("Your Software is up-to-date.\n%1 %2 is the most recent version available.",
                     qApp->applicationName(), QString::fromLatin1(digikam_version_short)));
    }
    else
    {
        setLabelText(i18n("Error while trying to check for new version:\n\"%1\"", error));
    }
}

void OnlineVersionDlg::slotOpenInBrowser()
{
    QDesktopServices::openUrl(QUrl(QLatin1String("https://www.digikam.org/download")));
}

} // namespace Digikam
