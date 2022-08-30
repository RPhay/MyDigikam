/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR Tesseract options
 *
 * Copyright (C) 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
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

#include "ocroptions.h"

// Local includes

#include <klocalizedstring.h>

namespace Digikam
{

OcrOptions::OcrOptions()
{
}

OcrOptions::~OcrOptions()
{
}

QMap<OcrOptions::Languages, QString> OcrOptions::languagesNames()
{
    QMap<OcrOptions::Languages, QString> languages;

    languages[OcrOptions::Languages::DEFAULT]                                         = i18nc("@info","Default");
    languages[OcrOptions::Languages::ENG]                                             = i18nc("@info","english");
    languages[OcrOptions::Languages::OSD]                                             = i18nc("@info","osd");

    return languages;
}


QMap<OcrOptions::PageSegmentationModes, QString> OcrOptions::psmNames()
{
    QMap<PageSegmentationModes, QString> psm;

    psm[OcrOptions::PageSegmentationModes::OSD_ONLY]                                  = i18nc("@info","OSD only.");
    psm[OcrOptions::PageSegmentationModes::AUTO_WITH_OSD]                             = i18nc("@info","Automatic with OSD.");
    psm[OcrOptions::PageSegmentationModes::AUTO_WITH_NO_OSD]                          = i18nc("@info","Automatic no OSD.");
    psm[OcrOptions::PageSegmentationModes::DEFAULT]                                   = i18nc("@info","Default.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_COL_TEXT_OF_VAR_SIZE]               = i18nc("@info","Column of text.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_UNIFORM_BLOCK_OF_VERTICALLY_TEXT]   = i18nc("@info","Block of vertically aligned text.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_UNIFORM_BLOCK_TEXT]                 = i18nc("@info","Block of text.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_TEXT_LINE]                          = i18nc("@info","Single text line.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_WORD]                               = i18nc("@info","Single word.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_WORD_IN_CIRCLE]                     = i18nc("@info","Single word in a circle.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_CHARACTER]                          = i18nc("@info","Single character.");
    psm[OcrOptions::PageSegmentationModes::SPARSE_TEXT]                               = i18nc("@info","Sparse text.");
    psm[OcrOptions::PageSegmentationModes::SPARSE_WITH_OSD]                           = i18nc("@info","Sparse text with OSD.");
    psm[OcrOptions::PageSegmentationModes::RAW_LINE]                                  = i18nc("@info","Raw line.");

    return psm;
}



QMap<OcrOptions::EngineModes, QString> OcrOptions::oemNames()
{
    QMap<EngineModes, QString> oem;

    oem[OcrOptions::EngineModes::LEGACY_ENGINE_ONLY]                                 = i18nc("@info","Legacy engine only.");
    oem[OcrOptions::EngineModes::NEURAL_NETS_LSTM_ONLY]                              = i18nc("@info","LSTM engine only.");
    oem[OcrOptions::EngineModes::LEGACY_LSTM_ENGINES]                                = i18nc("@info","Legacy + LSTM .");
    oem[OcrOptions::EngineModes::DEFAULT]                                            = i18nc("@info","Default.");

    return oem;
}


QString OcrOptions::LanguageCodeToValue(OcrOptions::Languages l)
{
    switch (l)
    {
        case OcrOptions::Languages::ENG:
        {
            return QLatin1String("eng");
        }

        case OcrOptions::Languages::OSD:
        {
            return QLatin1String("osd");
        }

        case OcrOptions::Languages::DEFAULT:
        {
            return QLatin1String("");
        }

        default:
        {
            return QLatin1String("");
        }
    }
}


QString OcrOptions::PsmCodeToValue(OcrOptions::PageSegmentationModes psm)
{
   return QString::fromLatin1("%1").arg((int)psm);  // psm tesseract cli values if range from 0 to 13
}


QString OcrOptions::OemCodeToValue(OcrOptions::EngineModes oem)
{
   return QString::fromLatin1("%1").arg((int)oem); // oem tesseract cli values if range from 0 to 3
}

} //  namespace Digikam