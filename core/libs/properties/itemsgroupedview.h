/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A list view to show grouped items
 *
 * SPDX-FileCopyrightText: 2008-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QPixmap>
#include <QTreeWidget>
#include <QTreeWidgetItem>

// Local includes

#include "iteminfo.h"
#include "thumbnailloadthread.h"

namespace Digikam
{

class ItemsGroupedView : public QTreeWidget
{
    Q_OBJECT

public:

    explicit ItemsGroupedView(QWidget* const parent);
    ~ItemsGroupedView() override;

    void setGroups(const ItemInfoList& items);

private Q_SLOTS:

    void slotGotThumbnail(const LoadingDescription&, const QPixmap&);
    void slotSettingsChanged();

private:

    void setThumbnail(QTreeWidgetItem* const item, const QPixmap& pix);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
