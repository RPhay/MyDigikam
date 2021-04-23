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

#include "metaengine.h"

using namespace Digikam;

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    qDebug() << "Extract Exiv2 tags list...";

    MetaEngine meta;
    MetaEngine::TagsMap exiv2Exif = meta.getStdExifTagsList();
    MetaEngine::TagsMap exiv2Iptc = meta.getIptcTagsList();
    MetaEngine::TagsMap exiv2Xmp  = meta.getXmpTagsList();
    QStringList tagsFound;

    qDebug() << "Extract ExifTool database as XML...";

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

    qDebug() << "Parse ExifTool tags database...";

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
                    QString exiv2TagPre;
                    QString name  = e2.attribute(QLatin1String("name"));
                    QString g11   = e2.attribute(QLatin1String("g1"));

                    if (!g11.isEmpty())
                    {
                        g1 = g11;
                    }

                    QString g22   = e2.attribute(QLatin1String("g2"));

                    if (!g22.isEmpty())
                    {
                        g2 = g22;
                    }

                    QString etTag = QString::fromLatin1("%1.%2.%3.%4").arg(g0).arg(g1).arg(g2).arg(name);

                    // Exif tags

                    if      (etTag.startsWith(QLatin1String("EXIF.InteropIFD")))
                    {
                        exiv2TagPre = QString::fromLatin1("Exif.Iop.");
                    }
                    else if (etTag.startsWith(QLatin1String("EXIF.IFD0")))
                    {
                        exiv2TagPre = QString::fromLatin1("Exif.Image.");
                    }
                    else if (etTag.startsWith(QLatin1String("EXIF.ExifIFD")))
                    {
                        exiv2TagPre = QString::fromLatin1("Exif.Photo.");
                    }
                    else if (etTag.startsWith(QLatin1String("EXIF.IFD1")))
                    {
                        exiv2TagPre = QString::fromLatin1("Exif.Thumbnail.");
                    }
                    else if (etTag.startsWith(QLatin1String("EXIF.GPS")))
                    {
                        exiv2TagPre = QString::fromLatin1("Exif.GPSInfo.");
                    }

                    QString left  = QString::fromLatin1("mapETtoExiv2.insert(QLatin1String(\"%1\"),").arg(etTag);

                    if (!exiv2TagPre.isEmpty() && etTag.startsWith(QLatin1String("EXIF.")))
                    {
                        if (exiv2Exif.contains(exiv2TagPre + name))
                        {
                            tagsFound << QString::fromLatin1("%1%2").arg(left, -90)
                                                                    .arg(QString::fromLatin1("QLatin1String(\"%1\"));").arg(exiv2TagPre + name));
                        }
                        else
                        {
                            tagsFound << QString::fromLatin1("%1%2").arg(left, -90)
                                                                    .arg(QString::fromLatin1("QLatin1String(\"%1\"));").arg(exiv2TagPre));
                        }

                        continue;
                    }

                    // Iptc tags

                    else if (etTag.startsWith(QLatin1String("IPTC.")))
                    {
                        exiv2TagPre = QString::fromLatin1("Iptc.Envelope.");

                        if (exiv2Iptc.contains(exiv2TagPre + name))
                        {
                            tagsFound << QString::fromLatin1("%1%2").arg(left, -90)
                                                                    .arg(QString::fromLatin1("QLatin1String(\"%1\"));").arg(exiv2TagPre + name));
                            continue;
                        }

                        exiv2TagPre = QString::fromLatin1("Iptc.Application2.");

                        if (exiv2Iptc.contains(exiv2TagPre + name))
                        {
                            tagsFound << QString::fromLatin1("%1%2").arg(left, -90)
                                                                    .arg(QString::fromLatin1("QLatin1String(\"%1\"));").arg(exiv2TagPre + name));
                            continue;
                        }

                        tagsFound << QString::fromLatin1("%1%2").arg(left, -90)
                                                                .arg(QString::fromLatin1("QLatin1String(\"%1\"));").arg(exiv2TagPre));
                        continue;
                    }

                    // Xmp tags

                    else if (etTag.startsWith(QLatin1String("XMP.XMP-dc")))
                    {
                        exiv2TagPre = QString::fromLatin1("Xmp.dc.");

                        if (exiv2Xmp.contains(exiv2TagPre + name))
                        {
                            tagsFound << QString::fromLatin1("%1%2").arg(left, -90)
                                                                    .arg(QString::fromLatin1("QLatin1String(\"%1\"));").arg(exiv2TagPre + name));
                            continue;
                        }

                        tagsFound << QString::fromLatin1("%1%2").arg(left, -90)
                                                                .arg(QString::fromLatin1("QLatin1String(\"%1\"));").arg(exiv2TagPre));
                        continue;
                    }

                }
            }
        }
    }

    tagsFound.sort();
    tagsFound.removeDuplicates();

    qDebug() << endl << "----------------------------------------------------------------------------------";
    qDebug() << "ExifTool tags found in Exiv2 (" << tagsFound.size() << "):" << endl;

    foreach (const QString& s, tagsFound)
    {
        qDebug().noquote() << s;
    }

    qDebug() << endl << "----------------------------------------------------------------------------------";
    qDebug() << "Found:"     << tagsFound.size();

    return 0;
}
