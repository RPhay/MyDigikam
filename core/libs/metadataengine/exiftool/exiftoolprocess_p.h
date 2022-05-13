/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-02-18
 * Description : Qt5 and Qt6 interface for exiftool - private container.
 *               Based on ZExifTool Qt interface published at 18 Feb 2021
 *               https://github.com/philvl/ZExifTool
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (c) 2021 by Philippe Vianney Liaud <philvl dot dev at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_EXIFTOOL_PROCESS_P_H
#define DIGIKAM_EXIFTOOL_PROCESS_P_H

#include "exiftoolprocess.h"

// Qt includes

#include <QFile>
#include <QList>
#include <QFileInfo>
#include <QByteArray>
#include <QApplication>
#include <QElapsedTimer>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "metaenginesettings.h"

#define CMD_ID_MIN 1
#define CMD_ID_MAX 2000000000

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolProcess::Private
{
public:

    class Q_DECL_HIDDEN Command
    {
    public:

        Command()
          : id (0),
            ac (ExifToolProcess::NO_ACTION)
        {
        }

        int                     id;
        QByteArray              argsStr;
        ExifToolProcess::Action ac;
    };

public:

    explicit Private(ExifToolProcess* const q);

    void execNextCmd();
    void readOutput(const QProcess::ProcessChannel channel);
    void setProcessErrorAndEmit(QProcess::ProcessError error,
                                const QString& description);

public:

    ExifToolProcess*        pp;
    QString                 etExePath;
    QString                 perlExePath;

    QElapsedTimer           execTimer;
    QList<Command>          cmdQueue;
    int                     cmdRunning;
    ExifToolProcess::Action cmdAction;

    int                     outAwait[2];             ///< [0] StandardOutput | [1] ErrorOutput
    bool                    outReady[2];             ///< [0] StandardOutput | [1] ErrorOutput
    QByteArray              outBuff[2];              ///< [0] StandardOutput | [1] ErrorOutput

    bool                    writeChannelIsClosed;

    QProcess::ProcessError  processError;
    QString                 errorString;

    int                     nextCmdId;               ///< Unique identifier, even in a multi-instances or multi-thread environment

    QMutex                  mutex;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PROCESS_P_H
