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

#include <QVariant>
#include <QHash>
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QProcess>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class ExifToolProcess;

class DIGIKAM_EXPORT ExifToolParser : public QObject
{
    Q_OBJECT

public:

    /**
     * A map used to store Tags Key and a list of Tags properties:
     *
     * With Exiv2 tag name as key as parsed map of tags:
     *  -   ExifTool tag name           (QString).
     *  -   ExifTool Tag value          (QVariant).
     *  -   ExifTool Tag type           (QString).
     *  -   ExifTool Tag description    (QString).
     *
     * With ExifTool tag name as key as ignored map of tags:
     *  -   Exiv2 tag name              (QString).
     *  -   ExifTool Tag value          (QString).
     *  -   ExifTool Tag type           (QString).
     *  -   ExifTool Tag description    (QString).
     */
    typedef QHash<QString, QVariantList> TagsMap;

public:

    explicit ExifToolParser(QObject* const parent = nullptr);
    ~ExifToolParser();

    bool load(const QString& path);

    /**
     * Turn on/off translations of ExiTool tags to Exiv2.
     * Default is on.
     */
    void setTranslations(bool);

    QString currentParsedPath()  const;
    TagsMap currentParsedTags()  const;
    TagsMap currentIgnoredTags() const;
    QString currentErrorString() const;

private Q_SLOTS:

    void slotCmdCompleted(int cmdId,
                          int execTime,
                          const QByteArray& cmdOutputChannel,
                          const QByteArray& cmdErrorChannel);

    void slotErrorOccurred(QProcess::ProcessError error);

    void slotMetaEngineSettingsChanged();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_PARSER_H
