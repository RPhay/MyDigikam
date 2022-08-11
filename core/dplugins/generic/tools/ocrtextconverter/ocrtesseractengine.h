#ifndef OCR_TESSERACT_ENGINE_H
#define OCR_TESSERACT_ENGINE_H

// Qt includes 

#include <QString>
#include <QProcess>

// Local includes

#include "drawdecoder.h"
#include "digikam_export.h"

using namespace Digikam;

namespace DigikamGenericTextConverterPlugin
{

class OcrTesseracrEngine : public QObject
{
    Q_OBJECT
public: 

    enum ConvertError
    {
        PROCESS_CONTINUE       =  1,        ///< Current stages is done.
        PROCESS_COMPLETE       =  0,        ///< All stages done.
        PROCESS_FAILED         = -1,        ///< A failure happen while processing.
        PROCESS_CANCELED       = -2,        ///< User has canceled processing.
    };

public:


    explicit OcrTesseracrEngine(); 
    ~OcrTesseracrEngine();

    QString inputFile()             const;
    QString outputFile()            const;
    QString outputText()            const; 

    void setInputFile(const QString& filePath);
    void setOutputFile(const QString& filePath);

    void setLanguagesMode(int mode);
    int  languagesMode()        const;

    void setPSMMode(int mode);
    int  PSMMode()              const;

    void setOEMMode(int mode);
    int  OEMMode()              const;

    void setDpi(int value);
    int  Dpi()              const;

    void setIsSaveTextFile(bool check);
    bool isSaveTextFile() const;

    bool      runOcrProcess();
     
private:

    // Disable
    
    OcrTesseracrEngine(const OcrTesseracrEngine&)            = delete;
    OcrTesseracrEngine& operator=(const OcrTesseracrEngine&) = delete;

private:

    void SaveOcrResult();

private:

    class Private;
    Private* const d; 

};

} // namespace DigikamGenericTextConverterPlugin

#endif // OCR_TESSERACT_ENGINE_H