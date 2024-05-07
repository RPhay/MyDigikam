/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-07-06
 * Description : Autodetect outlook binary program
 *
 * SPDX-FileCopyrightText: 2020-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_OUTLOOK_BINARY_H
#define DIGIKAM_OUTLOOK_BINARY_H

// Local includes

#include "dbinaryiface.h"

namespace Digikam
{

class OutlookBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit OutlookBinary(QObject* const parent = nullptr);
    ~OutlookBinary() override = default;
};

} // namespace Digikam

#endif // DIGIKAM_OUTLOOK_BINARY_H
