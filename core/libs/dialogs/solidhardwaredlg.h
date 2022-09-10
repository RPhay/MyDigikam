/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-04-07
 * Description : Solid hardware list dialog
 *
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SOLID_HARDWARE_DLG_H
#define DIGIKAM_SOLID_HARDWARE_DLG_H

// Local includes

#include "infodlg.h"
#include "searchtextbar.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SolidHardwareDlg : public InfoDlg
{
    Q_OBJECT

public:

    explicit SolidHardwareDlg(QWidget* const parent);
    ~SolidHardwareDlg()         override;

private Q_SLOTS:

    void slotSearchTextChanged(const SearchTextSettings&);
    void slotCopy2ClipBoard()   override;
    void slotPopulateDevices();

    void slotDeviceAdded(const QString&);
    void slotDeviceRemoved(const QString&);

private:

    QTreeWidget* currentTreeView() const;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SOLID_HARDWARE_DLG_H
