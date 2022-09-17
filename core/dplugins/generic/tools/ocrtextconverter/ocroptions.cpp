/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-26
 * Description : OCR Tesseract options
 *
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2022      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
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
    : psm             (int(PageSegmentationModes::DEFAULT)),
      oem             (int(EngineModes::DEFAULT)),
      dpi             (300),
      isSaveTextFile  (true),
      isSaveXMP       (true),
      iface           (nullptr),
      multicores      (false)
{
}

OcrOptions::~OcrOptions()
{
}

QMap<OcrOptions::PageSegmentationModes, QPair<QString, QString> > OcrOptions::psmNames()
{
    QMap<PageSegmentationModes,  QPair<QString, QString> > psm;

    psm[PageSegmentationModes::OSD_ONLY]                                = QPair(QLatin1String("OSD only"),           i18nc("@info:tooltip","Orientation and script detection (OSD) only."));
    psm[PageSegmentationModes::AUTO_WITH_OSD]                           = QPair(QLatin1String("With OSD"),           i18nc("@info:tooltip","Automatic page segmentation with OSD."));
    psm[PageSegmentationModes::AUTO_WITH_NO_OSD]                        = QPair(QLatin1String("No OSD"),             i18nc("@info:tooltip","Automatic page segmentation, but no OSD, or OCR. (not implemented)"));
    psm[PageSegmentationModes::DEFAULT]                                 = QPair(QLatin1String("Default"),            i18nc("@info:tooltip","Fully automatic page segmentation, but no OSD. (Default)."));
    psm[PageSegmentationModes::SINGLE_COL_TEXT_OF_VAR_SIZE]             = QPair(QLatin1String("Col of text"),        i18nc("@info:tooltip","Assume a single column of text of variable sizes."));
    psm[PageSegmentationModes::SINGLE_UNIFORM_BLOCK_OF_VERTICALLY_TEXT] = QPair(QLatin1String("Vertically aligned"), i18nc("@info:tooltip","Assume a single uniform block of vertically aligned text."));
    psm[PageSegmentationModes::SINGLE_UNIFORM_BLOCK_TEXT]               = QPair(QLatin1String("Block"),              i18nc("@info:tooltip","Assume a single uniform block of text."));
    psm[PageSegmentationModes::SINGLE_TEXT_LINE]                        = QPair(QLatin1String("Line"),               i18nc("@info:tooltip","Treat the image as a single text line."));
    psm[PageSegmentationModes::SINGLE_WORD]                             = QPair(QLatin1String("Word"),               i18nc("@info:tooltip","Treat the image as a single word."));
    psm[PageSegmentationModes::SINGLE_WORD_IN_CIRCLE]                   = QPair(QLatin1String("Word in circle"),     i18nc("@info:tooltip","Treat the image as a single word in a circle."));
    psm[PageSegmentationModes::SINGLE_CHARACTER]                        = QPair(QLatin1String("Character"),          i18nc("@info:tooltip","Treat the image as a single character."));
    psm[PageSegmentationModes::SPARSE_TEXT]                             = QPair(QLatin1String("Sparse text"),        i18nc("@info:tooltip","Sparse text. Find as much text as possible in no particular order."));
    psm[PageSegmentationModes::SPARSE_WITH_OSD]                         = QPair(QLatin1String("Sparse text + OSD"),  i18nc("@info:tooltip","Sparse text with OSD."));
    psm[PageSegmentationModes::RAW_LINE]                                = QPair(QLatin1String("Raw line"),           i18nc("@info:tooltip","Raw line. Treat the image as a single text line,"
                                                                                                                                           "bypassing hacks that are Tesseract-specific."));

    return psm;
}

QMap<OcrOptions::EngineModes, QPair<QString, QString> > OcrOptions::oemNames()
{
    QMap<EngineModes, QPair<QString, QString> > oem;

    oem[EngineModes::LEGACY_ENGINE_ONLY]    = QPair(QLatin1String("Legacy"),        i18nc("@info:tooltip","Legacy engine only."));
    oem[EngineModes::NEURAL_NETS_LSTM_ONLY] = QPair(QLatin1String("LSTM"),          i18nc("@info:tooltip","Neural nets LSTM engine only."));
    oem[EngineModes::LEGACY_LSTM_ENGINES]   = QPair(QLatin1String("Legacy + LSTM"), i18nc("@info:tooltip","Legacy + LSTM engines."));
    oem[EngineModes::DEFAULT]               = QPair(QLatin1String("Default"),       i18nc("@info:tooltip","Default, based on what is available."));

    return oem;
}

QString OcrOptions::PsmCodeToValue(OcrOptions::PageSegmentationModes psm) const
{
   return QString::fromLatin1("%1").arg((int)psm);  // psm tesseract cli values if range from 0 to 13
}

QString OcrOptions::OemCodeToValue(OcrOptions::EngineModes oem) const
{
   return QString::fromLatin1("%1").arg((int)oem);  // oem tesseract cli values if range from 0 to 3
}

} // namespace DigikamGenericTextConverterPlugin
