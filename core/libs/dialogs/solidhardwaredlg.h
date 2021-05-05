/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-04-07
 * Description : Solid hardware list dialog
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
