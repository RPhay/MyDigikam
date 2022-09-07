/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-12-31
 * Description : digiKam plugin about dialog
 *
 * Copyright (C) 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DPLUGIN_ABOUT_DLG_H
#define DIGIKAM_DPLUGIN_ABOUT_DLG_H

// Qt includes

#include <QDialog>

// Local includes

#include "digikam_export.h"
#include "dplugin.h"

namespace Digikam
{

class DIGIKAM_EXPORT DPluginAboutDlg : public QDialog
{
    Q_OBJECT

public:

    explicit DPluginAboutDlg(DPlugin* const tool, QWidget* const parent = nullptr);
    ~DPluginAboutDlg() override;

private:

    Q_DISABLE_COPY(DPluginAboutDlg)
};

} // namespace Digikam

#endif // DIGIKAM_DPLUGIN_ABOUT_DLG_H
