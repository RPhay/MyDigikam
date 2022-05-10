/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "playerwindow.h"

// Qt includes

#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QMessageBox>
#include <QFileDialog>

using namespace QtAV;

PlayerWindow::PlayerWindow(QWidget *parent)
    : QWidget(parent)
{
    m_unit          = 1000;
    setWindowTitle(QString::fromLatin1("QtAV simple player example"));
    m_player        = new AVPlayerCore(this);
    QVBoxLayout* vl = new QVBoxLayout();
    setLayout(vl);
    m_vo            = new VideoOutput(this);

    if (!m_vo->widget())
    {
        QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));

        return;
    }

    m_player->setRenderer(m_vo);
    vl->addWidget(m_vo->widget());
    m_slider = new QSlider();
    m_slider->setOrientation(Qt::Horizontal);

    connect(m_slider, SIGNAL(sliderMoved(int)),
            this, SLOT(seekBySlider(int)));

    connect(m_slider, SIGNAL(sliderPressed()),
            this, SLOT(seekBySlider()));

    connect(m_player, SIGNAL(positionChanged(qint64)),
            this, SLOT(updateSlider(qint64)));

    connect(m_player, SIGNAL(started()),
            this, SLOT(updateSlider()));

    connect(m_player, SIGNAL(notifyIntervalChanged()),
            this, SLOT(updateSliderUnit()));

    vl->addWidget(m_slider);
    QHBoxLayout* hb = new QHBoxLayout();
    vl->addLayout(hb);
    m_openBtn       = new QPushButton(tr("Open"));
    m_playBtn       = new QPushButton(tr("Play/Pause"));
    m_stopBtn       = new QPushButton(tr("Stop"));
    hb->addWidget(m_openBtn);
    hb->addWidget(m_playBtn);
    hb->addWidget(m_stopBtn);

    connect(m_openBtn, SIGNAL(clicked()),
            this, SLOT(openMedia()));

    connect(m_playBtn, SIGNAL(clicked()),
            this, SLOT(playPause()));

    connect(m_stopBtn, SIGNAL(clicked()),
            m_player, SLOT(stop()));
}

void PlayerWindow::openMedia()
{
    QString file = QFileDialog::getOpenFileName(0, tr("Open a video"));

    if (file.isEmpty())
        return;

    m_player->play(file);
}

void PlayerWindow::seekBySlider(int value)
{
    if (!m_player->isPlaying())
        return;

    m_player->seek(qint64(value*m_unit));
}

void PlayerWindow::seekBySlider()
{
    seekBySlider(m_slider->value());
}

void PlayerWindow::playPause()
{
    if (!m_player->isPlaying())
    {
        m_player->play();

        return;
    }

    m_player->pause(!m_player->isPaused());
}

void PlayerWindow::updateSlider(qint64 value)
{
    m_slider->setRange(0, int(m_player->duration() / m_unit));
    m_slider->setValue(int(value / m_unit));
}

void PlayerWindow::updateSlider()
{
    updateSlider(m_player->position());
}

void PlayerWindow::updateSliderUnit()
{
    m_unit = m_player->notifyInterval();
    updateSlider();
}
