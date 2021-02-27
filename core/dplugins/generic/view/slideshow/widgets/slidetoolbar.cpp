/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-10-05
 * Description : a tool bar for slideshow
 *
 * Copyright (C) 2004-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
 * Copyright (C)      2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#include "slidetoolbar.h"

// Qt includes

#include <QApplication>
#include <QActionGroup>
#include <QToolButton>
#include <QPixmap>
#include <QScreen>
#include <QMenu>
#include <QInputDialog>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dpluginaboutdlg.h"
#include "digikam_debug.h"
#include "setupslideshow_dialog.h"

namespace DigikamGenericSlideShowPlugin
{

class Q_DECL_HIDDEN SlideToolBar::Private
{
public:

    explicit Private()
      : playBtn         (nullptr),
        prevBtn         (nullptr),
        nextBtn         (nullptr),
        stopBtn         (nullptr),
        delayBtn        (nullptr),
        setupBtn        (nullptr),
        screenSelectBtn (nullptr),
        currentlyPause  (false),
        configDialog    (nullptr),
        settings        (nullptr)
    {
    }

    QToolButton*          playBtn;
    QToolButton*          prevBtn;
    QToolButton*          nextBtn;
    QToolButton*          stopBtn;
    QToolButton*          delayBtn;
    QToolButton*          setupBtn;
    QToolButton*          screenSelectBtn;

    bool                  currentlyPause;

    SetupSlideShowDialog* configDialog;
    SlideShowSettings*    settings;
};

SlideToolBar::SlideToolBar(SlideShowSettings* const settings, QWidget* const parent)
    : DHBox(parent),
      d    (new Private)
{
    setMouseTracking(true);
    setContentsMargins(QMargins());

    d->settings       = settings;

    d->playBtn        = new QToolButton(this);
    d->prevBtn        = new QToolButton(this);
    d->nextBtn        = new QToolButton(this);
    d->stopBtn        = new QToolButton(this);
    d->delayBtn       = new QToolButton(this);
    d->setupBtn       = new QToolButton(this);

    d->configDialog   = new SetupSlideShowDialog(d->settings, this);

    d->playBtn->setCheckable(true);
    d->playBtn->setChecked(!d->settings->autoPlayEnabled);
    d->playBtn->setFocusPolicy(Qt::NoFocus);
    d->prevBtn->setFocusPolicy(Qt::NoFocus);
    d->nextBtn->setFocusPolicy(Qt::NoFocus);
    d->stopBtn->setFocusPolicy(Qt::NoFocus);
    d->delayBtn->setFocusPolicy(Qt::NoFocus);
    d->setupBtn->setFocusPolicy(Qt::NoFocus);

    QSize s(32, 32);
    d->playBtn->setIconSize(s);
    d->prevBtn->setIconSize(s);
    d->nextBtn->setIconSize(s);
    d->stopBtn->setIconSize(s);
    d->delayBtn->setIconSize(s);
    d->setupBtn->setIconSize(s);

    QString iconString = d->settings->autoPlayEnabled ? QLatin1String("media-playback-pause")
                                                      : QLatin1String("media-playback-start");
    d->playBtn->setIcon(QIcon::fromTheme(iconString));
    d->prevBtn->setIcon(QIcon::fromTheme(QLatin1String("media-skip-backward")));
    d->nextBtn->setIcon(QIcon::fromTheme(QLatin1String("media-skip-forward")));
    d->stopBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-stop")));
    d->delayBtn->setIcon(QIcon::fromTheme(QLatin1String("appointment-new")));
    d->setupBtn->setIcon(QIcon::fromTheme(QLatin1String("systemsettings")));

    int num = qApp->screens().count();

    if (num > 1)
    {
        d->screenSelectBtn      = new QToolButton(this);
        QMenu* const screenMenu = new QMenu(d->screenSelectBtn);
        d->screenSelectBtn->setToolTip(i18n("Switch Screen"));
        d->screenSelectBtn->setIconSize(s);
        d->screenSelectBtn->setIcon(QIcon::fromTheme(QLatin1String("video-display")));
        d->screenSelectBtn->setMenu(screenMenu);
        d->screenSelectBtn->setPopupMode(QToolButton::InstantPopup);
        d->screenSelectBtn->setFocusPolicy(Qt::NoFocus);

        QActionGroup* const group = new QActionGroup(screenMenu);
        group->setExclusive(true);

        for (int i = 0 ; i < num ; ++i)
        {
            QString model      = qApp->screens().at(i)->model();
            QAction* const act = screenMenu->addAction(i18nc("%1 is the screen number (0, 1, ...)", "Screen %1", i) +
                                                       QString::fromUtf8(" (%1)").arg(model.left(model.length() - 1)));
            act->setData(QVariant::fromValue(i));
            act->setCheckable(true);
            group->addAction(act);

            if (i == d->settings->slideScreen)
            {
               act->setChecked(true);
            }
        }

        connect(screenMenu, SIGNAL(triggered(QAction*)),
                this, SLOT(slotScreenSelected(QAction*)));
    }

    connect(d->playBtn, SIGNAL(toggled(bool)),
            this, SLOT(slotPlayBtnToggled()));

    connect(d->nextBtn, SIGNAL(clicked()),
            this, SLOT(slotNexPrevClicked()));

    connect(d->prevBtn, SIGNAL(clicked()),
            this, SLOT(slotNexPrevClicked()));

    connect(d->nextBtn, SIGNAL(clicked()),
            this, SIGNAL(signalNext()));

    connect(d->prevBtn, SIGNAL(clicked()),
            this, SIGNAL(signalPrev()));

    connect(d->stopBtn, SIGNAL(clicked()),
            this, SIGNAL(signalClose()));

    connect(d->delayBtn, SIGNAL(clicked()),
            this, SLOT(slotChangeDelayButtonPressed()));

    connect(d->setupBtn, SIGNAL(clicked()),
            this, SLOT(slotMenuSlideShowConfiguration()));

    connect(d->configDialog, SIGNAL(finished(int)),
            this, SIGNAL(signalUpdateSettings()));

    connect(d->configDialog, SIGNAL(finished(int)),
            this, SLOT(slotConfigurationAccepted()));
}

SlideToolBar::~SlideToolBar()
{
    delete d;
}

bool SlideToolBar::isPaused() const
{
    return d->playBtn->isChecked();
}

void SlideToolBar::pause(bool val)
{
    if (val == isPaused())
    {
        return;
    }

    d->playBtn->setChecked(val);
    slotPlayBtnToggled();
}

void SlideToolBar::setEnabledPlay(bool val)
{
    d->playBtn->setEnabled(val);
}

void SlideToolBar::setEnabledNext(bool val)
{
    d->nextBtn->setEnabled(val);
}

void SlideToolBar::setEnabledPrev(bool val)
{
    d->prevBtn->setEnabled(val);
}

void SlideToolBar::closeConfigurationDialog()
{
    if (d->configDialog->isVisible())
    {
        d->configDialog->reject();
    }
}

void SlideToolBar::slotPlayBtnToggled()
{
    if (d->playBtn->isChecked())
    {
        d->playBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));

        emit signalPause();
    }
    else
    {
        d->playBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-pause")));

        emit signalPlay();
    }
}

void SlideToolBar::slotChangeDelayButtonPressed()
{
    bool ok;
    int delay    = d->settings->delay;
    bool running = (!isPaused() && d->playBtn->isEnabled());

    if (running)
    {
        d->playBtn->animateClick();
    }

    delay = QInputDialog::getInt(this, i18n("Specify delay for slide show"),
                                       i18n("Delay:"), delay, 1, 3600, 1, &ok);

    if (ok)
    {
        d->settings->delay = delay;
    }

    if (running)
    {
        d->playBtn->animateClick();
    }
}

void SlideToolBar::slotNexPrevClicked()
{
    if (!d->playBtn->isChecked())
    {
        d->playBtn->setChecked(true);
        d->playBtn->setIcon(QIcon::fromTheme(QLatin1String("media-playback-start")));

        emit signalPause();
    }
}

void SlideToolBar::slotMenuSlideShowConfiguration()
{
    if (d->configDialog->isVisible())
    {
        d->configDialog->reject();

        return;
    }

    d->currentlyPause = isPaused();

    if (!d->currentlyPause && d->playBtn->isEnabled())
    {
        d->playBtn->animateClick();
    }

    d->configDialog->show();
}

void SlideToolBar::slotConfigurationAccepted()
{
    if (!d->currentlyPause && d->playBtn->isEnabled())
    {
        d->playBtn->animateClick();
    }
}

void SlideToolBar::keyPressEvent(QKeyEvent* e)
{
    switch (e->key())
    {
        case (Qt::Key_F1):
        {
            d->currentlyPause = isPaused();

            if (!d->currentlyPause && d->playBtn->isEnabled())
            {
                d->playBtn->animateClick();
            }

            QPointer<DPluginAboutDlg> help = new DPluginAboutDlg(d->settings->plugin);
            help->exec();
            delete help;

            if (!d->currentlyPause && d->playBtn->isEnabled())
            {
                d->playBtn->animateClick();
            }

            break;
        }

        case (Qt::Key_F2):
        {
            slotMenuSlideShowConfiguration();
            break;
        }

        case (Qt::Key_Space):
        {
            if (d->playBtn->isEnabled())
            {
                d->playBtn->animateClick();
            }

            break;
        }

        case (Qt::Key_Left):
        case (Qt::Key_Up):
        case (Qt::Key_PageUp):
        {
            if (d->prevBtn->isEnabled())
            {
                d->prevBtn->animateClick();
            }

            break;
        }

        case (Qt::Key_Right):
        case (Qt::Key_Down):
        case (Qt::Key_PageDown):
        {
            if (d->nextBtn->isEnabled())
            {
                d->nextBtn->animateClick();
            }

            break;
        }

        case (Qt::Key_Escape):
        {
            if (d->stopBtn->isEnabled())
            {
                d->stopBtn->animateClick();
            }

            break;
        }

        default:
        {
            break;
        }
    }

    e->accept();
}

void SlideToolBar::slotScreenSelected(QAction* act)
{
    if (!act || (act->data().type() != QVariant::Int))
    {
        return;
    }

    emit signalScreenSelected(act->data().toInt());
}

} // namespace DigikamGenericSlideShowPlugin
