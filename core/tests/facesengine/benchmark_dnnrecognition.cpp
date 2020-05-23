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
#include <QFormLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QDir>
#include <QImage>
#include <QElapsedTimer>
#include <QLabel>
#include <QPen>
#include <QPainter>
#include <QLineEdit>
#include <QDebug>


// lib digikam includes
#include "opencvdnnfacedetector.h"
#include "facedetector.h"
#include "faceextractor.h"
#include "facerecognizer.h"
#include "identity.h"

using namespace Digikam;
using namespace RecognitionTest;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    explicit MainWindow(const QDir& directory, QWidget *parent = nullptr);
    ~MainWindow();

private Q_SLOTS:

    void slotDetectFaces(const QListWidgetItem* imageItem);

private:

    QLabel* showCVMat(const cv::Mat& cvimage);
    QList<QRectF> detectFaces(const QString& imagePath);
    void extractFaces(const QImage& img, QImage& imgScaled, const QList<QRectF>& faces);

    QWidget* setupControlPanel();

private:

    OpenCVDNNFaceDetector* m_detector;
    FaceRecognizer*        m_recognizer;
    FaceExtractor*         m_extractor;

    QLabel*                m_fullImage;
    QListWidget*           m_imageListView;
    QVBoxLayout*           m_croppedfaceLayout;
    QVBoxLayout*           m_preprocessedLayout;
    QVBoxLayout*           m_alignedLayout;

    // control panel
    QLineEdit*             m_imageLabel;
    QLabel*                m_similarityLabel;
    QLabel*                m_recognizationInfo;
};

MainWindow::MainWindow(const QDir &directory, QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QLatin1String("Face recognition Test"));

    m_detector   = new OpenCVDNNFaceDetector(DetectorNNModel::YOLO);
    m_recognizer = new FaceRecognizer(true);
    m_extractor  = new FaceExtractor();

    QWidget* const mainWidget = new QWidget(this);

    // Image erea
    QWidget* const imageArea = new QWidget(mainWidget);

    m_fullImage = new QLabel;
    m_fullImage->setScaledContents(true);

    // cropped face area
    QScrollArea* facesArea = new QScrollArea(this);
    facesArea->setWidgetResizable(true);
    facesArea->setAlignment(Qt::AlignRight);
    facesArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    facesArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* croppedFacesWidgets = new QWidget(this);
    m_croppedfaceLayout          = new QVBoxLayout(this);

    croppedFacesWidgets->setLayout(m_croppedfaceLayout);
    facesArea->setWidget(croppedFacesWidgets);

    // preprocessed face area
    QScrollArea* preprocessedFacesArea = new QScrollArea(this);
    preprocessedFacesArea->setWidgetResizable(true);
    preprocessedFacesArea->setAlignment(Qt::AlignRight);
    preprocessedFacesArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    preprocessedFacesArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* preprocessedFacesWidgets = new QWidget(this);
    m_preprocessedLayout              = new QVBoxLayout(this);

    preprocessedFacesWidgets->setLayout(m_preprocessedLayout);
    preprocessedFacesArea->setWidget(preprocessedFacesWidgets);

    // aligned face area
    QScrollArea* alignedFacesArea = new QScrollArea(this);
    alignedFacesArea->setWidgetResizable(true);
    alignedFacesArea->setAlignment(Qt::AlignRight);
    alignedFacesArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    alignedFacesArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* alignedFacesWidgets = new QWidget(this);
    m_alignedLayout              = new QVBoxLayout(this);

    alignedFacesWidgets->setLayout(m_alignedLayout);
    alignedFacesArea->setWidget(alignedFacesWidgets);

    // TODO add control panel to adjust detection hyper parameters
    QWidget* controlPanel = setupControlPanel();

    QSizePolicy spImage(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spImage.setVerticalPolicy(QSizePolicy::Expanding);
    spImage.setHorizontalStretch(3);
    m_fullImage->setSizePolicy(spImage);

    spImage.setHorizontalStretch(1);

    facesArea->setSizePolicy(spImage);
    preprocessedFacesArea->setSizePolicy(spImage);
    alignedFacesArea->setSizePolicy(spImage);

    spImage.setHorizontalPolicy(QSizePolicy::Preferred);
    spImage.setHorizontalStretch(2);
    controlPanel->setSizePolicy(spImage);

    QHBoxLayout* processingLayout = new QHBoxLayout(imageArea);
    processingLayout->addWidget(m_fullImage);
    processingLayout->addWidget(facesArea);
    processingLayout->addWidget(preprocessedFacesArea);
    processingLayout->addWidget(alignedFacesArea);
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
    delete m_detector;
    delete m_recognizer;
    delete m_extractor;

    delete m_fullImage;
    delete m_imageListView;
    delete m_croppedfaceLayout;
    delete m_preprocessedLayout;
    delete m_alignedLayout;

    delete m_recognizationInfo;
    delete m_imageLabel;
    delete m_similarityLabel;
}

void MainWindow::slotDetectFaces(const QListWidgetItem* imageItem)
{
    QString imagePath = imageItem->text();
    qDebug() << "Loading " << imagePath;
    QImage img(imagePath);
    QImage imgScaled(img.scaled(416, 416, Qt::KeepAspectRatio));

    // clear faces layout
    QLayoutItem* wItem;
    while ((wItem = m_croppedfaceLayout->takeAt(0)) != nullptr)
    {
        delete wItem->widget();
        delete wItem;
    }

    while ((wItem = m_preprocessedLayout->takeAt(0)) != nullptr)
    {
        delete wItem->widget();
        delete wItem;
    }

    while ((wItem = m_alignedLayout->takeAt(0)) != nullptr)
    {
        delete wItem->widget();
        delete wItem;
    }

    QList<QRectF> faces = detectFaces(imagePath);

    extractFaces(img, imgScaled, faces);

    // Only setPixmap after finishing drawing bboxes around detected faces
    m_fullImage->setPixmap(QPixmap::fromImage(imgScaled));
}


QLabel* MainWindow::showCVMat(const cv::Mat& cvimage)
{
    QLabel* image = nullptr;
    if(cvimage.cols*cvimage.rows != 0)
    {
        cv::Mat rgb;
        QPixmap p;
        cv::cvtColor(cvimage, rgb, (-2*cvimage.channels()+10));
        p.convertFromImage(QImage(rgb.data, rgb.cols, rgb.rows, QImage::Format_RGB888));

        image = new QLabel;
        image->setPixmap(p);
        //resize(cvimage.cols, cvimage.rows);
    }

    return image;
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
        QLabel* const croppedFace = new QLabel;
        croppedFace->setScaledContents(false);

        QRect rectDraw      = FaceDetector::toAbsoluteRect(rr, imgScaled.size());
        QRect rect          = FaceDetector::toAbsoluteRect(rr, img.size());
        QImage part         = img.copy(rect);

        // Show cropped faces
        croppedFace->setPixmap(QPixmap::fromImage(part.scaled(qMin(img.size().width(), 100),
                                                              qMin(img.size().width(), 100),
                                                              Qt::KeepAspectRatio)));
        m_croppedfaceLayout->addWidget(croppedFace);
        painter.drawRect(rectDraw);

        // Show preprocessed faces
        cv::Mat cvPreprocessedFace = m_recognizer->prepareForRecognition(part);

        m_preprocessedLayout->addWidget(showCVMat(cvPreprocessedFace));

        // Show aligned faces
        cv::Mat cvAlignedFace = m_extractor->alignFace(cvPreprocessedFace);

        m_alignedLayout->addWidget(showCVMat(cvAlignedFace));

        // get face embedding
        //std::vector<float> faceEmbedding = m_extractor->getFaceEmbedding(cvPreprocessedFace);

        //qDebug() << "face embedding of size" << faceEmbedding.size() << ":" << faceEmbedding;
    }
}

QWidget* MainWindow::setupControlPanel()
{
    QWidget* controlPanel = new QWidget();

    m_recognizationInfo = new QLabel(this);
    m_imageLabel        = new QLineEdit(this);
    m_similarityLabel   = new QLabel(this);

    QFormLayout* layout   = new QFormLayout(this);
    layout->addRow(new QLabel(QLatin1String("Reconized :")), m_recognizationInfo);
    layout->addRow(new QLabel(QLatin1String("Identity :")),  m_imageLabel);
    layout->addRow(new QLabel(QLatin1String("Similarity distance :")), m_similarityLabel);

    controlPanel->setLayout(layout);

    return controlPanel;
}


QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("dataset"), QLatin1String("Data set folder"), QLatin1String("path relative to data folder")));
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

   if (! parser->isSet(QLatin1String("dataset")))
   {
       qWarning("Data set is not set !!!");

       return 1;
   }

   QDir dataset(parser->value(QLatin1String("dataset")));

   MainWindow* window = new MainWindow(dataset, nullptr);

   window->show();

   return app.exec();
}

#include "benchmark_dnnrecognition.moc"

