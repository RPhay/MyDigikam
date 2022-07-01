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

#include "videoplayer.h"

// Qt includes

#include <QGraphicsView>

#if !defined(QT_NO_OPENGL) && QT_VERSION < QT_VERSION_CHECK(6,0,0)
#   include <QGLWidget>
#endif

#include <QCheckBox>
#include <QSlider>
#include <QLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QDial>

// Local includes

#include "digikam_debug.h"

using namespace QtAV;

VideoPlayer::VideoPlayer(QWidget* const parent)
    : QWidget  (parent),
      videoItem(nullptr)
{
    videoItem                   = new GraphicsItemRenderer;
    videoItem->resizeRenderer(640, 360);
    videoItem->setOutAspectRatioMode(QtAV::VideoAspectRatio);

    QGraphicsScene* const scene = new QGraphicsScene(this);
    scene->addItem(videoItem);

    view                        = new QGraphicsView(scene);

    QSlider* const rotateSlider = new QSlider(Qt::Horizontal);
    rotateSlider->setRange(-180, 180);
    rotateSlider->setValue(0);

    QSlider* const scaleSlider  = new QSlider(Qt::Horizontal);
    scaleSlider->setRange(0, 200);
    scaleSlider->setValue(100);

    QDial* const orientation    = new QDial();
    orientation->setRange(0, 3);
    orientation->setValue(0);

    connect(orientation, SIGNAL(valueChanged(int)),
            this, SLOT(setOrientation(int)));

    connect(rotateSlider, SIGNAL(valueChanged(int)),
            this, SLOT(rotateVideo(int)));

    connect(scaleSlider, SIGNAL(valueChanged(int)),
            this, SLOT(scaleVideo(int)));

    QPushButton* const openBtn = new QPushButton;
    openBtn->setText(QLatin1String("Open"));

    connect(openBtn, SIGNAL(clicked()),
            this, SLOT(open()));

    QCheckBox* const glBox   = new QCheckBox();
    glBox->setText(QString::fromLatin1("OpenGL"));
    glBox->setChecked(false);

    connect(glBox, SIGNAL(toggled(bool)),
            this, SLOT(setOpenGL(bool)));

    QHBoxLayout* const hb    = new QHBoxLayout;
    hb->addWidget(glBox);
    hb->addWidget(openBtn);
    hb->addWidget(orientation);
    QBoxLayout* const layout = new QVBoxLayout;
    layout->addWidget(view);
    layout->addWidget(rotateSlider);
    layout->addWidget(scaleSlider);
    layout->addLayout(hb);
    setLayout(layout);

    mediaPlayer.setRenderer(videoItem);
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::play(const QString& file)
{
    mediaPlayer.play(file);
}

void VideoPlayer::setOpenGL(bool o)
{
    videoItem->setOpenGL(o);

    if (!o)
    {
        view->setViewport(0);

        return;
    }

#if !defined(QT_NO_OPENGL) && QT_VERSION < QT_VERSION_CHECK(6,0,0)

    QGLWidget* const glw = new QGLWidget();   // QGLFormat(QGL::SampleBuffers));
    glw->setAutoFillBackground(false);
    view->setViewport(glw);
    view->setCacheMode(QGraphicsView::CacheNone);

#endif

}

void VideoPlayer::setOrientation(int value)
{
    videoItem->setOrientation(value * 90);
}

void VideoPlayer::rotateVideo(int angle)
{
    // rotate around the center of video element

    qreal x = videoItem->boundingRect().width()  / 2.0;
    qreal y = videoItem->boundingRect().height() / 2.0;
    videoItem->setTransform(QTransform().translate(x, y).rotate(angle).translate(-x, -y));
}

void VideoPlayer::scaleVideo(int value)
{
    qreal v = (qreal)value / 100.0;
    videoItem->setTransform(QTransform().scale(v, v));
}

void VideoPlayer::open()
{
    QString f = QFileDialog::getOpenFileName(nullptr,
                                             QLatin1String("Open a video"));

    if (f.isEmpty())
        return;

    play(f);
}
