/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-16-05
 * Description : a dialog to select user for Web Service tools
 *
 * SPDX-FileCopyrightText: 2015      by Shourya Singh Gupta <shouryasgupta at gmail dot com>
 * SPDX-FileCopyrightText: 2016-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_WS_SELECT_USER_DLG_H
#define DIGIKAM_WS_SELECT_USER_DLG_H

// Qt includes

#include <QString>
#include <QDialog>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT WSSelectUserDlg : public QDialog
{
    Q_OBJECT

public:

    explicit WSSelectUserDlg(QWidget* const parent, const QString& serviceName);
    ~WSSelectUserDlg() override;

    void    reactivate();
    QString getUserName() const;

protected Q_SLOTS:

    void slotOkClicked();
    void slotNewAccountClicked();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_WS_SELECT_USER_DLG_H
