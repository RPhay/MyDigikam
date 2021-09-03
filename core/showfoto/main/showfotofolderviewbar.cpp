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
        btnsBox      (nullptr)
    {
    }

    QToolButton*  simplifiedBtn;
    QToolButton*  detailledBtn;
    QButtonGroup* btnsBox;
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
    d->simplifiedBtn->setToolTip(i18nc("@info", "Folder view mode simplified"));
    d->btnsBox->addButton(d->simplifiedBtn, FolderViewSimplified);

    d->detailledBtn   = new QToolButton(this);
    d->detailledBtn->setCheckable(true);
    d->detailledBtn->setChecked(false);
    d->detailledBtn->setFocusPolicy(Qt::NoFocus);
    d->detailledBtn->setIconSize(QSize(22, 22));
    d->detailledBtn->setIcon(QIcon::fromTheme(QLatin1String("view-list-details")));
    d->detailledBtn->setToolTip(i18nc("@info", "Folder view mode detailled"));
    d->btnsBox->addButton(d->detailledBtn, FolderViewDetailled);

    connect(d->btnsBox, SIGNAL(buttonReleased(int)),
            this, SIGNAL(signalFolderViewModeChanged(int)));

    QWidget* const space = new QWidget(this);
    setStretchFactor(space, 10);
}

ShowfotoFolderViewBar::~ShowfotoFolderViewBar()
{
    delete d;
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
