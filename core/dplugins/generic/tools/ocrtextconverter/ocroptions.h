
#ifndef TESSERACT_OCR_OPTIONS_H
#define TESSERACT_OCR_OPTIONS_H

// Qt includes

#include <QString>
#include <QMap>

// Local includes

#include "digikam_export.h"


namespace Digikam
{

class DIGIKAM_EXPORT OcrOptions
{

public:

   enum Languages
    {
        ENG = 0,                           
        OSD = 1
    };

    enum PageSegmentationModes
    {
        OSD_ONLY           = 0,
        AUTO_WITH_OSD      ,
        AUTO_WITH_NO_OSD   ,
        FULLY_AUTO_PAGE,
        SINGLE_COL_TEXT_OF_VAR_SIZE,
        SINGLE_UNIFORM_BLOCK_OF_VERTICALLY_TEXT,
        SINGLE_UNIFORM_BLOCK_TEXT,
        SINGLE_TEXT_LINE,
        SINGLE_WORD,
        SINGLE_WORD_IN_CIRCLE,
        SINGLE_CHARACTER,
        SPARSE_TEXT,
        SPARSE_WITH_OSD,
        RAW_LINE
    };

    enum EngineModes
    {
        LEGACY_ENGINE_ONLY = 0,
        NEURAL_NETS_LSTM_ONLY = 1, 
        LEGACY_LSTM_ENGINES = 2,
        DEFAULT = 3
    };

public:

    explicit OcrOptions();
    ~OcrOptions();    

    static QMap<Languages, QString>               languagesNames();
    static QMap<PageSegmentationModes, QString>   psmNames();
    static QMap<EngineModes, QString>             oemNames();

    QString LanguageCodeToValue(Languages l);
    QString PsmCodeToValue(PageSegmentationModes psm);
    QString OemCodeToValue(EngineModes oem);
};

} // namespace Digikam

#endif // TESSERACT_OCR_OPTIONS_H

