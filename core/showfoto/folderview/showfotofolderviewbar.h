/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : a tool bar for Showfoto folder view
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

#ifndef SHOWFOTO_FOLDER_VIEW_TOOL_BAR_H
#define SHOWFOTO_FOLDER_VIEW_TOOL_BAR_H

// Qt includes

#include <QWidget>

// Local includes

#include "slideshowsettings.h"
#include "dlayoutbox.h"

using namespace Digikam;

namespace ShowFoto
{

class ShowfotoFolderViewBar : public DVBox
{
    Q_OBJECT

public:

    explicit ShowfotoFolderViewBar(QWidget* const parent);
    ~ShowfotoFolderViewBar() override;

    void setCurrentPath(const QString& path);

public Q_SLOTS:

    void slotPreviousEnabled(bool);
    void slotNextEnabled(bool);

Q_SIGNALS:

    void signalGoHome();
    void signalGoUp();
    void signalGoPrevious();
    void signalGoNext();
    void signalCustomPathChanged(const QString&);

private Q_SLOTS:

    void slotCustomPathChanged();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_TOOL_BAR_H
