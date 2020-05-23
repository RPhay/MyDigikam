#include "facedatabase.h"

#include <QSqlDatabase>
#include <QDebug>

namespace RecognitionTest
{
class Q_DECL_HIDDEN FaceDatabase::Private
{
public:
    Private()
    {
        db = QSqlDatabase::addDatabase(QLatin1String("QSQLITE"));
        db.setHostName(QLatin1String("localhost"));
        db.setDatabaseName(QLatin1String("testfacedb"));
        db.setUserName(QLatin1String("admin"));
        db.setPassword(QLatin1String("admin"));
        if (db.open())
        {
            qDebug() << "Open Face database";
        }
    }

    ~Private()
    {
    }

public:

    QSqlDatabase db;
};


FaceDatabase::FaceDatabase()
    : d(new Private())
{
}

FaceDatabase::~FaceDatabase()
{
    delete d;
}

}



