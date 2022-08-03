#include "ocrtesseractengine.h"

// Qt includes 

#include <QString>
#include <QProcess>
#include <QUrl>

// local includes 

#include "ocroptions.h"

namespace DigikamGenericTextConverterPlugin
{

class  OcrTesseracrEngine::Private
{

public:

    Private()
      : language ((int)OcrOptions::Languages::ENG),
        psm      ((int)OcrOptions::PageSegmentationModes::FULLY_AUTO_PAGE),
        oem      ((int)OcrOptions::EngineModes::DEFAULT),
        ocrProcess (nullptr)
    {
    }

    int language;
    int psm;
    int oem; 

    QProcess* ocrProcess;

    QString   inputFile;
    QString   outputFile;
    QString   ocrResult;  
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

void OcrTesseracrEngine::initOcrProcess()
{
    if (d->ocrProcess != nullptr)
    {
        delete d->ocrProcess;
    }

    d->ocrProcess = new QProcess();
    d->ocrProcess->setProcessChannelMode(QProcess::MergedChannels);   
}

QProcess* OcrTesseracrEngine::ocrProcess() const
{
    return d->ocrProcess;
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

bool OcrTesseracrEngine::runOcrProcess()
{
    initOcrProcess();
    
    // ------------------------- IN/OUT ARGUMENTS -------------------------
    
    QStringList args; 
    
    // add configuration image

    if (!d->inputFile.isEmpty())
    {
        args << d->inputFile;
    }  

    // output base name

    QString mess;

    if (!d->outputFile.isEmpty())
    {
        args << d->outputFile;
    }  
    else
    {
        args << QLatin1String("stdout");
        d->ocrResult   = QString::fromLocal8Bit(d->ocrProcess->readAllStandardOutput());
        qDebug() << (d->ocrResult);     
    }

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

    val = QLatin1String("300");
    if (!val.isEmpty())
    {
        args << QLatin1String("--dpi") << val;
    }

    // ------------------  Running tesseract process ------------------

    const QString cmd = QLatin1String("tesseract");
   
    d->ocrProcess->setProgram(cmd);
    d->ocrProcess->setArguments(args);
   
    qDebug() << "Running OCR : "
             << d->ocrProcess->program() 
             << d->ocrProcess->arguments();

    d->ocrProcess->start();

    bool successFlag = d->ocrProcess->waitForFinished(-1) && d->ocrProcess->exitStatus() == QProcess::NormalExit;
    
    if (!successFlag)
    {
        qWarning() << "Error starting OCR Process";
        return PROCESS_FAILED;
    }
    
    return PROCESS_COMPLETE;
}


/**

void DarkTableRawImportPlugin::slotErrorOccurred(QProcess::ProcessError error)
{
    switch (error)
    {
        case QProcess::FailedToStart:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process has failed to start";
            break;
        }

        case QProcess::Crashed:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process has crashed";
            break;
        }

        case QProcess::Timedout:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process time-out";
            break;
        }

        case QProcess::WriteError:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process write error";
            break;
        }

        case QProcess::ReadError:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process read error";
            break;
        }

        default:
        {
            qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: Process error unknown";
            break;
        }
    }
}

void DarkTableRawImportPlugin::slotProcessFinished(int code, QProcess::ExitStatus status)
{
    qCDebug(DIGIKAM_GENERAL_LOG) << "DarkTable :: return code:" << code << ":: Exit status:" << status;

    d->decoded = DImg(d->tempName);
    d->decoded.setAttribute(QLatin1String("isReadOnly"), true);

    if (d->decoded.isNull())
    {
        QString message = i18n("Error to import RAW image with DarkTable\nClose this dialog to load RAW image with native import tool");
        QMessageBox::information(nullptr, qApp->applicationName(), message);

        qCDebug(DIGIKAM_GENERAL_LOG) << "Decoded image is null! Load with Native tool...";
        qCDebug(DIGIKAM_GENERAL_LOG) << d->props.filePath;

        Q_EMIT signalLoadRaw(d->props);
    }
    else
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "Decoded image is not null...";
        qCDebug(DIGIKAM_GENERAL_LOG) << d->props.filePath;
        d->props = LoadingDescription(d->tempName, LoadingDescription::ConvertForEditor);

        FilterAction action(QLatin1String("darktable:RawConverter"), 1, FilterAction::DocumentedHistory);
        action.setDisplayableName(QString::fromUtf8(I18N_NOOP("DarkTable Raw Conversion")));
        d->decoded.addFilterAction(action);

        Q_EMIT signalDecodedImage(d->props, d->decoded);
    }

    delete d->darktable;
    d->darktable = nullptr;

    QFile::remove(d->tempName);
} 
  
**/

}    // namespace DigikamGenericTextConverterPlugin

