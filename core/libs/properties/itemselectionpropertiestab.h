/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A tab to display information about the current selection.
 *
 * SPDX-FileCopyrightText:      2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QUrl>
#include <QPixmap>
#include <QTreeWidgetItem>

// Local includes

#include "dlayoutbox.h"
#include "iteminfo.h"
#include "thumbnailloadthread.h"
#include "itempropertiessidebardb.h"

namespace Digikam
{

class PAlbum;
class ItemFilterModel;

class ItemSelectionPropertiesTab : public DVBox
{
    Q_OBJECT

public:

    explicit ItemSelectionPropertiesTab(ItemPropertiesSideBarDB* const parent);
    ~ItemSelectionPropertiesTab() override;

    void clear();

    void setSelectionCount(int count);
    void setSelectionSize(const QString& str);
    void setSelectionMimes(const QHash<QString, int>& mimes);
    void setSelectionGroups(const ItemInfoList& groups);

    void setTotalCount(int count);
    void setTotalSize(const QString& str);
    void setTotalMimes(const QHash<QString, int>& mimes);
    void setTotalGroups(const ItemInfoList& groups);

    void setItemFilterModel(ItemFilterModel* const model);

private Q_SLOTS:

    void slotOpenSelectionPressed();
    void slotOpenAlbumPressed();

private:

    PAlbum* currentPAlbum() const;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
