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

// Qt includes

#include <QApplication>
#include <QThread>
#include <QTimer>

// Local includes

#include "QtAV.h"
#include "GraphicsItemRenderer.h"
#include "WidgetRenderer.h"
#include "GLWidgetRenderer2.h"
#include "VideoPreviewWidget.h"

using namespace QtAV;

class Q_DECL_HIDDEN Thread : public QThread
{
    Q_OBJECT

public:

    explicit Thread(AVPlayerCore* const player)
      : QThread (nullptr),
        mpPlayer(player)
    {
    }

protected:

    virtual void run()
    {
        exec();
    }

    AVPlayerCore* mpPlayer = nullptr;
};

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    AVPlayerCore player;
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

#include "playerthread.moc"
