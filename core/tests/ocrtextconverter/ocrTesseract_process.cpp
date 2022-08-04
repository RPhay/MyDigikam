#include "ocrtesseractengine.h"

// Qt includes 

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QDir>

using namespace DigikamGenericTextConverterPlugin;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    OcrTesseracrEngine* ocrEngine = new OcrTesseracrEngine();

    ocrEngine->setInputFile(QLatin1String("/home/tranquochung/Desktop/repositories/digikam/core/tests/ocrtextconverter/data/scanned_img.png"));
    ocrEngine->setOutputFile(QLatin1String("/home/tranquochung/Desktop/repositories/digikam/core/tests/ocrtextconverter/data/output"));
    bool check = ocrEngine->runOcrProcess();

    delete ocrEngine;
    return app.exec();
}
