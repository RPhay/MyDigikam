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
#include <QFile>
#include <QBuffer>

// Local includes

#include "QtAV.h"
#include "WidgetRenderer.h"
#include "AVPlayerCore.h"

using namespace QtAV;

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QFile vidfile(a.arguments().last());

    if (!vidfile.open(QIODevice::ReadOnly))
        return 1;

    WidgetRenderer renderer[2];
    renderer[0].show();
    renderer[0].widget()->setWindowTitle(QString::fromLatin1("Test QFile"));
    renderer[1].show();
    renderer[1].widget()->setWindowTitle(QString::fromLatin1("Test QBuffer. Play <= 1M Video from Memory"));

    AVPlayerCore player[2];
    player[0].addVideoRenderer(&renderer[0]);
    player[1].addVideoRenderer(&renderer[1]);

    QByteArray data = vidfile.read(1024 * 1024);
    vidfile.seek(0);
    QBuffer buf(&data);

    if (buf.open(QIODevice::ReadOnly))
    {
        player[1].setIODevice(&buf);
    }

    player[0].setIODevice(&vidfile);
    player[0].play();
    player[1].play();

    return a.exec();
}
