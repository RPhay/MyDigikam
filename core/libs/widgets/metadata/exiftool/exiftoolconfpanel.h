/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-29
 * Description : ExifTool configuration panel.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_EXIF_TOOL_CONF_PANEL_H
#define DIGIKAM_EXIF_TOOL_CONF_PANEL_H

// Qt includes

#include <QWidget>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "searchtextbar.h"

namespace Digikam
{

class DIGIKAM_EXPORT ExifToolConfPanel : public QWidget
{
    Q_OBJECT

public:

    explicit ExifToolConfPanel(QWidget* const parent = nullptr);
    ~ExifToolConfPanel() override;

    QString exifToolDirectory() const;
    void setExifToolDirectory(const QString& dir);

private Q_SLOTS:

    void slotSearchTextChanged(const SearchTextSettings&);
    void slotExifToolBinaryFound(bool);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_TOOL_CONF_PANEL_H
