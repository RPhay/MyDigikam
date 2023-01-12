/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

PlayerWindow::PlayerWindow(QWidget* const parent)
    : QWidget(parent)
{
    m_unit                = 1000;
    setWindowTitle(QString::fromLatin1("QtAV Simple Player Example"));
    m_player              = new AVPlayerCore(this);
    QVBoxLayout* const vl = new QVBoxLayout();
    setLayout(vl);
    m_vo                  = new VideoOutput(this);

    if (!m_vo->widget())
    {
        QMessageBox::warning(nullptr, QString::fromLatin1("QtAV error"),
                             QLatin1String("Can not create video renderer"));

        return;
    }

    m_player->setRenderer(m_vo);
    vl->addWidget(m_vo->widget());
    m_slider              = new QSlider();
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
    QHBoxLayout* const hb = new QHBoxLayout();
    vl->addLayout(hb);
    m_openBtn             = new QPushButton(QLatin1String("Open"));
    m_playBtn             = new QPushButton(QLatin1String("Play/Pause"));
    m_stopBtn             = new QPushButton(QLatin1String("Stop"));
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
    QString file = QFileDialog::getOpenFileName(nullptr,
                                                QLatin1String("Open a video"));

    if (file.isEmpty())
        return;

    m_player->play(file);
}

void PlayerWindow::seekBySlider(int value)
{
    if (!m_player->isPlaying())
        return;

    m_player->seek(value * (qint64)m_unit);
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
