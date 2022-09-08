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
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "ocroptions.h"

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace DigikamGenericTextConverterPlugin
{

OcrOptions::OcrOptions()
{
}

OcrOptions::~OcrOptions()
{
}

QMap<OcrOptions::Languages, QPair<QString, QString> > OcrOptions::languagesNames()
{
    QMap<OcrOptions::Languages, QPair<QString, QString> > languages;

    languages[OcrOptions::Languages::DEFAULT]                                         = QPair(QLatin1String("Default")             , i18nc("@info:tooltip","Default language mode"));
    languages[OcrOptions::Languages::ENG]                                             = QPair(QLatin1String("Eng")                 , i18nc("@info:tooltip","English mode"));
    languages[OcrOptions::Languages::OSD]                                             = QPair(QLatin1String("OSD")                 , i18nc("@info:tooltip","Orientation and script detection module"));

    return languages;
}

QMap<OcrOptions::PageSegmentationModes,  QPair<QString, QString> > OcrOptions::psmNames()
{
    QMap<PageSegmentationModes,  QPair<QString, QString> > psm;

    psm[OcrOptions::PageSegmentationModes::OSD_ONLY]                                  = QPair(QLatin1String("OSD only")            , i18nc("@info:tooltip","Orientation and script detection (OSD) only."));
    psm[OcrOptions::PageSegmentationModes::AUTO_WITH_OSD]                             = QPair(QLatin1String("With OSD")            , i18nc("@info:tooltip","Automatic page segmentation with OSD."));
    psm[OcrOptions::PageSegmentationModes::AUTO_WITH_NO_OSD]                          = QPair(QLatin1String("No OSD")              , i18nc("@info:tooltip","Automatic page segmentation, but no OSD, or OCR. (not implemented)"));
    psm[OcrOptions::PageSegmentationModes::DEFAULT]                                   = QPair(QLatin1String("Default")             , i18nc("@info:tooltip","Fully automatic page segmentation, but no OSD. (Default)."));
    psm[OcrOptions::PageSegmentationModes::SINGLE_COL_TEXT_OF_VAR_SIZE]               = QPair(QLatin1String("Col of text")         , i18nc("@info:tooltip","Assume a single column of text of variable sizes."));
    psm[OcrOptions::PageSegmentationModes::SINGLE_UNIFORM_BLOCK_OF_VERTICALLY_TEXT]   = QPair(QLatin1String("Vertically aligned")  , i18nc("@info:tooltip","Assume a single uniform block of vertically aligned text."));
    psm[OcrOptions::PageSegmentationModes::SINGLE_UNIFORM_BLOCK_TEXT]                 = QPair(QLatin1String("Block")               , i18nc("@info:tooltip","Assume a single uniform block of text."));
    psm[OcrOptions::PageSegmentationModes::SINGLE_TEXT_LINE]                          = QPair(QLatin1String("Line")                , i18nc("@info:tooltip","Treat the image as a single text line."));
    psm[OcrOptions::PageSegmentationModes::SINGLE_WORD]                               = QPair(QLatin1String("Word")                , i18nc("@info:tooltip","Treat the image as a single word."));
    psm[OcrOptions::PageSegmentationModes::SINGLE_WORD_IN_CIRCLE]                     = QPair(QLatin1String("Word in circle")      , i18nc("@info:tooltip","Treat the image as a single word in a circle."));
    psm[OcrOptions::PageSegmentationModes::SINGLE_CHARACTER]                          = QPair(QLatin1String("Character")           , i18nc("@info:tooltip","Treat the image as a single character."));
    psm[OcrOptions::PageSegmentationModes::SPARSE_TEXT]                               = QPair(QLatin1String("Sparse text")         , i18nc("@info:tooltip","Sparse text. Find as much text as possible in no particular order."));
    psm[OcrOptions::PageSegmentationModes::SPARSE_WITH_OSD]                           = QPair(QLatin1String("Sparse text + OSD")   , i18nc("@info:tooltip","Sparse text with OSD."));
    psm[OcrOptions::PageSegmentationModes::RAW_LINE]                                  = QPair(QLatin1String("Raw line")            , i18nc("@info:tooltip","Raw line. Treat the image as a single text line,"
                                                                                                                                           "bypassing hacks that are Tesseract-specific."));

    return psm;
}

QMap<OcrOptions::EngineModes, QPair<QString, QString> > OcrOptions::oemNames()
{
    QMap<EngineModes, QPair<QString, QString> > oem;

    oem[OcrOptions::EngineModes::LEGACY_ENGINE_ONLY]                                 = QPair(QLatin1String("Legacy")               , i18nc("@info:tooltip","Legacy engine only."));
    oem[OcrOptions::EngineModes::NEURAL_NETS_LSTM_ONLY]                              = QPair(QLatin1String("LSTM")                 , i18nc("@info:tooltip","Neural nets LSTM engine only."));
    oem[OcrOptions::EngineModes::LEGACY_LSTM_ENGINES]                                = QPair(QLatin1String("Legacy + LSTM")        , i18nc("@info:tooltip","Legacy + LSTM engines."));
    oem[OcrOptions::EngineModes::DEFAULT]                                            = QPair(QLatin1String("Default")              , i18nc("@info:tooltip","Default, based on what is available."));

    return oem;
}

QString OcrOptions::LanguageCodeToValue(OcrOptions::Languages l) const
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

QString OcrOptions::PsmCodeToValue(OcrOptions::PageSegmentationModes psm) const
{
   return QString::fromLatin1("%1").arg((int)psm);  // psm tesseract cli values if range from 0 to 13
}

QString OcrOptions::OemCodeToValue(OcrOptions::EngineModes oem) const
{
   return QString::fromLatin1("%1").arg((int)oem); // oem tesseract cli values if range from 0 to 3
}

} // namespace DigikamGenericTextConverterPlugin
