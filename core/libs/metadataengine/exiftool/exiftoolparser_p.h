/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : ExifTool JSON parser
 *
 * Copyright (C) 2013-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
#include <QStringList>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QEventLoop>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "metaenginesettings.h"
#include "exiftoolprocess.h"
#include "digikam_config.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolParser::Private
{
public:

    explicit Private();

    bool       prepareProcess();
    QByteArray filePathEncoding(const QFileInfo& fi) const;
    void       manageEventLoop(int cmdAction);

    /**
     * Returns a string for an action.
     */
    QString actionString(int cmdAction) const;

public:

    ExifToolProcess*               proc;
    QEventLoop*                    loopLoad;
    QEventLoop*                    loopChunk;
    QEventLoop*                    loopApply;
    QString                        parsedPath;
    TagsMap                        parsedMap;
    TagsMap                        ignoredMap;

    QList<QMetaObject::Connection> hdls;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PARSER_P_H
