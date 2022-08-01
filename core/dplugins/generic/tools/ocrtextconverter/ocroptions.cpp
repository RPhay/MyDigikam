
// C++ includes

#include <cstdio>

// Qt includes

#include <QFile>

// KDE includes

#include "digikam_debug.h"

// Local includes

#include "ocroptions.h"

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

    languages[ENG]         = QString::fromLatin1("english"); 
    languages[OSD]         = QString::fromLatin1("osd");

    return languages;
}


QMap<OcrOptions::PageSegmentationModes, QString> OcrOptions::psmNames()
{
    QMap<PageSegmentationModes, QString> psm;

    psm[OSD_ONLY]                                = QLatin1String("Orientation and script detection (OSD) only.");
    psm[AUTO_WITH_OSD]                           = QLatin1String("Automatic page segmentation with OSD.");
    psm[AUTO_WITH_NO_OSD]                        = QLatin1String("Automatic page segmentation, but no OSD, or OCR. (not implemented)");
    psm[FULLY_AUTO_PAGE]                         = QLatin1String("Fully automatic page segmentation, but no OSD. (Default)");
    psm[SINGLE_COL_TEXT_OF_VAR_SIZE]             = QLatin1String("Assume a single column of text of variable sizes.");
    psm[SINGLE_UNIFORM_BLOCK_OF_VERTICALLY_TEXT] = QLatin1String("Assume a single uniform block of vertically aligned text.");
    psm[SINGLE_UNIFORM_BLOCK_TEXT]               = QLatin1String("Assume a single uniform block of text.");
    psm[SINGLE_TEXT_LINE]                        = QLatin1String("Treat the image as a single text line.");
    psm[SINGLE_WORD]                             = QLatin1String("Treat the image as a single word.");
    psm[SINGLE_WORD_IN_CIRCLE]                   = QLatin1String("Treat the image as a single word in a circle.");
    psm[SINGLE_CHARACTER]                        = QLatin1String("Treat the image as a single character.");
    psm[SPARSE_TEXT]                             = QLatin1String("Sparse text. Find as much text as possible in no particular order.");
    psm[SPARSE_WITH_OSD]                         = QLatin1String("Sparse text with OSD.");
    psm[RAW_LINE]                                = QLatin1String("Raw line. Treat the image as a single text line, bypassing hacks that are Tesseract-specific.");

    return psm;
}



QMap<OcrOptions::EngineModes, QString> OcrOptions::oemNames()
{
    QMap<EngineModes, QString> oem;

    oem[LEGACY_ENGINE_ONLY]      = QLatin1String("Legacy engine only.");
    oem[NEURAL_NETS_LSTM_ONLY]   = QLatin1String("Neural nets LSTM engine only.");
    oem[LEGACY_LSTM_ENGINES]     = QLatin1String("Legacy + LSTM engines.");
    oem[DEFAULT]                 = QLatin1String("Default, based on what is available.");
    
    return oem; 
}


QString OcrOptions::LanguageCodeToValue(OcrOptions::Languages l)
{
    switch (l)
    {
        case ENG:
        {
            return QLatin1String("eng");
            break;
        }
        
        case OSD:
        {
            return QLatin1String("osd");
            break;
        }
        
        default:
        {    
            return QLatin1String("eng");
            break;
        }
    }
}


QString OcrOptions::PsmCodeToValue(PageSegmentationModes psm)
{
   return QString::fromLatin1("%1").arg(psm);  // psm tesseract cli values if range from 0 to 13 
}


QString OcrOptions::OemCodeToValue(EngineModes oem)
{
   return QString::fromLatin1("%1").arg(oem); // oem tesseract cli values if range from 0 to 3
}

} //  namespace Digikam