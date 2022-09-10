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

// C++ includes

#include <cstdio>
#include <cstdlib>

// Qt includes

#include <QApplication>
#include <QFile>
#include <QMessageBox>

// Local includes

#include "videogroup.h"
#include "digikam_debug.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    int r   = 3;
    int c   = 3;
    int idx = 0;

    if ((idx = a.arguments().indexOf(QLatin1String("-r"))) > 0)
        r = a.arguments().at(idx + 1).toInt();

    if ((idx = a.arguments().indexOf(QLatin1String("-c"))) > 0)
        c = a.arguments().at(idx + 1).toInt();

    QString vo;
    idx = a.arguments().indexOf(QLatin1String("-vo"));

    if (idx > 0)
    {
        vo = a.arguments().at(idx+1);
    }
    else
    {
        QString exe(a.arguments().at(0));

        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("exe: %s",
                exe.toUtf8().constData());

        int i = exe.lastIndexOf(QLatin1Char('-'));

        if (i > 0)
        {
            vo = exe.mid(i+1, exe.indexOf(QLatin1Char('.')) - i - 1);
        }
    }

    qCDebug(DIGIKAM_TESTS_LOG).noquote()
        << QString::asprintf("vo: %s",
            vo.toUtf8().constData());

    vo = vo.toLower();

    if (
        (vo != QLatin1String("gl"))  &&
        (vo != QLatin1String("d2d")) &&
        (vo != QLatin1String("gdi")) &&
        (vo != QLatin1String("xv"))
       )
    {
        vo = QString::fromLatin1("qpainter");
    }

    VideoGroup wall;
    wall.setVideoRendererTypeString(vo);
    wall.setRows(r);
    wall.setCols(c);

    wall.show();
    QString file;

    if (a.arguments().size() > 1)
        file = a.arguments().last();

    if (QFile(file).exists())
    {
        wall.play(file);
    }

    return a.exec();
}
