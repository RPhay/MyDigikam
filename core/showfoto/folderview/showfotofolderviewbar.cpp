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
#include <QSlider>
#include <QLayout>
#include <QToolButton>
#include <QPixmap>
#include <QLineEdit>
#include <QActionGroup>
#include <QMenu>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "showfotofolderviewiconprovider.h"
#include "showfotofolderviewlist.h"

namespace ShowFoto
{

class Q_DECL_HIDDEN ShowfotoFolderViewBar::Private
{

public:

    explicit Private()
      : previousBtn    (nullptr),
        nextBtn        (nullptr),
        upBtn          (nullptr),
        homeBtn        (nullptr),
        iconSizeSlider (nullptr),
        optionsBtn     (nullptr),
        optionsMenu    (nullptr),
        shortAction    (nullptr),
        detailledAction(nullptr),
        loadBtn        (nullptr),
        pathEdit       (nullptr)
    {
    }

    QToolButton*    previousBtn;
    QToolButton*    nextBtn;
    QToolButton*    upBtn;
    QToolButton*    homeBtn;
    QSlider*        iconSizeSlider;
    QToolButton*    optionsBtn;
    QMenu*          optionsMenu;
    QAction*        shortAction;
    QAction*        detailledAction;
    QToolButton*    loadBtn;
    QLineEdit*      pathEdit;
    QList<QAction*> actionsList;
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

    d->nextBtn               = new QToolButton(btnBox);
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

    d->upBtn                 = new QToolButton(btnBox);
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

    d->homeBtn               = new QToolButton(btnBox);
    d->homeBtn->setDefaultAction(btnAction);
    d->homeBtn->setFocusPolicy(Qt::NoFocus);

    // ---

    d->iconSizeSlider        = new QSlider(Qt::Horizontal, btnBox);
    d->iconSizeSlider->setRange(16, ShowfotoFolderViewIconProvider::maxIconSize());
    d->iconSizeSlider->setSingleStep(10);
    d->iconSizeSlider->setValue(32);
    d->iconSizeSlider->setFocusPolicy(Qt::NoFocus);
    btnBox->setStretchFactor(d->iconSizeSlider, 10);

    connect(d->iconSizeSlider, SIGNAL(valueChanged(int)),
            this, SLOT(slotIconSizeChanged(int)));

    slotIconSizeChanged(32);

    // ---

    d->optionsBtn              = new QToolButton(btnBox);
    d->optionsBtn->setToolTip(i18nc("@info: folder-view", "Folder-View Options"));
    d->optionsBtn->setIcon(QIcon::fromTheme(QLatin1String("configure")));
    d->optionsBtn->setPopupMode(QToolButton::InstantPopup);
    d->optionsBtn->setWhatsThis(i18nc("@info: folder-view", "Selection options to render folder-view."));

    d->optionsMenu             = new QMenu(d->optionsBtn);
    QActionGroup* const optGrp = new QActionGroup(this);

    d->shortAction             = d->optionsMenu->addAction(i18nc("@action:inmenu", "Short View"));
    d->shortAction->setCheckable(true);
    optGrp->addAction(d->shortAction);

    d->detailledAction         = d->optionsMenu->addAction(i18nc("@action:inmenu", "Detailled-View"));
    d->detailledAction->setCheckable(true);
    optGrp->addAction(d->detailledAction);

    optGrp->setExclusive(true);
    d->optionsBtn->setMenu(d->optionsMenu);

    connect(d->optionsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(slotOptionsChanged(QAction*)));

    // ---

    btnAction                = new QAction(this);
    btnAction->setObjectName(QLatin1String("LoadContents"));
    btnAction->setIcon(QIcon::fromTheme(QLatin1String("media-playlist-normal")));
    btnAction->setText(i18nc("action", "Load Contents"));
    btnAction->setToolTip(i18nc("@info", "Load Contents to Editor"));

    connect(btnAction, SIGNAL(triggered(bool)),
            this, SIGNAL(signalLoadContents()));

    d->actionsList << btnAction;

    d->loadBtn               = new QToolButton(btnBox);
    d->loadBtn->setDefaultAction(btnAction);
    d->loadBtn->setFocusPolicy(Qt::NoFocus);

    connect(d->loadBtn, SIGNAL(clicked()),
            this, SIGNAL(signalLoadContents()));

    // ---

    d->pathEdit              = new QLineEdit(this);
    d->pathEdit->setClearButtonEnabled(true);
    d->pathEdit->setPlaceholderText(i18nc("@info: folder-view path edit", "Enter local path here..."));
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

void ShowfotoFolderViewBar::setFolderViewMode(int mode)
{
    if (mode == ShowfotoFolderViewList::ShortView)
    {
        d->shortAction->setChecked(true);
    }
    else
    {
        d->detailledAction->setChecked(true);
    }
}

int ShowfotoFolderViewBar::folderViewMode() const
{
    if (d->shortAction->isChecked())
    {
        return ShowfotoFolderViewList::ShortView;
    }

    return ShowfotoFolderViewList::DetailledView;
}

void ShowfotoFolderViewBar::setCurrentPath(const QString& path)
{
    d->pathEdit->setText(path);
    QDir dir(path);
    d->upBtn->setEnabled(dir.cdUp());
}

void ShowfotoFolderViewBar::setIconSize(int size)
{
    d->iconSizeSlider->setValue(size);
}

int ShowfotoFolderViewBar::iconSize() const
{
    return d->iconSizeSlider->value();
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

void ShowfotoFolderViewBar::slotIconSizeChanged(int size)
{
    d->iconSizeSlider->setToolTip(i18nc("@info", "Icon Size: %1", size));

    emit signalIconSizeChanged(size);
}

void ShowfotoFolderViewBar::slotOptionsChanged(QAction* action)
{
    int mode;

    if      (action == d->shortAction)
    {
        mode = ShowfotoFolderViewList::ShortView;
    }
    else if (action == d->detailledAction)
    {
        mode = ShowfotoFolderViewList::DetailledView;
    }

    emit signalViewModeChanged(mode);
}

} // namespace ShowFoto
