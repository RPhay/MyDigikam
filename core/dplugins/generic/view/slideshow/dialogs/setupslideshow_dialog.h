/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-05-21
 * Description : setup tab for slideshow options.
 *
 * Copyright (C) 2005-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_SETUP_SLIDESHOW_DIALOG_H
#define DIGIKAM_SETUP_SLIDESHOW_DIALOG_H

// Qt includes

#include <QScrollArea>

// Local includes

#include "dplugindialog.h"

using namespace Digikam;

namespace DigikamGenericSlideShowPlugin
{

class SlideShowSettings;

class SetupSlideShowDialog : public DPluginDialog
{
    Q_OBJECT

public:

    explicit SetupSlideShowDialog(SlideShowSettings* const settings, QWidget* const parent = nullptr);
    ~SetupSlideShowDialog() override;

public Q_SLOTS:

    void slotSetUnchecked(int);
    void slotApplySettings();

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericSlideShowPlugin

#endif // DIGIKAM_SETUP_SLIDESHOW_DIALOG_H
