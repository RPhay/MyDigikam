/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool metadata list view.
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

#ifndef DIGIKAM_EXIF_TOOL_LIST_VIEW_H
#define DIGIKAM_EXIF_TOOL_LIST_VIEW_H

// Qt includes

#include <QTreeWidget>
#include <QWidget>
#include <QString>
#include <QUrl>

// Local includes

#include "exiftoolparser.h"

namespace Digikam
{

class ExifToolListViewGroup;

class ExifToolListView : public QTreeWidget
{
    Q_OBJECT

public:

    explicit ExifToolListView(QWidget* const parent);
    ~ExifToolListView() override;

    bool loadFromUrl(const QUrl& url);

private:

    void setMetadata(const ExifToolParser::TagsMap& map);
    ExifToolListViewGroup* findGroup(const QString& group);

private:

    ExifToolParser* m_parser;
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_TOOL_LIST_VIEW_H
