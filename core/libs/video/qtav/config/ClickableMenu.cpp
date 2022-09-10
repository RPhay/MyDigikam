/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "ClickableMenu.h"

// Qt includes

#include <QMouseEvent>

namespace QtAV
{

ClickableMenu::ClickableMenu(QWidget* const parent)
    : QMenu(parent)
{
}

ClickableMenu::ClickableMenu(const QString& title, QWidget* const parent)
    : QMenu(title, parent)
{
}

void ClickableMenu::mouseReleaseEvent(QMouseEvent* e)
{
    QAction* const action = actionAt(e->pos());

    if (action)
    {
        action->activate(QAction::Trigger);

        return;
    }

    QMenu::mouseReleaseEvent(e);
}

} // namespace QtAV
