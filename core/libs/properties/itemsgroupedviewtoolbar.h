/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-04-11
 * Description : A list view to show grouped items
 *
 * SPDX-FileCopyrightText: 2008-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>

namespace Digikam
{

class ItemsGroupedViewToolbar : public QWidget
{
    Q_OBJECT

public:

    explicit ItemsGroupedViewToolbar(QWidget* const parent = nullptr);

Q_SIGNALS:

    void signalDeleteRequested();

protected:

    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event)      override;
};

} // namespace Digikam
