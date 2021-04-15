/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : ExifTool JSON analyzer
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

#ifndef DIGIKAM_EXIFTOOL_JSON_H
#define DIGIKAM_EXIFTOOL_JSON_H

// Qt Core

#include <QObject>
#include <QString>

namespace Digikam
{

class ExifToolJson : public QObject
{
    Q_OBJECT

public:

    explicit ExifToolJson(QObject* const parent = nullptr);
    ~ExifToolJson();

public:

private Q_SLOTS:

};

} // namespace Digikam

#endif // DIGIKAM_EXIFTOOL_JSON_H
