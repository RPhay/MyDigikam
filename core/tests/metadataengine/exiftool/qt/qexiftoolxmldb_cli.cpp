/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-28
 * Description : a command line tool to parse ExifTool XML database
 *
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes

#include <QString>
#include <QStringList>
#include <QDomDocument>
#include <QDomElement>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QObject>
#include <QProcess>

// Local includes

#include "exiftoolparser.h"
#include "exiftooltranslator.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QProcess proc;
    QString program = QLatin1String("/usr/bin/exiftool");
    QStringList arguments;
    arguments << QLatin1String("-listx") << QLatin1String("-n");
    proc.start(program, arguments);
    bool ret = proc.waitForFinished();

    if (!ret)
    {
        return -1;
    }

    QByteArray xml = proc.readAllStandardOutput();

    QDomDocument doc(QLatin1String("ExifToolDB"));

    if (!doc.setContent(xml))
    {
        qDebug() << "Cannot open XML DB contents";
        return false;
    }

    QDomElement docElem = doc.documentElement();

    if (docElem.tagName() != QLatin1String("taginfo"))
    {
        qDebug() << "XML DB do not content taginfo entry";
        return false;
    }

    for (QDomNode n1 = docElem.firstChild() ; !n1.isNull() ; n1 = n1.nextSibling())
    {
        QDomElement e1 = n1.toElement();

        if (e1.isNull())
        {
             continue;
        }

        if (e1.tagName() == QLatin1String("table"))
        {
            QString g0  = e1.attribute(QLatin1String("g0"));
            QString g1  = e1.attribute(QLatin1String("g1"));
            QString g2  = e1.attribute(QLatin1String("g2"));

            for (QDomNode n2 = e1.firstChild() ; !n2.isNull() ; n2 = n2.nextSibling())
            {
                QDomElement e2 = n2.toElement();

                if (e2.isNull())
                {
                    continue;
                }

                if (e2.tagName() == QLatin1String("tag"))
                {
                    QString name = e2.attribute(QLatin1String("name"));
                    qDebug().noquote() << QString::fromLatin1("%1.%2.%3.%4").arg(g0).arg(g1).arg(g2).arg(name);
                }
            }
        }
    }

    return 0;
}
