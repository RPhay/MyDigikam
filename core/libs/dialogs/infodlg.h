/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : general info list dialog
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_INFO_DLG_H
#define DIGIKAM_INFO_DLG_H

// Qt includes

#include <QWidget>
#include <QTreeWidget>
#include <QDialogButtonBox>
#include <QTabWidget>
#include <QDialog>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT InfoDlg : public QDialog
{
    Q_OBJECT

public:

    explicit InfoDlg(QWidget* const parent);
    ~InfoDlg();

    virtual void setInfoMap(const QMap<QString, QString>& list);

    QTabWidget*       tabView()    const;
    QTreeWidget*      listView()   const;
    QWidget*          mainWidget() const;
    QDialogButtonBox* buttonBox()  const;

private Q_SLOTS:

    void slotHelp();
    virtual void slotCopy2ClipBoard();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_INFO_DLG_H
