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

#include <QToolButton>
#include <QPixmap>
#include <QButtonGroup>
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
      : simplifiedBtn(nullptr),
        detailledBtn (nullptr),
        upBtn        (nullptr),
        homeBtn      (nullptr),
        btnsBox      (nullptr),
        pathEdit     (nullptr)
    {
    }

    QToolButton*  simplifiedBtn;
    QToolButton*  detailledBtn;
    QToolButton*  upBtn;
    QToolButton*  homeBtn;
    QButtonGroup* btnsBox;
    QLineEdit*    pathEdit;
};

ShowfotoFolderViewBar::ShowfotoFolderViewBar(QWidget* const parent)
    : DHBox(parent),
      d    (new Private)
{
    setContentsMargins(QMargins());

    d->btnsBox        = new QButtonGroup(this);
    d->btnsBox->setExclusive(true);

    d->simplifiedBtn  = new QToolButton(this);
    d->simplifiedBtn->setCheckable(true);
    d->simplifiedBtn->setChecked(true);
    d->simplifiedBtn->setFocusPolicy(Qt::NoFocus);
    d->simplifiedBtn->setIconSize(QSize(22, 22));
    d->simplifiedBtn->setIcon(QIcon::fromTheme(QLatin1String("view-list-text")));
    d->simplifiedBtn->setToolTip(i18nc("@info", "Folder-view mode simplified"));
    d->btnsBox->addButton(d->simplifiedBtn, FolderViewSimplified);

    d->detailledBtn   = new QToolButton(this);
    d->detailledBtn->setCheckable(true);
    d->detailledBtn->setChecked(false);
    d->detailledBtn->setFocusPolicy(Qt::NoFocus);
    d->detailledBtn->setIconSize(QSize(22, 22));
    d->detailledBtn->setIcon(QIcon::fromTheme(QLatin1String("view-list-details")));
    d->detailledBtn->setToolTip(i18nc("@info", "Folder-view mode detailled"));
    d->btnsBox->addButton(d->detailledBtn, FolderViewDetailled);

    connect(d->btnsBox, SIGNAL(buttonReleased(int)),
            this, SIGNAL(signalFolderViewModeChanged(int)));

    d->upBtn       = new QToolButton(this);
    d->upBtn->setCheckable(false);
    d->upBtn->setFocusPolicy(Qt::NoFocus);
    d->upBtn->setIconSize(QSize(22, 22));
    d->upBtn->setIcon(QIcon::fromTheme(QLatin1String("go-up")));
    d->upBtn->setToolTip(i18nc("@info", "Go up in folder-view hierarchy"));

    connect(d->upBtn, SIGNAL(clicked()),
            this, SIGNAL(signalGoUp()));

    d->homeBtn       = new QToolButton(this);
    d->homeBtn->setCheckable(false);
    d->homeBtn->setFocusPolicy(Qt::NoFocus);
    d->homeBtn->setIconSize(QSize(22, 22));
    d->homeBtn->setIcon(QIcon::fromTheme(QLatin1String("go-home")));
    d->homeBtn->setToolTip(i18nc("@info", "Back to home directory"));

    connect(d->homeBtn, SIGNAL(clicked()),
            this, SIGNAL(signalGoHome()));

    d->pathEdit = new QLineEdit(this);
    d->pathEdit->setClearButtonEnabled(true);
    d->pathEdit->setWhatsThis(i18nc("@info", "Enter the customized folder-view path"));
    setStretchFactor(d->pathEdit, 10);

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
}

void ShowfotoFolderViewBar::slotCustomPathChanged()
{
    emit signalCustomPathChanged(d->pathEdit->text());
}

void ShowfotoFolderViewBar::setFolderViewMode(int mode)
{
    if (mode == FolderViewDetailled)
    {
        d->detailledBtn->setChecked(true);
    }
    else
    {
        d->simplifiedBtn->setChecked(true);
    }

    emit signalFolderViewModeChanged(mode);
}

int ShowfotoFolderViewBar::folderViewMode() const
{
    if (d->simplifiedBtn->isChecked())
    {
        return FolderViewSimplified;
    }

    return FolderViewDetailled;
}

} // namespace ShowFoto
