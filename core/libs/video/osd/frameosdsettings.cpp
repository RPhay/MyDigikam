/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : on screen display settings container.
 *
 * SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021      by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "frameosdsettings.h"

// Qt includes

#include <QFontDatabase>

// KDE includes

#include <kconfiggroup.h>

namespace Digikam
{

FrameOsdSettings::FrameOsdSettings()
    : printName             (true),
      printDate             (true),
      printApertureFocal    (false),
      printMakeModel        (false),
      printLensModel        (false),
      printExpoSensitivity  (false),
      printComment          (false),
      printTitle            (false),
      printCapIfNoTitle     (false),
      printTags             (false),
      printRating           (false),
      osdFont               (QFontDatabase::systemFont(QFontDatabase::GeneralFont))
{
}

FrameOsdSettings::~FrameOsdSettings()
{
}

void FrameOsdSettings::readSettings(KConfigGroup& group)
{
    printName            = group.readEntry("MJPEGStreamPrintNameEntry",                        true);
    printDate            = group.readEntry("MJPEGStreamPrintDateEntry",                        true);
    printApertureFocal   = group.readEntry("MJPEGStreamPrintApertureFocalEntry",               false);
    printExpoSensitivity = group.readEntry("MJPEGStreamPrintExpoSensitivityEntry",             false);
    printMakeModel       = group.readEntry("MJPEGStreamPrintMakeModelEntry",                   false);
    printLensModel       = group.readEntry("MJPEGStreamPrintLensModelEntry",                   false);
    printComment         = group.readEntry("MJPEGStreamPrintCommentEntry",                     false);
    printTitle           = group.readEntry("MJPEGStreamPrintTitleEntry",                       false);
    printCapIfNoTitle    = group.readEntry("MJPEGStreamPrintCapIfNoTitleEntry",                false);
    printTags            = group.readEntry("MJPEGStreamPrintTagsEntry",                        false);
    printRating          = group.readEntry("MJPEGStreamPrintRatingEntry",                      false);
    osdFont              = group.readEntry("MJPEGStreamCaptionFontEntry",                      QFontDatabase::systemFont(QFontDatabase::GeneralFont));
}

void FrameOsdSettings::writeSettings(KConfigGroup& group)
{
    group.writeEntry("MJPEGStreamPrintNameEntry",            printName);
    group.writeEntry("MJPEGStreamPrintDateEntry",            printDate);
    group.writeEntry("MJPEGStreamPrintApertureFocalEntry",   printApertureFocal);
    group.writeEntry("MJPEGStreamPrintExpoSensitivityEntry", printExpoSensitivity);
    group.writeEntry("MJPEGStreamPrintMakeModelEntry",       printMakeModel);
    group.writeEntry("MJPEGStreamPrintLensModelEntry",       printLensModel);
    group.writeEntry("MJPEGStreamPrintCommentEntry",         printComment);
    group.writeEntry("MJPEGStreamPrintTitleEntry",           printTitle);
    group.writeEntry("MJPEGStreamPrintCapIfNoTitleEntry",    printCapIfNoTitle);
    group.writeEntry("MJPEGStreamPrintTagsEntry",            printTags);
    group.writeEntry("MJPEGStreamPrintRatingEntry",          printRating);
    group.writeEntry("MJPEGStreamCaptionFontEntry",          osdFont);
}

} // namespace Digikam
