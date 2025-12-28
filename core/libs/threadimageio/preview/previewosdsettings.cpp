/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-13
 * Description : Preview OSD settings container.
 *
 * SPDX-FileCopyrightText: 2007-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "previewosdsettings.h"

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>

namespace Digikam
{

void PreviewOsdSettings::readFromConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName);

    printName                 = group.readEntry(configPreviewOsdPrintNameEntry,            true);
    printDate                 = group.readEntry(configPreviewOsdPrintDateEntry,            false);
    printApertureFocal        = group.readEntry(configPreviewOsdPrintApertureFocalEntry,   false);
    printExpoSensitivity      = group.readEntry(configPreviewOsdPrintExpoSensitivityEntry, false);
    printMakeModel            = group.readEntry(configPreviewOsdPrintMakeModelEntry,       false);
    printLensModel            = group.readEntry(configPreviewOsdPrintLensModelEntry,       false);
    printComment              = group.readEntry(configPreviewOsdPrintCommentEntry,         false);
    printTitle                = group.readEntry(configPreviewOsdPrintTitleEntry,           false);
    printCapIfNoTitle         = group.readEntry(configPreviewOsdPrintCapIfNoTitleEntry,    false);
    printTags                 = group.readEntry(configPreviewOsdPrintTagsEntry,            false);
    captionFont               = group.readEntry(configPreviewOsdCaptionFontEntry,
                                                QFontDatabase::systemFont(QFontDatabase::GeneralFont));
}

void PreviewOsdSettings::writeToConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(configGroupName);

    group.writeEntry(configPreviewOsdPrintNameEntry,            printName);
    group.writeEntry(configPreviewOsdPrintDateEntry,            printDate);
    group.writeEntry(configPreviewOsdPrintApertureFocalEntry,   printApertureFocal);
    group.writeEntry(configPreviewOsdPrintExpoSensitivityEntry, printExpoSensitivity);
    group.writeEntry(configPreviewOsdPrintMakeModelEntry,       printMakeModel);
    group.writeEntry(configPreviewOsdPrintLensModelEntry,       printLensModel);
    group.writeEntry(configPreviewOsdPrintCommentEntry,         printComment);
    group.writeEntry(configPreviewOsdPrintTitleEntry,           printTitle);
    group.writeEntry(configPreviewOsdPrintCapIfNoTitleEntry,    printCapIfNoTitle);
    group.writeEntry(configPreviewOsdPrintTagsEntry,            printTags);
    group.writeEntry(configPreviewOsdCaptionFontEntry,          captionFont);
    group.sync();
}

} // namespace Digikam
