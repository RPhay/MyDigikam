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
#include <QApplication>

#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator ts;
    if (ts.load(qApp->applicationDirPath().append(QLatin1String("/i18n/QtAV_")).append(QLocale::system().name())))
        a.installTranslator(&ts);
    QTranslator qtts;
    if (qtts.load(QString::fromLatin1("qt_") + QLocale::system().name()))
        a.installTranslator(&qtts);

    VideoPlayer w;
    w.show();
    if (a.arguments().size() > 1)
        w.play(a.arguments().last());
    
    return a.exec();
}
