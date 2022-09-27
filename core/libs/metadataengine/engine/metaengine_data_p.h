/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-15
 * Description : Exiv2 library interface.
 *               Internal private data container.
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_META_ENGINE_DATA_PRIVATE_H
#define DIGIKAM_META_ENGINE_DATA_PRIVATE_H

#include "metaengine_p.h"

namespace Digikam
{

class Q_DECL_HIDDEN MetaEngineData::Private : public QSharedData
{
public:

    void clear();

public:

    std::string     imageComments;

    Exiv2::ExifData exifMetadata;

    Exiv2::IptcData iptcMetadata;

#ifdef _XMP_SUPPORT_

    Exiv2::XmpData  xmpMetadata;

#endif

};

} // namespace Digikam

#endif // DIGIKAM_META_ENGINE_DATA_PRIVATE_H
