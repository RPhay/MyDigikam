
// Qt includes 

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QDir>

int main(int argc, char* argv[])
{
    
    QProcess *ocrProcess = new QProcess();
//    ocrProcess->setProcessChannelMode(QProcess::MergedChannels);

    
    // ------------------------- IN/OUT ARGUMENTS -------------------------

    QString pathImage       = QLatin1String("../../core/tests/text_convert/data/");
    QString pathToSaveText  = QLatin1String("../../core/tests/text_convert/data/");
    QString inputFileName   = pathImage.append(QLatin1String("scanned_img.png"));
    QString outputTextName  = pathToSaveText.append(QLatin1String("scanned_img_text"));
  
    QStringList args;

    // add configuration image

    if (!inputFileName.isEmpty())
    {
        args << inputFileName;
    }  

    // output base name

    QString mess;

    if (!outputTextName.isEmpty())
    {
        args << outputTextName;
        mess = QLatin1String("Text is saved is file : %1").arg(outputTextName);
    }  
    else
    {
        args << QLatin1String("stdout");
        mess =  QLatin1String(ocrProcess->readAllStandardOutput());
        
    }

    // ----------------------------- OPTIONS -----------------------------

    QString lang       = QLatin1String("eng");
    QString psm        = QLatin1String("3");
    QString oem        = QLatin1String("3");
    QString dpi        = QLatin1String("300");
    QUrl userWords     = QUrl(QLatin1String(" "));                    
    QUrl userPatterns  = QUrl(QLatin1String(" "));                    

    // User words 
    if (!userWords.isValid())
    {
        args << QLatin1String("--user-words") << userWords.toLocalFile();
    }

    // User patterns
    if (!userPatterns.isValid())
    {
        args << QLatin1String("--user-patterns") << userPatterns.toLocalFile();
    }

    // page Segmentation mode 
    if (!psm.isEmpty())
    {
        args << QLatin1String("--psm") << psm;
    }    
    // OCR enginge mode 
    if (!oem.isEmpty())
    {
        args << QLatin1String("--oem") << oem;
    }
    // Language
    if (!lang.isEmpty())
    {
        args << QLatin1String("-l") << lang;
    }

    // Specify DPI for input image
    // Tesseract work best with 300 dpi for input image
    if (!dpi.isEmpty())
    {
        args << QLatin1String("--dpi") << dpi;
    }

    // ------------------  Running tesseract process ------------------

    const QString cmd = QLatin1String("tesseract");
    
    ocrProcess->setProgram(cmd);
    ocrProcess->setArguments(args);


    qDebug() << "Running OCR : "
             << ocrProcess->program() 
             << ocrProcess->arguments();

    ocrProcess->start();

    bool successFlag    = ocrProcess->waitForFinished(-1) && ocrProcess->exitStatus() == QProcess::NormalExit;
    
    if (!successFlag)
    {
        qWarning() << "Error starting OCR Process";
        return 0;
    }

    qDebug() << mess;

    return 0;
}