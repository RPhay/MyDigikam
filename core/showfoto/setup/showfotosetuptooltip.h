/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-07-09
 * Description : item tool tip configuration setup tab
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef SHOW_FOTO_SETUP_TOOL_TIP_H
#define SHOW_FOTO_SETUP_TOOL_TIP_H

// Qt includes

#include <QScrollArea>

namespace ShowFoto
{

class ShowfotoSetupToolTip : public QScrollArea
{
    Q_OBJECT

public:

    explicit ShowfotoSetupToolTip(QWidget* const parent = nullptr);
    ~ShowfotoSetupToolTip() override;

    void applySettings();

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOW_FOTO_SETUP_TOOL_TIP_H
