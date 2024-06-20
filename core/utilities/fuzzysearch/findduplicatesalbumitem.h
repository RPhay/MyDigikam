/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-06-17
 * Description : Find Duplicates tree-view search album item.
 *
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QTreeWidget>
#include <QUrl>
#include <QList>

// Local includes

#include "album.h"
#include "iteminfo.h"
#include "thumbnailloadthread.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT FindDuplicatesAlbumItem : public QTreeWidgetItem
{

public:

    enum Column
    {
      REFERENCE_IMAGE = 0,
      REFERENCE_DATE  = 1,
      REFERENCE_ALBUM = 2,
      RESULT_COUNT    = 3,
      AVG_SIMILARITY  = 4
    };

public:

    explicit FindDuplicatesAlbumItem(QTreeWidget* const parent, SAlbum* const album);
    ~FindDuplicatesAlbumItem()                        override;

    bool hasValidThumbnail()                     const;

    /**
     * Calculates the duplicates count and average similarity.
     **/
    void calculateInfos(const QList<qlonglong>& deletedImages = QList<qlonglong>());

    /**
     * Returns the item count.
     **/
    int itemCount()                              const;

    SAlbum* album()                              const;
    QUrl    refUrl()                             const;

    void setThumb(const QPixmap& pix,
                  bool hasThumb = true);

    bool operator<(const QTreeWidgetItem& other) const override;
    QList<ItemInfo> duplicatedItems();

private:

    class Private;
    Private* const d = nullptr;

private:

    Q_DISABLE_COPY(FindDuplicatesAlbumItem)
};

} // namespace Digikam
