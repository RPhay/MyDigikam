/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-05-13
 * Description : CLI tool to test and benchmark Face Extractor of face engines
 *
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

// Qt includes
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QImage>

// lib digikam includes
#include "facedetector.h"

using namespace Digikam;


int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));          // for DB init.

    // Options for commandline parser

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QLatin1String("database"), QLatin1String("Faces database"), QLatin1String("path to db folder")));
    parser.addOption(QCommandLineOption(QLatin1String("split-ratio"), QLatin1String("Split ratio (test set / whole set)"), QLatin1String("decimal")));
    parser.addOption(QCommandLineOption(QLatin1String("test-set"), QLatin1String("Test set folder"), QLatin1String("path relative to db folder")));
    parser.addOption(QCommandLineOption(QLatin1String("dev-set"), QLatin1String("Training set (dev set) folder"), QLatin1String("path relative to db folder")));
    parser.addOption(QCommandLineOption(QLatin1String("nb-label"), QLatin1String("Number of total objects"), QLatin1String("nbIdentities")));
    parser.addOption(QCommandLineOption(QLatin1String("samples-per-label"), QLatin1String("Number of samples per object"), QLatin1String("nbSamples")));
    parser.addOption(QCommandLineOption(QLatin1String("allset"), QLatin1String("Option to run test on the entire set")));
    parser.addHelpOption();
    parser.process(app);

    // Parse arguments

    bool optionErrors = false;

    if (parser.optionNames().empty())
    {
        qWarning() << "NO options!!!";
        optionErrors = true;
    }
    else if (!parser.isSet(QLatin1String("database")))
    {
        qWarning() << "MISSING database for test!!!";
        optionErrors = true;
    }
    else if (!parser.isSet(QLatin1String("allset")) &&
            (!parser.isSet(QLatin1String("nb-label")) || !parser.isSet(QLatin1String("samples-per-label"))))
    {
        qWarning() << "UNKNOWN training set / test set separation!!!";
        optionErrors = true;
    }
    else if (parser.isSet(QLatin1String("test-set")) && !parser.isSet(QLatin1String("dev-set")))
    {
        qWarning() << "UNKNOWN Dev set!!!";
        optionErrors = true;
    }
    else if (parser.isSet(QLatin1String("dev-set")) && !parser.isSet(QLatin1String("test-set")))
    {
        qWarning() << "UNKNOWN Test set!!!";
        optionErrors = true;
    }

    if (optionErrors)
    {
        parser.showHelp();
        return 1;
    }

    QString facedb              = parser.value(QLatin1String("database"));
    unsigned int nbOfSamples    = 0;
    unsigned int nbOfIdentities = 0;

    if (!parser.isSet(QLatin1String("allset")))
    {
        nbOfSamples    = parser.value(QLatin1String("samples-per-label")).toUInt();
        nbOfIdentities = parser.value(QLatin1String("nb-label")).toUInt();
    }

    double ratio = 0;

    if (parser.isSet(QLatin1String("split-ratio")))
    {
        ratio = parser.value(QLatin1String("split-ratio")).toDouble();
    }

    // Init config for digiKam

}
