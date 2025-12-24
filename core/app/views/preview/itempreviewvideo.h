/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-21-12
 * Description : a embedded view to show item preview widget.
 *
 * SPDX-FileCopyrightText: 2006-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2011 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Local includes

#include "mediaplayerview.h"
#include "iteminfo.h"

namespace Digikam
{

class ItemPreviewVideo : public MediaPlayerView
{
    Q_OBJECT

public:

    explicit ItemPreviewVideo(QWidget* const parent);
    ~ItemPreviewVideo() override;

    void setItemInfo(const ItemInfo& info, const ItemInfo& previous, const ItemInfo& next);

private Q_SLOTS:

    void slotAssignRating(int rating);
    void slotAssignPickLabel(int pickId);
    void slotAssignColorLabel(int colorId);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
