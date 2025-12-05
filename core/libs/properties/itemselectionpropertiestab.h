/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A tab to display information about the current selection.
 *
 * SPDX-FileCopyrightText: 2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
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

#include "dexpanderbox.h"
#include "iteminfo.h"
#include "thumbnailloadthread.h"

namespace Digikam
{

class ItemSelectionPropertiesTab : public DExpanderBox
{
    Q_OBJECT

public:

    explicit ItemSelectionPropertiesTab(QWidget* const parent);
    ~ItemSelectionPropertiesTab() override;

    void setCurrentURL(const QUrl& url = QUrl());

    void setSelectionCount(const QString& str);
    void setSelectionSize(const QString& str);
    void setSelectionGroups(const ItemInfoList& infos);
    void setTotalCount(const QString& str);
    void setTotalSize(const QString& str);
    void setTotalGroups(const ItemInfoList& infos);

private Q_SLOTS:

    void slotThumbnail(const LoadingDescription&, const QPixmap&);

private:

    void setThumbnail(QTreeWidgetItem* const item, const QPixmap& pix);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
