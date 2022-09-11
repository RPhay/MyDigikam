/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-03-20
 * Description : Data set for item lister
 *
 * SPDX-FileCopyrightText: 2007-2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemlisterrecord.h"

namespace Digikam
{

ItemListerRecord::ItemListerRecord()
    : albumID                         (-1),
      albumRootID                     (-1),
      rating                          (-1),
      fileSize                        (-1),
      imageID                         (-1),
      currentFuzzySearchReferenceImage(-1),
      currentSimilarity               (0.0),
      category                        (DatabaseItem::UndefinedCategory)
{
}

bool ItemListerRecord::operator==(const ItemListerRecord& record) const
{
    return (imageID == record.imageID);
}

} // namespace Digikam
