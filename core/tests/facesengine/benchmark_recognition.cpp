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
#include <QListWidgetItem>
#include <QDir>
#include <QImage>
#include <QElapsedTimer>
#include <QDebug>
#include <QHash>
#include <QTest>

// lib digikam includes
#include "opencvdnnfacedetector.h"
#include "facedetector.h"
#include "faceextractor.h"
#include "facerecognizer.h"
#include "identity.h"

using namespace Digikam;
using namespace RecognitionTest;

// TODO: Recognition is incorrect where human are wearing glasses
// TODO: Verify cosnine distance Vs L2 distance



/**
 * @brief The Benchmark class: a benchmark especially made for testing the performance of face recognition module
 * on faces images. The dataset should contain only face data (un-cropped or cropped). It's not built for other purposes.
 *
 * If the faces are already cropped, the photo will be passed directly the FaceRecognizer for face recognition.
 * If the faces are un-cropped in original image, it will pass through FaceDetector before it landing on Facerecognizer
 *
 * These 2 types of images can be easily detected by FaceDetector because FaceDetector cannot detected already cropped face.
 */
class Benchmark : public QObject
{
    Q_OBJECT
public:

    Benchmark();
    ~Benchmark();

public:

    void verifyTestSet(FaceRecognizer::ComparisonMetric metric, double threshold);

    QVector<QListWidgetItem*> splitData(const QDir& dataDir, float splitRatio);

public:

    QCommandLineParser* m_parser;
    float m_error;
    int   m_trainSize;
    int   m_testSize;

private:

    cv::Mat preprocess(QImage* faceImg);

public:
    Q_SLOT void fetchData();
    Q_SLOT void registerTrainingSet();
    Q_SLOT void verifyTestSetCosDistance();
    Q_SLOT void verifyTestSetL2Distance();
    Q_SLOT void verifyTestSetL2NormDistance();
    Q_SLOT void verifyTestSetSupportVectorMachine();


private:

    QHash<QString, QVector<QImage*> > m_trainSet;
    QHash<QString, QVector<QImage*> > m_testSet;

    OpenCVDNNFaceDetector* m_detector;
    FaceRecognizer*        m_recognizer;
};

Benchmark::Benchmark()
    : QObject(),
      m_parser(nullptr),
      m_error(-1),
      m_trainSize(0),
      m_testSize(0)
{
    m_detector   = new OpenCVDNNFaceDetector(DetectorNNModel::SSDMOBILENET);
    m_recognizer = new FaceRecognizer(true);
}

Benchmark::~Benchmark()
{
    for (QHash<QString, QVector<QImage*> >::iterator vector  = m_trainSet.begin();
                                                     vector != m_trainSet.end();
                                                   ++vector)
    {

        QVector<QImage*>::iterator img = vector.value().begin();

        while (img != vector.value().end())
        {
            delete *img;
            img = vector.value().erase(img);
        }
    }

    for (QHash<QString, QVector<QImage*> >::iterator vector  = m_testSet.begin();
                                                     vector != m_testSet.end();
                                                   ++vector)
    {
        QVector<QImage*>::iterator img = vector.value().begin();

        while (img != vector.value().end())
        {
            delete *img;
            img = vector.value().erase(img);
        }
    }

    delete m_detector;
    delete m_recognizer;
    delete m_parser;
}

void Benchmark::registerTrainingSet()
{
    m_trainSize = 0;

    QElapsedTimer timer;
    timer.start();

    for (QHash<QString, QVector<QImage*> >::iterator iter  = m_trainSet.begin();
                                                     iter != m_trainSet.end();
                                                   ++iter)
    {
        for (int i = 0; i < iter.value().size(); ++i)
        {
            Identity newIdentity = m_recognizer->newIdentity(preprocess(iter.value().at(i)));

            newIdentity.setAttribute(QLatin1String("fullName"), iter.key());

            m_recognizer->saveIdentity(newIdentity);

            ++m_trainSize;
        }
    }

    unsigned int elapsedDetection = timer.elapsed();
    qDebug() << "Registered <<  :" << m_trainSize << "faces in training set, with average" << float(elapsedDetection)/m_trainSize << "ms/face";
}

void Benchmark::verifyTestSet(FaceRecognizer::ComparisonMetric metric, double threshold)
{
    int nbNotRecognize = 0;
    int nbWrongLabel   = 0;
    m_testSize = 0;

    QElapsedTimer timer;
    timer.start();

    for (QHash<QString, QVector<QImage*> >::iterator iter  = m_testSet.begin();
                                                     iter != m_testSet.end();
                                                   ++iter)
    {
        for (int i = 0; i < iter.value().size(); ++i)
        {
            Identity newIdentity = m_recognizer->findIdenity(preprocess(iter.value().at(i)), metric, threshold);

            if (newIdentity.isNull() && m_trainSet.contains(iter.key()))
            {
                // cannot recognize when label is already register
                ++nbNotRecognize;
            }
            else if (newIdentity.attribute(QLatin1String("fullName")) != iter.key())
            {
                // wrong label
                ++nbWrongLabel;
            }

            ++m_testSize;
        }
    }

    unsigned int elapsedDetection = timer.elapsed();

    if (m_testSize == 0)
    {
        qWarning() << "test set is empty";

        return;
    }

    m_error = float(nbNotRecognize + nbWrongLabel)/m_testSize;

    qDebug() << "nb Not Recognized :" << nbNotRecognize;
    qDebug() << "nb Wrong Label :" << nbWrongLabel;

    qDebug() << "Recognition error :" << m_error
             << "on total" << m_trainSize << "training faces, and"
                           << m_testSize << "test faces, (" << float(elapsedDetection)/m_testSize << " ms/face)";
}

cv::Mat Benchmark::preprocess(QImage* faceImg)
{
    QList<QRectF> faces;

    try
    {
        // NOTE detection with filePath won't work when format is not standard
        // NOTE unexpected behaviour with detecFaces(const QString&)
        cv::Size paddedSize(0, 0);
        cv::Mat cvImage       = m_detector->prepareForDetection(*faceImg, paddedSize);
        QList<QRect> absRects = m_detector->detectFaces(cvImage, paddedSize);
        faces                 = FaceDetector::toRelativeRects(absRects,
                                                              QSize(cvImage.cols - 2*paddedSize.width,
                                                              cvImage.rows - 2*paddedSize.height));
    }
    catch (cv::Exception& e)
    {
        qWarning() << "cv::Exception:" << e.what();
    }
    catch(...)
    {
        qWarning() << "Default exception from OpenCV";
    }

    if (faces.isEmpty())
    {
        return (m_recognizer->prepareForRecognition(*faceImg));
    }

    QRect rect = FaceDetector::toAbsoluteRect(faces[0], faceImg->size());

    return (m_recognizer->prepareForRecognition(faceImg->copy(rect)));
}

QVector<QListWidgetItem*> Benchmark::splitData(const QDir& dataDir, float splitRatio)
{
    int nbData = 0;

    qsrand(QTime::currentTime().msec());

    QVector<QListWidgetItem*> imageItems;

    // Each subdirectory in data directory should match with a label
    QFileInfoList subDirs = dataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < subDirs.size(); ++i)
    {
        QDir subDir(subDirs[i].absoluteFilePath());

        QString label = subDirs[i].fileName();

        QFileInfoList filesInfo = subDir.entryInfoList(QDir::Files | QDir::Readable);

        // suffle dataset
        QList<QFileInfo>::iterator it = filesInfo.begin();
        QList<QFileInfo>::iterator it1;

        for (int i = 0; i < filesInfo.size(); ++i)
        {
            int inc = (int) (float(filesInfo.size()) * qrand() / (RAND_MAX + 1.0));

            it1     = filesInfo.begin();
            it1    += inc;

            std::swap(*(it++), *(it1));
         }

        // split train/test
        for (int i = 0; i < filesInfo.size(); ++i)
        {
            QImage* img = new QImage(filesInfo[i].absoluteFilePath());

            if (i < filesInfo.size() * splitRatio)
            {
                if (! img->isNull())
                {
                    m_trainSet[label].append(img);
                    imageItems.append(new QListWidgetItem(QIcon(filesInfo[i].absoluteFilePath()), filesInfo[i].absoluteFilePath()));
                    ++nbData;
                }
            }
            else
            {
                if (! img->isNull())
                {
                    m_testSet[label].append(img);
                    imageItems.append(new QListWidgetItem(QIcon(filesInfo[i].absoluteFilePath()), filesInfo[i].absoluteFilePath()));
                    ++nbData;
                }
            }
        }
    }

    unsigned int elapsedDetection = timer.elapsed();

    qDebug() << "Fetched dataset with" << nbData << "samples, with average" << float(elapsedDetection)/nbData << "ms/image.";;

    return imageItems;
}

void Benchmark::fetchData()
{
    if (! m_parser->isSet(QLatin1String("dataset")))
    {
        qWarning("Data set is not set !!!");

        return;
    }

    QDir dataset(m_parser->value(QLatin1String("dataset")));

    float splitRatio = 0.8;
    if (m_parser->isSet(QLatin1String("split")))
    {
        splitRatio = m_parser->value(QLatin1String("split")).toFloat();
    }

    splitData(dataset, splitRatio);
}

void Benchmark::verifyTestSetCosDistance()
{
    verifyTestSet(FaceRecognizer::CosDistance, 0.7);
}

void Benchmark::verifyTestSetL2Distance()
{
    verifyTestSet(FaceRecognizer::L2Distance, 0.7);
}

void Benchmark::verifyTestSetL2NormDistance()
{
    verifyTestSet(FaceRecognizer::L2NormDistance, 0.7);
}

void Benchmark::verifyTestSetSupportVectorMachine()
{
    verifyTestSet(FaceRecognizer::SupportVectorMachine, 0.7);
}

QCommandLineParser* parseOptions(const QCoreApplication& app)
{
    QCommandLineParser* parser = new QCommandLineParser();
    parser->addOption(QCommandLineOption(QLatin1String("dataset"), QLatin1String("Data set folder"), QLatin1String("path relative to data folder")));
    parser->addOption(QCommandLineOption(QLatin1String("split"), QLatin1String("Split ratio"), QLatin1String("split ratio of training data")));
    parser->addHelpOption();
    parser->process(app);

    return parser;
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    Benchmark benchmark;
    benchmark.m_parser = parseOptions(app);

    //QTest::qExec(&benchmark);

    benchmark.fetchData();
    benchmark.registerTrainingSet();
    //benchmark.verifyTestSetCosDistance();
    //benchmark.verifyTestSetL2Distance();
    //benchmark.verifyTestSetL2NormDistance();

    benchmark.verifyTestSetSupportVectorMachine();
}


#include "benchmark_recognition.moc"
