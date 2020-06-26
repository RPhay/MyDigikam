/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-07
 * Description : Database interface for face recognition
 *
 * Copyright (C) 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "facedatabase.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

namespace RecognitionTest
{
class Q_DECL_HIDDEN FaceDatabase::Private
{
public:
    Private()
        : db(QSqlDatabase::addDatabase(QLatin1String("QSQLITE")))
    {
        db.setDatabaseName(QLatin1String(":memory:"));
        if (!db.open())
        {
            qFatal("Unable to establish a database connection");
        }

        query.exec(QLatin1String("SET sql_notes = 0"));
        query.exec(QLatin1String("CREATE TABLE IF NOT EXISTS create table identity (id int primary key, label varchar(20))"));
        query.exec(QLatin1String("SET sql_notes = 1"));
    }

    ~Private()
    {
        db.close();
    }

public:

    QSqlDatabase db;
    QSqlQuery    query;
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



