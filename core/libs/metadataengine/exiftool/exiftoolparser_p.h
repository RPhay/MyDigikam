/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-11-28
 * Description : ExifTool process stream parser - private container.
 *
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EXIFTOOL_PARSER_P_H
#define DIGIKAM_EXIFTOOL_PARSER_P_H

#include "exiftoolparser.h"

// Qt includes

#include <QDir>
#include <QLocale>
#include <QStringList>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QEventLoop>
#include <QFileInfo>
#include <QTemporaryFile>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "metaenginesettings.h"
#include "digikam_config.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolParser::Private
{
public:

    explicit Private();
    ~Private();

    bool       prepareProcess();
    bool       startProcess(const QByteArrayList& cmdArgs, ExifToolProcess::Action cmdAction);
    QByteArray filePathEncoding(const QFileInfo& fi) const;
    void       manageEventLoop(int cmdAction);

    /**
     * Returns a string for an action.
     */
    QString actionString(int cmdAction) const;

public:

    ExifToolProcess*               proc;            ///< ExifTool process instance.
    QList<QEventLoop*>             evLoops;         ///< Event loops for the ExifTool process actions.
    QString                        currentPath;     ///< Current file path processed by ExifTool.
    ExifToolData                   exifToolData;    ///< Current ExifTool data (input or output depending of the called method.
    QTemporaryFile                 argsFile;        ///< Temporary file to store Exiftool arg config file.

    QList<QMetaObject::Connection> hdls;            ///< Handles of signals/slots connections used to control streams with ExifTool process.
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PARSER_P_H
