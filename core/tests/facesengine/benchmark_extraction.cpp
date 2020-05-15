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
#include <QElapsedTimer>
#include <QLabel>
#include <QPen>
#include <QPainter>
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

private Q_SLOTS:

    void slotDetectFaces(const QListWidgetItem* imageItem);

private:
    FaceDetector m_detector;
    QLabel*      m_fullImage;

    QListWidget* m_imageListView;

    QHBoxLayout* m_processingLayout;
};

MainWindow::MainWindow(const QDir &directory, QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QLatin1String("Face detection Test"));

    QWidget* const mainWidget = new QWidget;

    // Image erea
    QWidget* const imageArea = new QWidget;

    m_fullImage = new QLabel;
    m_fullImage->setScaledContents(true);
    m_fullImage->setText(QLatin1String("place holder"));

    QWidget* const metaDataArea = new QWidget;

    QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spLeft.setVerticalPolicy(QSizePolicy::Expanding);
    m_fullImage->setSizePolicy(spLeft);

    QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spRight.setVerticalPolicy(QSizePolicy::Preferred);
    metaDataArea->setSizePolicy(spRight);


    m_processingLayout = new QHBoxLayout(imageArea);
    m_processingLayout->addWidget(m_fullImage);
    //m_processingLayout->addWidget(metaDataArea);




    // Itemlist erea
    QScrollArea* const scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignBottom);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);







    QSizePolicy spHigh(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spHigh.setVerticalPolicy(QSizePolicy::Expanding);
    imageArea->setSizePolicy(spHigh);

    QSizePolicy spLow(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spLow.setVerticalPolicy(QSizePolicy::Fixed);
    scrollArea->setSizePolicy(spLow);

    QVBoxLayout* const mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(imageArea);
    mainLayout->addWidget(scrollArea);

    m_imageListView = new QListWidget(mainWidget);
    m_imageListView->setViewMode(QListView::IconMode);
    m_imageListView->setIconSize(QSize(200, 150));
    m_imageListView->setResizeMode(QListWidget::Adjust);
    m_imageListView->setFlow(QListView::LeftToRight);
    m_imageListView->setWrapping(false);
    m_imageListView->setDragEnabled(false);

    connect(m_imageListView, &QListWidget::currentItemChanged,
            this,            &MainWindow::slotDetectFaces);

    QStringList subjects = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);

    for (QStringList::const_iterator iter  = subjects.cbegin();
                                     iter != subjects.cend();
                                   ++iter)
    {
        QString filePath = directory.filePath(*iter);
        QListWidgetItem* item = new QListWidgetItem(QIcon(filePath), filePath);

        m_imageListView->addItem(item);
    }

    scrollArea->setWidget(m_imageListView);

    setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{
}

void MainWindow::slotDetectFaces(const QListWidgetItem* imageItem)
{
    QString imagePath = imageItem->text();
    qDebug() << "Loading " << imagePath;
    QImage img(imagePath);
    QImage imgScaled(img.scaled(416, 416, Qt::KeepAspectRatio));

    QElapsedTimer timer;
    unsigned int elapsedDetection = 0;

    timer.start();
    QList<QRectF> faces = m_detector.detectFaces(imagePath);

    elapsedDetection = timer.elapsed();

    qDebug() << "(Input CV) Found " << faces.size() << " faces, in " << elapsedDetection << "ms";

    if (faces.isEmpty())
    {
        qDebug() << "No faces found";
        return;
    }

    qDebug() << "Coordinates of detected faces : ";

    foreach (const QRectF& r, faces)
    {
        qDebug() << r;
    }

    QPainter painter(&imgScaled);
    QPen paintPen(Qt::green);
    paintPen.setWidth(1);
    painter.setPen(paintPen);

    foreach (const QRectF& rr, faces)
    {
        QLabel* const label = new QLabel;
        label->setScaledContents(false);
        QRect rectDraw      = FaceDetector::toAbsoluteRect(rr, imgScaled.size());
        QRect r             = FaceDetector::toAbsoluteRect(rr, img.size());
        QImage part         = img.copy(r);
        label->setPixmap(QPixmap::fromImage(part.scaled(qMin(img.size().width(), 50),
                                                        qMin(img.size().width(), 50),
                                                        Qt::KeepAspectRatio)));
        m_processingLayout->addWidget(label);
        painter.drawRect(rectDraw);
    }

    // Only setPixmap after finishing drawing bboxes around detected faces
    m_fullImage->setPixmap(QPixmap::fromImage(imgScaled));
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


