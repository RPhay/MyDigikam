/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-05-16
 * Description : Autodetects pano_modify binary program and version
 *
 * SPDX-FileCopyrightText: 2013-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_PANO_MODIFY_BINARY_H
#define DIGIKAM_PANO_MODIFY_BINARY_H

// Local includes

#include "dbinaryiface.h"

using namespace Digikam;

namespace DigikamGenericPanoramaPlugin
{

class PanoModifyBinary : public DBinaryIface
{
    Q_OBJECT

public:

    explicit PanoModifyBinary(QObject* const parent = nullptr);
    ~PanoModifyBinary() override = default;
};

} // namespace DigikamGenericPanoramaPlugin

#endif // DIGIKAM_PANO_MODIFY_BINARY_H
