/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : Autodetect enblend binary program and version
 *
 * SPDX-FileCopyrightText: 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ENBLEND_BINARY_H
#define DIGIKAM_ENBLEND_BINARY_H

// Local includes

#include "dbinaryiface.h"

using namespace Digikam;

namespace DigikamGenericPanoramaPlugin
{

class EnblendBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit EnblendBinary(QObject* const parent = nullptr);
    ~EnblendBinary() override = default;
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_ENBLEND_BINARY_H
