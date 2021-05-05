/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-07-03
 * Description : Queue common settings container.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020      by Maik Qualmann <metzpinguin at gmail dot com>
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

#include "queuesettings.h"

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

// Local includes

#include "digikam_config.h"

namespace Digikam
{

QueueSettings::QueueSettings()
    : useMultiCoreCPU   (false),
      saveAsNewVersion  (true),
      exifSetOrientation(true),
      useOrgAlbum       (true),
      conflictRule      (FileSaveConflictBox::DIFFNAME),
      renamingRule      (USEORIGINAL),
      rawLoadingRule    (DEMOSAICING)
{
    KSharedConfig::Ptr config          = KSharedConfig::openConfig();
    KConfigGroup group                 = config->group("ImageViewer Settings");

    ioFileSettings.JPEGCompression     = group.readEntry(QLatin1String("JPEGCompression"),     75);
    ioFileSettings.JPEGSubSampling     = group.readEntry(QLatin1String("JPEGSubSampling"),     1);
    ioFileSettings.PNGCompression      = group.readEntry(QLatin1String("PNGCompression"),      9);
    ioFileSettings.TIFFCompression     = group.readEntry(QLatin1String("TIFFCompression"),     false);

#ifdef HAVE_JASPER

    ioFileSettings.JPEG2000Compression = group.readEntry(QLatin1String("JPEG2000Compression"), 75);
    ioFileSettings.JPEG2000LossLess    = group.readEntry(QLatin1String("JPEG2000LossLess"),    true);

#endif // HAVE_JASPER

    ioFileSettings.PGFCompression      = group.readEntry(QLatin1String("PGFCompression"),      3);
    ioFileSettings.PGFLossLess         = group.readEntry(QLatin1String("PGFLossLess"),         true);

#ifdef HAVE_X265

    ioFileSettings.HEIFCompression     = group.readEntry(QLatin1String("HEIFCompression"),     75);
    ioFileSettings.HEIFLossLess        = group.readEntry(QLatin1String("HEIFLossLess"),        true);

#endif // HAVE_X265
}

QueueSettings::~QueueSettings()
{
}

} // namespace Digikam
