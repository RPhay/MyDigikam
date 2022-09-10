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

#ifndef QTAV_WIDGETS_CLICKABLE_MENU_H
#define QTAV_WIDGETS_CLICKABLE_MENU_H

// Qt includes

#include <QMenu>

// Local includes

#include "QtAVWidgets_Global.h"

namespace QtAV
{

class DIGIKAM_EXPORT ClickableMenu : public QMenu
{
    Q_OBJECT

public:

    explicit ClickableMenu(QWidget* const parent = nullptr);
    explicit ClickableMenu(const QString& title, QWidget* const parent = nullptr);

protected:

    virtual void mouseReleaseEvent(QMouseEvent*);
};

} // namespace QtAV

#endif // QTAV_WIDGETS_CLICKABLE_MENU_H
