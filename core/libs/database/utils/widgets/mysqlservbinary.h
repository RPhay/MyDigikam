/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-06-20
 * Description : Autodetects Mysql server binary program and version
 *
 * SPDX-FileCopyrightText: 2016-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MYSQL_SERV_BINARY_H
#define DIGIKAM_MYSQL_SERV_BINARY_H

// Local includes

#include "digikam_export.h"
#include "dbinaryiface.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT MysqlServBinary : public DBinaryIface
{
    Q_OBJECT

public:

    MysqlServBinary();
    ~MysqlServBinary() override;

private:

    // Disabled
    explicit MysqlServBinary(QObject*) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_MYSQL_SERV_BINARY_H
