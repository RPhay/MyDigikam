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

#ifndef DIGIKAM_EXIFTOOL_PARSER_H
#define DIGIKAM_EXIFTOOL_PARSER_H

// Qt Core

#include <QObject>
#include <QString>
#include <QByteArray>

namespace Digikam
{

class QExifToolProcess;
class MetaEngine;

class ExifToolParser : public QObject
{
    Q_OBJECT

public:

    explicit ExifToolParser(QObject* const parent = nullptr);
    ~ExifToolParser();

    bool parse(const QString& path, MetaEngine* meta = nullptr);

Q_SIGNALS:

    void signalExifToolMetadata();

private Q_SLOTS:

    void slotCmdCompleted(int cmdId,
                          int execTime,
                          const QByteArray& cmdOutputChannel,
                          const QByteArray& cmdErrorChannel);

private:

    QExifToolProcess* m_proc;
    MetaEngine*       m_meta;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PARSER_H
