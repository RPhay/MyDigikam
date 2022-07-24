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

// local includes 

#include "ocrtextedit.h"

class KTextEdit;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    KTextEdit* edit = new KTextEdit();
    edit->show();
    
    return app.exec();
}

   