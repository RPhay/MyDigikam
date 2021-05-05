/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2016-06-20
 * Description : Autodetects Mysql initializer binary program and version
 *
 * Copyright (C) 2016-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_MYSQL_INIT_BINARY_H
#define DIGIKAM_MYSQL_INIT_BINARY_H

// Local includes

#include "digikam_export.h"
#include "dbinaryiface.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT MysqlInitBinary : public DBinaryIface
{
    Q_OBJECT

public:

    MysqlInitBinary();
    ~MysqlInitBinary() override;

private:

    // Disable
    explicit MysqlInitBinary(QObject*) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_MYSQL_INIT_BINARY_H
