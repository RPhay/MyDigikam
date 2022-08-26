
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

    enum class Languages
    {
        DEFAULT = 0,
        ENG,
        OSD
    };

    enum class PageSegmentationModes
    {
        OSD_ONLY           = 0,
        AUTO_WITH_OSD      ,
        AUTO_WITH_NO_OSD   ,
        DEFAULT,
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

    enum class EngineModes
    {
        LEGACY_ENGINE_ONLY = 0,
        NEURAL_NETS_LSTM_ONLY,
        LEGACY_LSTM_ENGINES,
        DEFAULT
    };

public:

    explicit OcrOptions();
    ~OcrOptions();

    static QMap<OcrOptions::Languages, QString>               languagesNames();
    static QMap<OcrOptions::PageSegmentationModes, QString>   psmNames();
    static QMap<OcrOptions::EngineModes, QString>             oemNames();

    QString LanguageCodeToValue(OcrOptions::Languages l);
    QString PsmCodeToValue(OcrOptions::PageSegmentationModes psm);
    QString OemCodeToValue(OcrOptions::EngineModes oem);
};

} // namespace Digikam

#endif // TESSERACT_OCR_OPTIONS_H

