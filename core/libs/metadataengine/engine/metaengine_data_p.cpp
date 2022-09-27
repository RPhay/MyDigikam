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

#include "metaengine_data_p.h"

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

void MetaEngineData::Private::clear()
{
    QMutexLocker lock(&s_metaEngineMutex);

    try
    {
        imageComments.clear();
        exifMetadata.clear();
        iptcMetadata.clear();

#ifdef _XMP_SUPPORT_

        xmpMetadata.clear();

#endif

    }
    catch (Exiv2::AnyError& e)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Cannot clear data container with Exiv2 "
                                           << "(Error #" << (int)e.code() << ": "
                                           << QString::fromStdString(e.what())
                                           << ")";
    }
    catch (...)
    {
        qCCritical(DIGIKAM_METAENGINE_LOG) << "Default exception from Exiv2";
    }

}

} // namespace Digikam
