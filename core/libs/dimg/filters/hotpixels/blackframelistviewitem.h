/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-07-05
 * Description : a list view to display black frames - the item
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2005-2006 by Unai Garro <ugarro at users dot sourceforge dot net>
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

#ifndef DIGIKAM_BLACK_FRAME_LIST_VIEW_ITEM_H
#define DIGIKAM_BLACK_FRAME_LIST_VIEW_ITEM_H

// Qt includes

#include <QList>
#include <QString>
#include <QUrl>
#include <QTreeWidget>

// Local includes

#include "digikam_export.h"
#include "hotpixelprops.h"

namespace Digikam
{

class DIGIKAM_EXPORT BlackFrameListViewItem : public QObject,
                                              public QTreeWidgetItem
{
    Q_OBJECT

public:

    enum BlackFrameConst
    {
        // Columns
        PREVIEW     = 0,
        SIZE        = 1,
        HOTPIXELS   = 2,

        // Thumbnail properties
        THUMB_WIDTH = 150
    };

public:

    explicit BlackFrameListViewItem(QTreeWidget* const parent, const QUrl& url);
    ~BlackFrameListViewItem() override;

    QUrl    frameUrl()      const;
    QString toolTipString() const;

    void emitHotPixelsParsed();

Q_SIGNALS:

    void signalHotPixelsParsed(const QList<HotPixelProps>&, const QUrl&);

private Q_SLOTS:

    void slotHotPixelsParsed(const QList<HotPixelProps>&);
    void slotLoadingProgress(float);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BLACK_FRAME_LIST_VIEW_ITEM_H
