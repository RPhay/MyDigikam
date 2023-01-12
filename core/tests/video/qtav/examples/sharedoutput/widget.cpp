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

#include "widget.h"

// Qt includes

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QTimer>

// Local includes

#include "QtAV.h"
#include "QtAVWidgets_Global.h"

using namespace QtAV;

Widget::Widget(QWidget* const parent)
    : QWidget(parent)
{
    QtAV::Widgets::registerRenderers();
    setWindowTitle(QString::fromLatin1("A Test for Shared Video Renderer. QtAV %1")
                                       .arg(QtAV_Version_String_Long()));

    QVBoxLayout* main_layout = new QVBoxLayout;
    QHBoxLayout* btn_layout  = new QHBoxLayout;
    renderer                 = new VideoOutput();
    renderer->widget()->setFocusPolicy(Qt::StrongFocus);
    renderer->resizeRenderer(640, 480);

    for (int i = 0 ; i < 2 ; ++i)
    {
        player[i]             = new AVPlayerCore(this);
        player[i]->setRenderer(renderer);
        QVBoxLayout* const vb = new QVBoxLayout;
        play_btn[i]           = new QPushButton(this);
        play_btn[i]->setText(QString::fromLatin1("Play-%1").arg(i));
        file_btn[i]           = new QPushButton(this);
        file_btn[i]->setText(QString::fromLatin1("Choose video-%1").arg(i));

        connect(play_btn[i], SIGNAL(clicked()),
                this, SLOT(playVideo()));

        connect(file_btn[i], SIGNAL(clicked()),
                this, SLOT(setVideo()));

        vb->addWidget(play_btn[i]);
        vb->addWidget(file_btn[i]);
        btn_layout->addLayout(vb);
    }

    QPushButton* const net_btn = new QPushButton(QLatin1String("Test online video(rtsp)"));

    connect(net_btn, SIGNAL(clicked()),
            this, SLOT(testRTSP()));

    main_layout->addWidget(renderer->widget());
    main_layout->addWidget(net_btn);
    main_layout->addLayout(btn_layout);
    setLayout(main_layout);
    resize(720, 600);
}

Widget::~Widget()
{
}

void Widget::playVideo()
{
    for (int i = 0 ; i < 2 ; ++i)
        player[i]->pause(true);

    QPushButton* const btn = qobject_cast<QPushButton*>(sender());
    int idx                = btn->text().section(QLatin1Char('-'), 1).toInt();
    player[idx]->pause(false);
}

void Widget::setVideo()
{
    QString v = QFileDialog::getOpenFileName(0, QString::fromLatin1("Select a video"));

    if (v.isEmpty())
        return;

    QPushButton* const btn = qobject_cast<QPushButton*>(sender());
    int idx                = btn->text().section(QLatin1Char('-'), 1).toInt();
    QString oldv           = player[idx]->file();

    if (v == oldv)
        return;

    for (int i = 0 ; i < 2 ; ++i)
        player[i]->pause(true);

    player[idx]->stop();
    player[idx]->setFile(v);
    player[idx]->play();
}

void Widget::testRTSP()
{
    for (int i = 0 ; i < 2 ; ++i)
        player[i]->stop();

    player[0]->play(QString::fromLatin1("rtsp://rtsp.stream/pattern"));
    player[0]->pause(true);

    player[1]->play(QString::fromLatin1("rtsp://rtsp.stream/movie"));
}
