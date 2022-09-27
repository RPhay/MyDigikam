/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-01-19
 * Description : free space widget tool tip
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FREE_SPACE_TOOL_TIP_H
#define DIGIKAM_FREE_SPACE_TOOL_TIP_H

// Local includes

#include "ditemtooltip.h"

namespace Digikam
{

class FreeSpaceToolTip : public DItemToolTip
{
    Q_OBJECT

public:

    explicit FreeSpaceToolTip(QWidget* const parent);
    ~FreeSpaceToolTip()         override;

    void setToolTip(const QString& tip);

    void show();

protected:

    QRect   repositionRect()    override;
    QString tipContents()       override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FREE_SPACE_TOOL_TIP_H
