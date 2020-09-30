/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : Help Box Dialog for Face Management
 *
 * Copyright (C) 2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
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

#ifndef DIGIKAM_FACE_MANAGEMENT_HELP_DLG_H
#define DIGIKAM_FACE_MANAGEMENT_HELP_DLG_H

// Qt Includes

#include <QDialog>

namespace Digikam
{

class FaceManagementHelpDlg : public QDialog
{
    Q_OBJECT

public:

    explicit FaceManagementHelpDlg(QWidget* const parent);
    ~FaceManagementHelpDlg() override;
};

} //namespace Digikam

#endif // DIGIKAM_FACE_MANAGEMENT_HELP_DLG_H
