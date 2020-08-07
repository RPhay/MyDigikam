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
#include <QJsonObject>
#include <QJsonDocument>

// lib digikam includes
#include "opencvdnnfacedetector.h"
#include "facedetector.h"
#include "dnnfaceextractor.h"
#include "opencvdnnfacerecognizer.h"
#include "facialrecognition_wrapper.h"
#include "dbengineparameters.h"
#include "coredbaccess.h"

using namespace Digikam;
/**
 * @brief The Benchmark class: a benchmark especially made for testing the performance of face recognition module
 * on faces images. The dataset should contain only face data. It's not built for other purposes.
 */
class Benchmark : public QObject
{
    Q_OBJECT
public:

    Benchmark();
    ~Benchmark();

public:

    void verifyTestSet();

    void splitData(const QDir& dataDir, float splitRatio);

public:

    QCommandLineParser* m_parser;
    float m_error;
    int   m_trainSize;
    int   m_testSize;

private:

    QImage detect(QImage* faceImg);

public:
    Q_SLOT void fetchData();
    Q_SLOT void registerTrainingSet();
    Q_SLOT void saveData();

    Q_SLOT void testWriteDb();
    Q_SLOT void verifyKNearestDb();

private:

    QHash<QString, QVector<QImage*> > m_trainSet;
    QHash<QString, QVector<QImage*> > m_testSet;

    OpenCVDNNFaceDetector* m_detector;
    FacialRecognitionWrapper*  m_recognizer;
};

Benchmark::Benchmark()
    : QObject(),
      m_parser(nullptr),
      m_error(-1),
      m_trainSize(0),
      m_testSize(0)
{
    DbEngineParameters prm = DbEngineParameters::parametersFromConfig();
    CoreDbAccess::setParameters(prm, CoreDbAccess::MainApplication);

    m_detector   = new OpenCVDNNFaceDetector(DetectorNNModel::SSDMOBILENET);
    m_recognizer = new FacialRecognitionWrapper();

    m_recognizer->clearAllTraining();
    m_recognizer->deleteIdentities(m_recognizer->allIdentities());
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
        QMap<QString, QString> attributes;
        attributes[QLatin1String("fullName")] = iter.key();

        Identity newIdentity = m_recognizer->addIdentity(attributes);

        qDebug() << "add new identity to database" << newIdentity.id();

        QList<QImage> trainImages;

        for (int i = 0; i < iter.value().size(); ++i)
        {
            QImage croppedFace = detect(iter.value().at(i));

            if (!croppedFace.isNull())
            {
                trainImages << croppedFace;

                ++m_trainSize;
            }
        }

        m_recognizer->train(newIdentity, trainImages, QLatin1String("train face classifier"));
    }

    unsigned int elapsedDetection = timer.elapsed();
    qDebug() << "Registered <<  :" << m_trainSize << "faces in training set, with average" << float(elapsedDetection)/m_trainSize << "ms/face";
}

void Benchmark::verifyTestSet()
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
        QList<QImage> croppedFaces;

        for (int i = 0; i < iter.value().size(); ++i)
        {
            QImage croppedFace = detect(iter.value().at(i));

            if (!croppedFace.isNull())
            {
                Identity prediction = m_recognizer->recognizeFace(croppedFace);
                if (prediction.isNull() && m_trainSet.contains(iter.key()))
                {
                    // cannot recognize when label is already register
                    ++nbNotRecognize;
                }
                else if (prediction.attribute(QLatin1String("fullName")) != iter.key())
                {
                    // wrong label
                    ++nbWrongLabel;
                }

                ++m_testSize;
            }
        }
/*
        QList<Identity> predictions = m_recognizer->recognizeFaces(croppedFaces);

        for (int i = 0; i < predictions.size(); ++i)
        {
            if (predictions[i].isNull() && m_trainSet.contains(iter.key()))
            {
                // cannot recognize when label is already register
                ++nbNotRecognize;
            }
            else if (predictions[i].attribute(QLatin1String("fullName")) != iter.key())
            {
                // wrong label
                ++nbWrongLabel;
            }
        }
*/
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

    qDebug() << "Accuracy :" << (1 - m_error) * 100 << "%"
             << "on total" << m_trainSize << "training faces, and"
                           << m_testSize << "test faces, (" << float(elapsedDetection)/m_testSize << " ms/face)";
}

QImage Benchmark::detect(QImage* faceImg)
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
        return QImage();
    }

    QRect rect = FaceDetector::toAbsoluteRect(faces[0], faceImg->size());

    return faceImg->copy(rect);
}

void Benchmark::splitData(const QDir& dataDir, float splitRatio)
{
    int nbData = 0;

    qsrand(QTime::currentTime().msec());

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
                    ++nbData;
                }
            }
            else
            {
                if (! img->isNull())
                {
                    m_testSet[label].append(img);
                    ++nbData;
                }
            }
        }
    }

    unsigned int elapsedDetection = timer.elapsed();

    qDebug() << "Fetched dataset with" << nbData << "samples, with average" << float(elapsedDetection)/nbData << "ms/image.";;
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

void Benchmark::saveData()
{
    QDir dataDir(m_parser->value(QLatin1String("dataset")));
    QFileInfoList subDirs = dataDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Name);

    QElapsedTimer timer;
    timer.start();

    QJsonArray faceEmbeddingArray;

    for (int i = 0; i < subDirs.size(); ++i)
    {
        QDir subDir(subDirs[i].absoluteFilePath());
        QFileInfoList filesInfo = subDir.entryInfoList(QDir::Files | QDir::Readable);

        for (int j = 0; j < filesInfo.size(); ++j)
        {
            QImage* img = new QImage(filesInfo[j].absoluteFilePath());

            if (! img->isNull())
            {
                cv::Mat face;
            /*
                if (preprocess(img, face))
                {

                    Identity newIdentity = m_recognizer->newIdentity(face);

                    QJsonObject identityJson;
                    identityJson[QLatin1String("id")] = i;
                    identityJson[QLatin1String("faceembedding")] = QJsonDocument::fromJson(newIdentity.attribute(QLatin1String("faceEmbedding")).toLatin1()).array();

                    faceEmbeddingArray.append(identityJson);
                }
            */
            }
        }
    }

    qDebug() << "Save face embedding in" << timer.elapsed() << "ms/face";

    QFile dataFile(dataDir.dirName() + QLatin1String(".json"));

    if (!dataFile.open(QIODevice::WriteOnly))
    {
        qWarning("Couldn't open save file.");
        return;
    }

    QJsonDocument saveDoc(faceEmbeddingArray);
    dataFile.write(saveDoc.toJson());

    dataFile.close();
}

void Benchmark::testWriteDb()
{
    QDir dataDir(m_parser->value(QLatin1String("dataset")));

    QFile dataFile(dataDir.dirName() + QLatin1String(".json"));

    if (!dataFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open data file.");
        return;
    }

    QByteArray saveData = dataFile.readAll();
    dataFile.close();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    QJsonArray data = loadDoc.array();

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < data.size(); ++i)
    {
        QJsonObject object = data[i].toObject();

        std::vector<float> faceEmbedding = DNNFaceExtractor::decodeVector(object[QLatin1String("faceembedding")].toArray());

        //m_recognizer->insertData(DNNFaceExtractor::vectortomat(faceEmbedding), object[QLatin1String("id")].toInt());
    }

    qDebug() << "write face embedding to spatial database with average" << timer.elapsed() /data.size() << "ms/faceEmbedding";
}

void Benchmark::verifyKNearestDb()
{
    QDir dataDir(m_parser->value(QLatin1String("dataset")));

    QFile dataFile(dataDir.dirName() + QLatin1String(".json"));

    if (!dataFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open data file.");
        return;
    }

    QByteArray saveData = dataFile.readAll();
    dataFile.close();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    QJsonArray data = loadDoc.array();

    int nbCorrect = 0;

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < data.size(); ++i)
    {
        QJsonObject object = data[i].toObject();

        std::vector<float> faceEmbedding = DNNFaceExtractor::decodeVector(object[QLatin1String("faceembedding")].toArray());

        int label = object[QLatin1String("id")].toInt();

        QMap<double, QVector<int> > closestNeighbors /*= m_recognizer->getClosestNodes(DNNFaceExtractor::vectortomat(faceEmbedding), 1.0, 5)*/;

        QMap<int, QVector<double> > votingGroups;

        for (QMap<double, QVector<int> >::const_iterator iter  = closestNeighbors.cbegin();
                                                         iter != closestNeighbors.cend();
                                                       ++iter)
        {
            for (int j = 0; j < iter.value().size(); ++j)
            {
                votingGroups[iter.value()[j]].append(iter.key());
            }
        }

        double maxScore = 0;
        int prediction;

        for (QMap<int, QVector<double> >::const_iterator group  = votingGroups.cbegin();
                                                         group != votingGroups.cend();
                                                       ++group)
        {
            double score = 0;

            for (int i = 0; i < group.value().size(); ++i)
            {
                score += (1 - group.value()[i]);
            }

            if (score > maxScore)
            {
                maxScore   = score;
                prediction = group.key();
            }
        }

        if (label == prediction)
        {
            ++nbCorrect;
        }
    }

    qDebug() << "Accuracy" << (float(nbCorrect)/data.size())*100 << "with average" << timer.elapsed() / data.size() << "ms/faceEmbedding";
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

    app.setApplicationName(QString::fromLatin1("digikam"));          // for DB init.

    Benchmark benchmark;
    benchmark.m_parser = parseOptions(app);

    //benchmark.saveData();
    //benchmark.testWriteDb();
    //benchmark.verifyKNearestDb();

    benchmark.fetchData();
    benchmark.registerTrainingSet();
    benchmark.verifyTestSet();

    return 0;
}


#include "benchmark_recognition.moc"
