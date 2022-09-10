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
#include <QLocale>

// Local includes

#include "videoplayer.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    VideoPlayer w;
    w.show();

    if (a.arguments().size() > 1)
        w.play(a.arguments().last());

    return a.exec();
}
