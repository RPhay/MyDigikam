/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-21
 * Description : Queue common settings container.
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_QUEUE_SETTINGS_H
#define DIGIKAM_BQM_QUEUE_SETTINGS_H

// Qt includes

#include <QUrl>

// Local includes

#include "iofilesettings.h"
#include "drawdecodersettings.h"
#include "filesaveconflictbox.h"

namespace Digikam
{

/**
 * This container host all common settings used by a queue, not including assigned batch tools
 */
class QueueSettings
{

public:

    enum RenamingRule
    {
        USEORIGINAL = 0,
        CUSTOMIZE
    };

    enum RawLoadingRule
    {
        USEEMBEDEDJPEG = 0,
        DEMOSAICING
    };

public:

    explicit QueueSettings();
    ~QueueSettings();

public:

    bool                              useMultiCoreCPU;

    bool                              saveAsNewVersion;

    /// Setting managed through Metadata control panel.
    bool                              exifSetOrientation;

    /// If true, original file dir will be used to process queue items.
    bool                              useOrgAlbum;

    QString                           renamingParser;

    QUrl                              workingUrl;

    FileSaveConflictBox::ConflictRule conflictRule;
    RenamingRule                      renamingRule;
    RawLoadingRule                    rawLoadingRule;

    DRawDecoderSettings               rawDecodingSettings;

    IOFileSettings                    ioFileSettings;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_QUEUE_SETTINGS_H
