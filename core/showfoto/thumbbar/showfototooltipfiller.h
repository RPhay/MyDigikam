/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 09-08-2013
 * Description : Showfoto tool tip filler
 *
 * SPDX-FileCopyrightText: 2013 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef SHOW_FOTO_TOOL_TIP_FILLER_H
#define SHOW_FOTO_TOOL_TIP_FILLER_H

// Qt include

#include <QString>

namespace ShowFoto
{

class ShowfotoItemInfo;

namespace ShowfotoToolTipFiller
{
    QString ShowfotoItemInfoTipContents(const ShowfotoItemInfo& info);

} // namespace ShowfotoToolTipFiller

} // namespace ShowFoto

#endif // SHOW_FOTO_TOOL_TIP_FILLER_H
