
// Qt includes

#include <QApplication>
#include <QCommandLineParser>
#include <QMainWindow>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QImage>
#include <QElapsedTimer>
#include <QLabel>
#include <QPen>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QHash>
#include <QFileInfo>
#include <QRandomGenerator>

// Local includes

#include "digikam_debug.h"
#include "dnnbasedetectormodel.h"
#include "autotagsassign.h"

// C++ include
#include <iostream>


using namespace Digikam;

QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* const parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QStringList() << QLatin1String("dataset"), QLatin1String("Data set folder"), QLatin1String("path relative to data folder")));
    parser->addHelpOption();
    parser->process(app);

    return parser;
}


int main(int argc, char** argv)
{   
    QApplication app(argc, argv);
    // Options for commandline parser

    QCommandLineParser* const parser = parseOptions(app);
    if (!parser->isSet(QLatin1String("dataset")))
    {
        qWarning("Data set is not set !!!");
    }
 
    QDir dataset(parser->value(QLatin1String("dataset"))); 
    QFileInfoList filesInfo = dataset.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    QScopedPointer<autoTagsAssign> autotagsEngine (new autoTagsAssign());
    QList<QList<QString>> tags;

    int batch_size   = 1;
    int sizeData = 54;
    
    // 54 eles 0...53
    // i = 0, 0...9 || i = 40, 40...49 || i = 50, 50...53  
    for (int i = 0; i <= sizeData - sizeData%batch_size - batch_size; i += batch_size)
    {
        QList<QString> bacthImgs;
        for (int j = 0; j < batch_size; j++)
        {
            bacthImgs.push_back(filesInfo[i+j].absoluteFilePath());
        }
        tags += autotagsEngine->generateTagsList(bacthImgs, batch_size);
    }
    
    QList<QString> bacthImgs;
    for (int i = sizeData - sizeData%batch_size; i < sizeData; i++)
    {
        bacthImgs.push_back(filesInfo[i].absoluteFilePath());
    }
    tags += autotagsEngine->generateTagsList(bacthImgs, batch_size);

    for (int i = 0; i < sizeData; i++)
    {   
        qDebug() << "Path: " << filesInfo[i].absoluteFilePath();
        qDebug() << tags[i] << "\n";
    }
    
    return app.exec();
}