/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-09-27
 * Description : Tool tip for Showfoto stack view item.
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

#ifndef SHOWFOTO_STACK_VIEW_TOOL_TIP_H
#define SHOWFOTO_STACK_VIEW_TOOL_TIP_H

// Qt includes

#include <QString>
#include <QRect>
#include <QModelIndex>

// Local includes

#include "ditemtooltip.h"
#include "showfotoiteminfo.h"

using namespace Digikam;

namespace ShowFoto
{

class ShowfotoStackViewList;

class ShowfotoStackViewToolTip : public DItemToolTip
{
    Q_OBJECT

public:

    explicit ShowfotoStackViewToolTip(ShowfotoStackViewList* const view);
    ~ShowfotoStackViewToolTip()          override;

    void setIndex(const QModelIndex& index);

private:

    QRect   repositionRect()              override;
    QString tipContents()                 override;

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_STACK_VIEW_TOOL_TIP_H
