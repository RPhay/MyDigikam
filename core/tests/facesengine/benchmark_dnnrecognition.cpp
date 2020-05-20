/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-05-20
 * Description : Testing tool for dnn face recognition of face engines
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
#include "opencvdnnfacedetector.h"
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

    void showCVMat(const cv::Mat& cvimage);
    QList<QRectF> detectFaces(const QString& imagePath);
    void extractFaces(const QImage& img, QImage& imgScaled, const QList<QRectF>& faces);

private:

    OpenCVDNNFaceDetector* m_detector;

    QLabel*                m_fullImage;
    QListWidget*           m_imageListView;
    QVBoxLayout*           m_croppedfaceLayout;
};

MainWindow::MainWindow(const QDir &directory, QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QLatin1String("Face detection Test"));

    m_detector = new OpenCVDNNFaceDetector(DetectorNNModel::YOLO);

    QWidget* const mainWidget = new QWidget(this);

    // Image erea
    QWidget* const imageArea = new QWidget(mainWidget);

    m_fullImage = new QLabel;
    m_fullImage->setScaledContents(true);

    QScrollArea* facesArea = new QScrollArea(this);
    m_croppedfaceLayout = new QVBoxLayout(facesArea);
    facesArea->setLayout(m_croppedfaceLayout);
    facesArea->setWidgetResizable(true);
    facesArea->setAlignment(Qt::AlignRight);
    facesArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    facesArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // TODO add control panel to adjust detection hyper parameters
    QWidget* const controlPanel = new QWidget;

    QSizePolicy spImage(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spImage.setVerticalPolicy(QSizePolicy::Expanding);
    m_fullImage->setSizePolicy(spImage);


    QSizePolicy spFaces(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spFaces.setVerticalPolicy(QSizePolicy::Expanding);
    facesArea->setSizePolicy(spFaces);

    QSizePolicy spControl(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spControl.setVerticalPolicy(QSizePolicy::Expanding);
    controlPanel->setSizePolicy(spControl);

    QHBoxLayout* processingLayout = new QHBoxLayout(imageArea);
    processingLayout->addWidget(m_fullImage);
    processingLayout->addWidget(facesArea);
    processingLayout->addWidget(controlPanel);

    // Itemlist erea
    QScrollArea* const itemsArea = new QScrollArea;
    itemsArea->setWidgetResizable(true);
    itemsArea->setAlignment(Qt::AlignBottom);
    itemsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    itemsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    QSizePolicy spHigh(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spHigh.setVerticalPolicy(QSizePolicy::Expanding);
    imageArea->setSizePolicy(spHigh);

    QSizePolicy spLow(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spLow.setVerticalPolicy(QSizePolicy::Fixed);
    itemsArea->setSizePolicy(spLow);

    QVBoxLayout* const mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->addWidget(imageArea);
    mainLayout->addWidget(itemsArea);

    m_imageListView = new QListWidget(mainWidget);
    m_imageListView->setViewMode(QListView::IconMode);
    m_imageListView->setIconSize(QSize(200, 150));
    m_imageListView->setResizeMode(QListWidget::Adjust);
    m_imageListView->setFlow(QListView::LeftToRight);
    m_imageListView->setWrapping(false);
    m_imageListView->setDragEnabled(false);

    QStringList subjects = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);

    for (QStringList::const_iterator iter  = subjects.cbegin();
                                     iter != subjects.cend();
                                   ++iter)
    {
        QString filePath = directory.filePath(*iter);
        QListWidgetItem* item = new QListWidgetItem(QIcon(filePath), filePath);

        m_imageListView->addItem(item);
    }

    connect(m_imageListView, &QListWidget::currentItemChanged,
            this,            &MainWindow::slotDetectFaces);

    itemsArea->setWidget(m_imageListView);

    setCentralWidget(mainWidget);
}

MainWindow::~MainWindow()
{
    delete m_fullImage;
    delete m_imageListView;
    delete m_croppedfaceLayout;
}

void MainWindow::slotDetectFaces(const QListWidgetItem* imageItem)
{
    QString imagePath = imageItem->text();
    qDebug() << "Loading " << imagePath;
    QImage img(imagePath);
    QImage imgScaled(img.scaled(416, 416, Qt::KeepAspectRatio));

    // clear faces layout
    QLayoutItem *wItem;
    while ((wItem = m_croppedfaceLayout->takeAt(0)) != nullptr)
    {
        delete wItem->widget();
        delete wItem;
    }

    QList<QRectF> faces = detectFaces(imagePath);

    extractFaces(img, imgScaled, faces);

    // Only setPixmap after finishing drawing bboxes around detected faces
    m_fullImage->setPixmap(QPixmap::fromImage(imgScaled));
}


void MainWindow::showCVMat(const cv::Mat& cvimage)
{
    if(cvimage.cols*cvimage.rows != 0)
    {
        cv::Mat rgb;
        QPixmap p;
        cv::cvtColor(cvimage, rgb, (-2*cvimage.channels()+10));
        p.convertFromImage(QImage(rgb.data, rgb.cols, rgb.rows, QImage::Format_RGB888));
        //resize(cvimage.cols, cvimage.rows);
    }
}

QList<QRectF> MainWindow::detectFaces(const QString& imagePath)
{
    QImage img(imagePath);
/*
    cv::Mat img;
    img = cv::imread(imagePath.toStdString());

    if(! img.data )
    {
        qDebug() <<  "Open cv Could not open or find the image";
    }
*/
    QList<QRectF> faces;

    try
    {
        QElapsedTimer timer;
        unsigned int elapsedDetection = 0;
        timer.start();

        // NOTE detection with filePath won't work when format is not standard
        // NOTE unexpected behaviour with detecFaces(const QString&)
        cv::Size paddedSize(0, 0);
        cv::Mat cvImage       = m_detector->prepareForDetection(img, paddedSize);
        QList<QRect> absRects = m_detector->detectFaces(cvImage, paddedSize);
        faces                 = FaceDetector::toRelativeRects(absRects,
                                                              QSize(cvImage.cols - 2*paddedSize.width,
                                                              cvImage.rows - 2*paddedSize.height));
        elapsedDetection = timer.elapsed();


        // debug padded image
        //showCVMat(cvImage);

        qDebug() << "(Input CV) Found " << absRects.size() << " faces, in " << elapsedDetection << "ms";
    }
    catch (cv::Exception& e)
    {
        qWarning() << "cv::Exception:" << e.what();
    }
    catch(...)
    {
        qWarning() << "Default exception from OpenCV";
    }

    return faces;
}

void MainWindow::extractFaces(const QImage& img, QImage& imgScaled, const QList<QRectF>& faces)
{
    if (faces.isEmpty())
    {
        qWarning() << "No face detected";
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
        label->setPixmap(QPixmap::fromImage(part.scaled(qMin(img.size().width(), 100),
                                                        qMin(img.size().width(), 100),
                                                        Qt::KeepAspectRatio)));
        m_croppedfaceLayout->addWidget(label);
        painter.drawRect(rectDraw);
    }
}


QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("data-set"), QLatin1String("Data set folder"), QLatin1String("path relative to data folder")));
    parser->addHelpOption();
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

#include "benchmark_dnnrecognition.moc"

