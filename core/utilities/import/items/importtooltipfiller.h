/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-28-07
 * Description : Import icon view tool tip
 *
 * SPDX-FileCopyrightText: 2012      by Islam Wazery <wazery at ubuntu dot com>
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMPORT_TOOL_TIP_FILLER_H
#define DIGIKAM_IMPORT_TOOL_TIP_FILLER_H

// Qt includes

#include <QString>

namespace Digikam
{

class FilterAction;
class CamItemInfo;

namespace ImportToolTipFiller
{
    QString CamItemInfoTipContents(const CamItemInfo& info);

} // namespace ImportToolTipFiller

} // namespace Digikam

#endif // DIGIKAM_IMPORT_TOOL_TIP_FILLER_H
