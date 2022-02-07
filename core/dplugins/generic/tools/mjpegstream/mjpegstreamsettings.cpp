/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-24
 * Description : MJPEG settings container.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2021 by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "mjpegstreamsettings.h"

// Qt includes

#include <QFontDatabase>

// KDE includes

#include <kconfiggroup.h>

// Local includes

#include "vidslidesettings.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegStreamSettings::MjpegStreamSettings()
    : port                  (8080),
      loop                  (true),
      quality               (75),
      delay                 (5),
      outSize               (VidSlideSettings::BLUERAY),
      rate                  (10),
      transition            (TransitionMngr::None),
      effect                (EffectMngr::None),
      iface                 (nullptr),
      printName             (true),
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

MjpegStreamSettings::~MjpegStreamSettings()
{
}

void MjpegStreamSettings::setCollectionMap(const MjpegServerMap& map)
{
    inputImages.clear();

    for (MjpegServerMap::const_iterator it = map.constBegin() ; it != map.constEnd() ; ++it)
    {
        inputImages.append(it.value());
    }
}

void MjpegStreamSettings::readSettings(KConfigGroup& group)
{
    port                 = group.readEntry("MJPEGStreamPort",                                  8080);
    loop                 = group.readEntry("MJPEGStreamLoop",                                  true);
    quality              = group.readEntry("MJPEGStreamQuality",                               75);
    delay                = group.readEntry("MJPEGStreamDelay",                                 5);
    rate                 = group.readEntry("MJPEGStreamRate",                                  10);
    outSize              = group.readEntry("MJPEGStreamOutSize",                               (int)VidSlideSettings::BLUERAY);
    effect               = (EffectMngr::EffectType)group.readEntry("MJPEGStreamEffect",        (int)EffectMngr::None);
    transition           = (TransitionMngr::TransType)group.readEntry("MJPEGStreamTransition", (int)TransitionMngr::None);
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

void MjpegStreamSettings::writeSettings(KConfigGroup& group)
{
    group.writeEntry("MJPEGStreamPort",                      port);
    group.writeEntry("MJPEGStreamLoop",                      loop);
    group.writeEntry("MJPEGStreamQuality",                   quality);
    group.writeEntry("MJPEGStreamRate",                      rate);
    group.writeEntry("MJPEGStreamDelay",                     delay);
    group.writeEntry("MJPEGStreamOutSize",                   outSize);
    group.writeEntry("MJPEGStreamEffect",                    (int)effect);
    group.writeEntry("MJPEGStreamTransition",                (int)transition);
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

} // namespace DigikamGenericMjpegStreamPlugin
