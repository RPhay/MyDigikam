/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-20
 * Description : black frame list view tool tip
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_BLACK_FRAME_TOOL_TIP_H
#define DIGIKAM_BLACK_FRAME_TOOL_TIP_H

// Local includes

#include "ditemtooltip.h"
#include "digikam_export.h"

class QTreeWidgetItem;
class QTreeWidget;

namespace Digikam
{

class DIGIKAM_EXPORT BlackFrameToolTip : public DItemToolTip
{
    Q_OBJECT

public:

    explicit BlackFrameToolTip(QTreeWidget* const view);
    ~BlackFrameToolTip() override;

    void setToolTipString(const QString& tip);
    void setItem(QTreeWidgetItem* const item);

    void show();

protected:

    QRect   repositionRect() override;
    QString tipContents() override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_BLACK_FRAME_TOOL_TIP_H
