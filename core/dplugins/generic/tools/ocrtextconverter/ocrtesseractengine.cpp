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
      : language ((int)OcrOptions::Languages::DEFAULT),
        psm      ((int)OcrOptions::PageSegmentationModes::DEFAULT),
        oem      ((int)OcrOptions::EngineModes::DEFAULT)
    {
    }

    int        language;
    int        psm;
    int        oem; 
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
    d->cancel = false;
    QProcess* const ocrProcess = new QProcess();
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

        if (!d->outputFile.isEmpty())
        {
            args << d->outputFile;
        }  
        else
        {
            args << QLatin1String("stdout");
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
            delete ocrProcess;
            return PROCESS_FAILED;
        }

        if (d->cancel)
        {
            delete ocrProcess;
            return PROCESS_CANCELED;
        }

    }
    catch(const QProcess::ProcessError& e)
    {   
        qWarning() << "Text Converter has error" << e;
        delete ocrProcess;
        return PROCESS_FAILED;
    }
    
    
    d->ocrResult   = QString::fromLocal8Bit(ocrProcess->readAllStandardOutput());
    delete ocrProcess;

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

