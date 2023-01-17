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
#include <QWidget>

// Local includes

#include "QtAV.h"
#include "QtAVWidgets_Global.h"

int main(int argc, char* argv[])
{
    QtAV::Widgets::registerRenderers();
    QApplication a(argc, argv);
    QtAV::VideoOutput renderer;
    renderer.widget()->show();
    renderer.widget()->setWindowTitle(QString::fromLatin1("Play Video from qrc--QtAV %1")
                                      .arg(QtAV_Version_String_Long()));
    QtAV::AVPlayerCore player;
    player.setRenderer(&renderer);

    player.play(QString::fromLatin1("qrc:/test.mp4"));

    return a.exec();
}
