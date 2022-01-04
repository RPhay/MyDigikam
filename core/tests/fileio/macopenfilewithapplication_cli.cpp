/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-04
 * Description : a command line tool to open file with the default
 *               MacOS apllication bundle.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QString>
#include <QUrl>
#include <QList>
#include <QFileInfo>

// Local includes

#include "digikam_debug.h"
#include "dservicemenu.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "openfilewithapplication - Open file with default MacOS bundle Application";
        qCDebug(DIGIKAM_TESTS_LOG) << "Usage: <file path>";
        return -1;
    }

    QString fname    = QString::fromUtf8(argv[1]);
    QString suffix   = QFileInfo(fname).suffix();
    QList<QUrl> list = DServiceMenu::MacApplicationForFileExtension(suffix);

    if (list.isEmpty())
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "No application found to open" << fname;
        return -1;
    }

    QUrl appUrl  = list.first();
    QUrl fileUrl = QUrl::fromLocalFile(fname);

    bool ret = DServiceMenu::MacOpenFileWithApplication(fileUrl, appUrl);

    if (!ret)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << "Cannot start application" << appUrl << "to open" << fname;
        return -1;
    }

    qCDebug(DIGIKAM_TESTS_LOG) << "Application" << appUrl << "started to open" << fname;

    return 0;
}
