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

    languages[OcrOptions::Languages::DEFAULT]                                         = QString::fromLatin1("Default"); 
    languages[OcrOptions::Languages::ENG]                                             = QString::fromLatin1("english"); 
    languages[OcrOptions::Languages::OSD]                                             = QString::fromLatin1("osd");

    return languages;
}


QMap<OcrOptions::PageSegmentationModes, QString> OcrOptions::psmNames()
{
    QMap<PageSegmentationModes, QString> psm;

    psm[OcrOptions::PageSegmentationModes::OSD_ONLY]                                  = QLatin1String("Orientation and script detection (OSD) only.");
    psm[OcrOptions::PageSegmentationModes::AUTO_WITH_OSD]                             = QLatin1String("Automatic page segmentation with OSD.");
    psm[OcrOptions::PageSegmentationModes::AUTO_WITH_NO_OSD]                          = QLatin1String("Automatic page segmentation, but no OSD, or OCR. (not implemented)");
    psm[OcrOptions::PageSegmentationModes::DEFAULT]                                   = QLatin1String("Fully automatic page segmentation, but no OSD. (Default)");
    psm[OcrOptions::PageSegmentationModes::SINGLE_COL_TEXT_OF_VAR_SIZE]               = QLatin1String("Assume a single column of text of variable sizes.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_UNIFORM_BLOCK_OF_VERTICALLY_TEXT]   = QLatin1String("Assume a single uniform block of vertically aligned text.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_UNIFORM_BLOCK_TEXT]                 = QLatin1String("Assume a single uniform block of text.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_TEXT_LINE]                          = QLatin1String("Treat the image as a single text line.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_WORD]                               = QLatin1String("Treat the image as a single word.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_WORD_IN_CIRCLE]                     = QLatin1String("Treat the image as a single word in a circle.");
    psm[OcrOptions::PageSegmentationModes::SINGLE_CHARACTER]                          = QLatin1String("Treat the image as a single character.");
    psm[OcrOptions::PageSegmentationModes::SPARSE_TEXT]                               = QLatin1String("Sparse text. Find as much text as possible in no particular order.");
    psm[OcrOptions::PageSegmentationModes::SPARSE_WITH_OSD]                           = QLatin1String("Sparse text with OSD.");
    psm[OcrOptions::PageSegmentationModes::RAW_LINE]                                  = QLatin1String("Raw line. Treat the image as a single text line, bypassing hacks that are Tesseract-specific.");

    return psm;
}



QMap<OcrOptions::EngineModes, QString> OcrOptions::oemNames()
{
    QMap<EngineModes, QString> oem;

    oem[OcrOptions::EngineModes::LEGACY_ENGINE_ONLY]                                 = QLatin1String("Legacy engine only.");
    oem[OcrOptions::EngineModes::NEURAL_NETS_LSTM_ONLY]                              = QLatin1String("Neural nets LSTM engine only.");
    oem[OcrOptions::EngineModes::LEGACY_LSTM_ENGINES]                                = QLatin1String("Legacy + LSTM engines.");
    oem[OcrOptions::EngineModes::DEFAULT]                                            = QLatin1String("Default, based on what is available.");
     
    return oem; 
}


QString OcrOptions::LanguageCodeToValue(OcrOptions::Languages l)
{
    switch (l)
    {
        case OcrOptions::Languages::ENG:
        {
            return QLatin1String("eng");
            break;
        }
        
        case OcrOptions::Languages::OSD:
        {
            return QLatin1String("osd");
            break;
        }

        case OcrOptions::Languages::DEFAULT: 
        {
            return QLatin1String("");
            break;
        }
        
        default:
        {    
            return QLatin1String("");
            break;
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