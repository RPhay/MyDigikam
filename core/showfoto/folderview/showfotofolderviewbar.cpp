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

    QToolButton*            previousBtn;
    QToolButton*            nextBtn;
    QToolButton*            upBtn;
    QToolButton*            homeBtn;
    QToolButton*            loadBtn;
    QLineEdit*              pathEdit;
    QList<QAction*>         actionsList;
};

ShowfotoFolderViewBar::ShowfotoFolderViewBar(QWidget* const parent)
    : DVBox(parent),
      d    (new Private)
{
    setContentsMargins(QMargins());

    DHBox* const btnBox      = new DHBox(this);
    QAction* btnAction       = nullptr;

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("GoPrevious"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("go-previous")));
    btnAction->setText(i18nc("action", "Go to Previous"));
    btnAction->setToolTip(i18nc("@info", "Go to previous place in folder-view hierarchy"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalGoPrevious()));

    d->actionsList << btnAction;

    d->previousBtn           = new QToolButton(btnBox);
    d->previousBtn->setDefaultAction(btnAction);
    d->previousBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("GoNext"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("go-next")));
    btnAction->setText(i18nc("action", "Go to Next"));
    btnAction->setToolTip(i18nc("@info", "Go to next place in folder-view hierarchy"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalGoNext()));

    d->actionsList << btnAction;

    d->nextBtn        = new QToolButton(btnBox);
    d->nextBtn->setDefaultAction(btnAction);
    d->nextBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("GoUp"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("go-up")));
    btnAction->setText(i18nc("action", "Go Up"));
    btnAction->setToolTip(i18nc("@info", "Go up in folder-view hierarchy"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalGoUp()));

    d->actionsList << btnAction;

    d->upBtn          = new QToolButton(btnBox);
    d->upBtn->setDefaultAction(btnAction);
    d->upBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("GoHome"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("go-home")));
    btnAction->setText(i18nc("action", "Go Home"));
    btnAction->setToolTip(i18nc("@info", "Go to home directory"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalGoHome()));

    d->actionsList << btnAction;

    d->homeBtn        = new QToolButton(btnBox);
    d->homeBtn->setDefaultAction(btnAction);
    d->homeBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    QWidget* const space = new QWidget(btnBox);
    btnBox->setStretchFactor(space, 10);

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("LoadContents"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
    btnAction->setText(i18nc("action", "Load Contents"));
    btnAction->setToolTip(i18nc("@info", "Load Contents to Editor"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalLoadContents()));

    d->actionsList << btnAction;

    d->loadBtn        = new QToolButton(btnBox);
    d->loadBtn->setDefaultAction(btnAction);
    d->loadBtn->setFocusPolicy(Qt::NoFocus);

    connect(d->loadBtn, SIGNAL(clicked()),
            this, SIGNAL(signalLoadContents()));

    // ---

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

QAction* ShowfotoFolderViewBar::toolBarAction(const QString& name) const
{
    foreach (QAction* const act, d->actionsList)
    {
        if (act && (act->objectName() == name))
        {
            return act;
        }
    }

    return nullptr;
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
