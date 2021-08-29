/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 28/08/2021
 * Description : Writer of focus points to exiftool data
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
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

#ifndef DIGIKAM_FOCUSPOINTS_WRITER_H
#define DIGIKAM_FOCUSPOINTS_WRITER_H

// Qt includes

#include <QObject>
#include <QVariant>
#include <QStringList>

// Local includes

#include "digikam_export.h"
#include "focuspoint.h"

namespace Digikam
{

class DIGIKAM_EXPORT FocusPointsWriter : public QObject
{
    Q_OBJECT

public:

    explicit FocusPointsWriter(QObject* const parent, const QString& path);
    ~FocusPointsWriter();

public:

    void writeFocusPoint(const FocusPoint& point);
    void writeFocusPoint(const QRectF& rectF);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FOCUSPOINTS_WRITER_H
