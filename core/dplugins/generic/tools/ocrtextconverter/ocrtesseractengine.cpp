#include "ocrtesseractengine.h"

// Qt includes 

#include <QString>
#include <QProcess>
#include <QUrl>
#include <QFileInfo>

// local includes 

#include "ocroptions.h"

namespace DigikamGenericTextConverterPlugin
{

class  OcrTesseracrEngine::Private
{

public:

    Private()
      : language        ((int)OcrOptions::Languages::DEFAULT),
        psm             ((int)OcrOptions::PageSegmentationModes::DEFAULT),
        oem             ((int)OcrOptions::EngineModes::DEFAULT),
        dpi             (300),
        isSaveTextFile  (true)
    {
    }

    int        language;
    int        psm;
    int        oem; 
    int        dpi;

    bool       isSaveTextFile;

    bool       cancel;
 
    QString    inputFile;
    QString    outputFile;
    QString    ocrResult;  
};

OcrTesseracrEngine::OcrTesseracrEngine()
    : d(new Private())
{
}

OcrTesseracrEngine::~OcrTesseracrEngine()
{
    delete d;
}

void OcrTesseracrEngine::setLanguagesMode(int mode)
{
    d->language = mode;
} 

int OcrTesseracrEngine::languagesMode() const 
{
    return d->language;
}

void OcrTesseracrEngine::setPSMMode(int mode)
{
    d->psm = mode;
}

int OcrTesseracrEngine::PSMMode() const 
{
    return d->psm;
}

void OcrTesseracrEngine::setOEMMode(int mode)
{
    d->oem = mode;
}

int OcrTesseracrEngine::OEMMode() const 
{
    return d->oem;
}

void OcrTesseracrEngine::setDpi(int value)
{
    d->dpi = value;
}

int OcrTesseracrEngine::Dpi() const
{
    return d->dpi;
}

QString OcrTesseracrEngine::inputFile() const
{
    return d->inputFile;
}

QString OcrTesseracrEngine::outputFile() const
{
    return d->outputFile;
}

QString OcrTesseracrEngine::outputText() const
{
  return d->ocrResult;
}

void OcrTesseracrEngine::setInputFile(const QString& filePath)
{
    d->inputFile = filePath;
}

void OcrTesseracrEngine::setOutputFile(const QString& filePath)
{
    d->outputFile = filePath;
}

void OcrTesseracrEngine::setIsSaveTextFile(bool check)
{
    d->isSaveTextFile = check;
}

bool OcrTesseracrEngine::isSaveTextFile() const
{
    return d->isSaveTextFile;
}

bool OcrTesseracrEngine::runOcrProcess()
{
    d->cancel = false;
    QScopedPointer<QProcess> ocrProcess (new QProcess());
    ocrProcess->setProcessChannelMode(QProcess::SeparateChannels);

    try
    {
        // ------------------------- IN/OUT ARGUMENTS -------------------------

        QStringList args; 

        // add configuration image

        if (!d->inputFile.isEmpty())
        {
            args << d->inputFile;
        }  

        // output base name

        QString mess;

        args << QLatin1String("stdout");

        // ----------------------------- OPTIONS -----------------------------

        OcrOptions ocropt;

        // page Segmentation mode 

        QString val = ocropt.PsmCodeToValue(static_cast<OcrOptions::PageSegmentationModes>(d->psm));
        if (!val.isEmpty())
        {
            args << QLatin1String("--psm") << val;
        }    

        // OCR enginge mode 

        val = ocropt.OemCodeToValue(static_cast<OcrOptions::EngineModes>(d->oem));
        if (!val.isEmpty())
        {
            args << QLatin1String("--oem") << val;
        }

        // Language

        val = ocropt.LanguageCodeToValue(static_cast<OcrOptions::Languages>(d->language));
        if (!val.isEmpty())
        {
            args << QLatin1String("-l") << val;
        }


        // dpi 

        val = QString::fromLatin1("%1").arg(d->dpi);
        if (!val.isEmpty())
        {
            args << QLatin1String("--dpi") << val;
        }

        // ------------------  Running tesseract process ------------------

        const QString cmd = QLatin1String("tesseract");
    
        ocrProcess->setProgram(cmd);
        ocrProcess->setArguments(args);
    
        qDebug() << "Running OCR : "
                 << ocrProcess->program() 
                 << ocrProcess->arguments();

        ocrProcess->start();

        bool successFlag =  ocrProcess->waitForFinished(-1) &&  ocrProcess->exitStatus() == QProcess::NormalExit;

        if (!successFlag)
        {
            qWarning() << "Error starting OCR Process";
            return PROCESS_FAILED;
        }

        if (d->cancel)
        {
            return PROCESS_CANCELED;
        }

    }
    catch(const QProcess::ProcessError& e)
    {   
        qWarning() << "Text Converter has error" << e;
        return PROCESS_FAILED;
    }
    
    
    d->ocrResult   = QString::fromLocal8Bit(ocrProcess->readAllStandardOutput());

    SaveOcrResult();

    return PROCESS_COMPLETE;
}

void OcrTesseracrEngine::SaveOcrResult()
{
    if (d->isSaveTextFile)
    {   
        QFileInfo fi(d->inputFile);
        d->outputFile =   fi.absolutePath() 
                          +  QLatin1String("/")
                          + (QString::fromLatin1("%1-textconverter.txt").arg(fi.fileName()));
        
        QFile file(d->outputFile);

        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream stream(&file);
            stream << d->ocrResult;
            file.close();
        }
    }

    // TODO option to save metadata into original image 
} 

}    // namespace DigikamGenericTextConverterPlugin

