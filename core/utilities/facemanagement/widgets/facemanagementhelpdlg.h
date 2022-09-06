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
 * SPDX-License-Identifier: GPL-2.0-or-later
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
