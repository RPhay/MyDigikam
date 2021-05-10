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
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QFileInfo>
#include <QElapsedTimer>
#include <QList>
#include <QByteArray>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolProcess::Private
{
public:

    class Command
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

    ExifToolProcess*        parent;
    QString                 etExePath;
    QString                 perlExePath;
    QProcess*               process;

    QElapsedTimer           execTimer;
    QList<Command>          cmdQueue;
    int                     cmdRunning;
    ExifToolProcess::Action cmdAction;

    int                     outAwait[2];             ///< [0] StandardOutput | [1] ErrorOutput
    bool                    outReady[2];             ///< [0] StandardOutput | [1] ErrorOutput
    QByteArray              outBuff[2];              ///< [0] StandardOutput | [1] ErrorOutput

    bool                    writeChannelIsClosed;
    bool                    exifToolHasFinished;

    QProcess::ProcessError  processError;
    QString                 errorString;

public:

    static const int        CMD_ID_MIN  = 1;
    static const int        CMD_ID_MAX  = 2000000000;

    static int              s_nextCmdId;               ///< Unique identifier, even in a multi-instances or multi-thread environment
    static QMutex           s_cmdIdMutex;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PROCESS_P_H
