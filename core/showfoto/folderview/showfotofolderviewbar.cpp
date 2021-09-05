/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : a tool bar for Showfoto folder view
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

#include "showfotofolderviewbar.h"

// Qt includes

#include <QDir>
#include <QLayout>
#include <QToolButton>
#include <QPixmap>
#include <QLineEdit>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewBar::Private
{

public:

    explicit Private()
      : previousBtn (nullptr),
        nextBtn     (nullptr),
        upBtn       (nullptr),
        homeBtn     (nullptr),
        loadBtn     (nullptr),
        pathEdit    (nullptr)
    {
    }

    QToolButton*  previousBtn;
    QToolButton*  nextBtn;
    QToolButton*  upBtn;
    QToolButton*  homeBtn;
    QToolButton*  loadBtn;
    QLineEdit*    pathEdit;
};

ShowfotoFolderViewBar::ShowfotoFolderViewBar(QWidget* const parent)
    : DVBox(parent),
      d    (new Private)
{
    setContentsMargins(QMargins());

    DHBox* const btnBox = new DHBox(this);

    d->previousBtn      = new QToolButton(btnBox);
    d->previousBtn->setFocusPolicy(Qt::NoFocus);
    d->previousBtn->setIcon(QIcon::fromTheme(QLatin1String("go-previous")));
    d->previousBtn->setToolTip(i18nc("@info", "Go to previous place in folder-view hierarchy"));

    connect(d->previousBtn, SIGNAL(clicked()),
            this, SIGNAL(signalGoPrevious()));

    d->nextBtn        = new QToolButton(btnBox);
    d->nextBtn->setFocusPolicy(Qt::NoFocus);
    d->nextBtn->setIcon(QIcon::fromTheme(QLatin1String("go-next")));
    d->nextBtn->setToolTip(i18nc("@info", "Go to next place in folder-view hierarchy"));

    connect(d->nextBtn, SIGNAL(clicked()),
            this, SIGNAL(signalGoNext()));

    d->upBtn          = new QToolButton(btnBox);
    d->upBtn->setFocusPolicy(Qt::NoFocus);
    d->upBtn->setIcon(QIcon::fromTheme(QLatin1String("go-up")));
    d->upBtn->setToolTip(i18nc("@info", "Go up in folder-view hierarchy"));

    connect(d->upBtn, SIGNAL(clicked()),
            this, SIGNAL(signalGoUp()));

    d->homeBtn        = new QToolButton(btnBox);
    d->homeBtn->setFocusPolicy(Qt::NoFocus);
    d->homeBtn->setIcon(QIcon::fromTheme(QLatin1String("go-home")));
    d->homeBtn->setToolTip(i18nc("@info", "Back to home directory"));

    connect(d->homeBtn, SIGNAL(clicked()),
            this, SIGNAL(signalGoHome()));

    QWidget* const space = new QWidget(btnBox);
    btnBox->setStretchFactor(space, 10);

    d->loadBtn        = new QToolButton(btnBox);
    d->loadBtn->setFocusPolicy(Qt::NoFocus);
    d->loadBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
    d->loadBtn->setToolTip(i18nc("@info", "Load Contents to Editor"));

    connect(d->loadBtn, SIGNAL(clicked()),
            this, SIGNAL(signalLoadContents()));

    d->pathEdit       = new QLineEdit(this);
    d->pathEdit->setClearButtonEnabled(true);
    d->pathEdit->setWhatsThis(i18nc("@info", "Enter the customized folder-view path"));

    connect(d->pathEdit, SIGNAL(returnPressed()),
            this, SLOT(slotCustomPathChanged()));
}

ShowfotoFolderViewBar::~ShowfotoFolderViewBar()
{
    delete d;
}

void ShowfotoFolderViewBar::setCurrentPath(const QString& path)
{
    d->pathEdit->setText(path);
    QDir dir(path);
    d->upBtn->setEnabled(dir.cdUp());
}

void ShowfotoFolderViewBar::slotCustomPathChanged()
{
    emit signalCustomPathChanged(d->pathEdit->text());
}

void ShowfotoFolderViewBar::slotPreviousEnabled(bool b)
{
    d->previousBtn->setEnabled(b);
}

void ShowfotoFolderViewBar::slotNextEnabled(bool b)
{
    d->nextBtn->setEnabled(b);
}

} // namespace ShowFoto
