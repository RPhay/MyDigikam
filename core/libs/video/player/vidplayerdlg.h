/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_VID_PLAYER_DLG_H
#define DIGIKAM_VID_PLAYER_DLG_H

// Qt include

#include <QDialog>
#include <QWidget>
#include <QString>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT VidPlayerDlg : public QDialog
{
    Q_OBJECT

public:

    explicit VidPlayerDlg(const QString& file, QWidget* const parent = nullptr);
    ~VidPlayerDlg() override;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_VID_PLAYER_DLG_H
