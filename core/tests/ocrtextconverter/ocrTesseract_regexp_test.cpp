// Qt includes 

#include <QtGui>
#include <QApplication>
#include <QTextEdit>
#include <QUrl>
#include <QString>
#include <QTextImageFormat>
#include <QTabWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>

// KDE includes 

#include <KTextEdit>
#include <KXmlGuiWindow>
#include <KProcess>

// local includes 
 

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
  
    QString opt = QLatin1String("list-langs");

    KProcess process; 
    process.setOutputChannelMode(KProcess::MergedChannels);
    process << QLatin1String("tesseract") << QLatin1String("--%1").arg(opt);

    process.execute(5000);

    const QByteArray output = process.readAllStandardOutput();
    const QList<QByteArray> lines = output.split('\n');
    
    for (const QByteArray& line : lines)
    {
        const QString lineStr = QString::fromLocal8Bit(line);

        QRegExp rx;

        if (opt == QLatin1String("list-langs")) 
            rx.setPattern(QLatin1String("^\\s*(\\w+)()$"));
        else 
            rx.setPattern(QLatin1String("^\\s*(\\d+)\\s+(\\w.+)?$"));
        
        if (rx.indexIn(lineStr)>-1)
        {
            const QString value = rx.cap(1);
            qDebug() << value;
            QString desc = rx.cap(2).simplified();
            if (desc.endsWith(QLatin1Char('.')) || desc.endsWith(QLatin1Char(','))) 
            desc.chop(1);
        }
    }

    return app.exec();
}

   