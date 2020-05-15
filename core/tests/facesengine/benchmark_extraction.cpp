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
#include <QMainWindow>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QImage>
#include <QLabel>
#include <QDebug>

// lib digikam includes
#include "facedetector.h"

using namespace Digikam;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QDir& directory, QWidget *parent = nullptr);
    ~MainWindow();
};

MainWindow::MainWindow(const QDir &directory, QWidget *parent)
    : QMainWindow(parent)
{
    QWidget* const mainWidget = new QWidget;

    QLabel* const fullImage = new QLabel;
    fullImage->setScaledContents(false);

    QScrollArea* const scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignBottom);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);


    QSizePolicy spHigh(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spHigh.setVerticalPolicy(QSizePolicy::Expanding);
    fullImage->setSizePolicy(spHigh);

    QSizePolicy spLow(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spLow.setVerticalPolicy(QSizePolicy::Fixed);
    scrollArea->setSizePolicy(spLow);

    QVBoxLayout* const layout = new QVBoxLayout(mainWidget);
    layout->addWidget(fullImage);
    layout->addWidget(scrollArea);

    QListWidget* imageListView = new QListWidget(mainWidget);
    imageListView->setViewMode(QListView::IconMode);
    imageListView->setIconSize(QSize(200, 150));
    imageListView->setResizeMode(QListWidget::Adjust);
    imageListView->setFlow(QListView::LeftToRight);
    imageListView->setWrapping(false);

    QStringList subjects = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);

    for (QStringList::const_iterator iter  = subjects.cbegin();
                                     iter != subjects.cend();
                                   ++iter)
    {
        QString filePath = directory.filePath(*iter);
        QListWidgetItem* item = new QListWidgetItem(QIcon(filePath), filePath);

        imageListView->addItem(item);
    }

    scrollArea->setWidget(imageListView);

    setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{
}


QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* parser = new QCommandLineParser();

    //parser->addOption(QCommandLineOption(QLatin1String("database"), QLatin1String("Faces database"), QLatin1String("path to db folder")));
    //parser->addOption(QCommandLineOption(QLatin1String("split-ratio"), QLatin1String("Split ratio (test set / whole set)"), QLatin1String("decimal")));
    parser->addOption(QCommandLineOption(QLatin1String("data-set"), QLatin1String("Data set folder"), QLatin1String("path relative to data folder")));
    //parser->addOption(QCommandLineOption(QLatin1String("dev-set"), QLatin1String("Training set (dev set) folder"), QLatin1String("path relative to db folder")));
    //parser->addOption(QCommandLineOption(QLatin1String("nb-label"), QLatin1String("Number of total objects"), QLatin1String("nbIdentities")));
    //parser->addOption(QCommandLineOption(QLatin1String("samples-per-label"), QLatin1String("Number of samples per object"), QLatin1String("nbSamples")));
    //parser->addOption(QCommandLineOption(QLatin1String("allset"), QLatin1String("Option to run test on the entire set")));
    //parser->addHelpOption();

    parser->process(app);

    return parser;
}


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QString::fromLatin1("digikam"));          // for DB init.

    // Options for commandline parser

   QCommandLineParser* parser = parseOptions(app);

   if (! parser->isSet(QLatin1String("data-set")))
   {
       qWarning("Data set is not set !!!");

       return 1;
   }

   QDir dataset(parser->value(QLatin1String("data-set")));

   MainWindow* window = new MainWindow(dataset, nullptr);

   window->show();

   return app.exec();
}

#include "benchmark_extraction.moc"


