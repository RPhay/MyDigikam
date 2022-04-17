/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

// Qt includes

#include <QApplication>
#include <QThread>
#include <QTimer>

// Local includes

#include "QtAV.h"
#include "QtAVWidgets.h"

using namespace QtAV;
class Thread : public QThread
{
public:
    Thread(AVPlayer *player):
        QThread(0)
      , mpPlayer(player)
    {}
protected:
    virtual void run() {
        //mpPlayer->play();
        exec();
    }
    AVPlayer *mpPlayer;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AVPlayer player;
    WidgetRenderer renderer;
    renderer.show();
    player.addVideoRenderer(&renderer);
    player.setFile(a.arguments().last());
    Thread thread(&player);
    player.moveToThread(&thread);
    thread.start();
    player.play();

    return a.exec();
}
