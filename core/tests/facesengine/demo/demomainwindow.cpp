/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-06-21
 * Description : GUI test program for FacesEngine
 *
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C)      2010 by Alex Jironkin <alexjironkin at gmail dot com>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
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

#include "demomainwindow.h"
#include "ui_demomainwindow.h"

// Qt includes

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QElapsedTimer>
#include <QStandardPaths>

// Local includes

#include "digikam_debug.h"
#include "facialrecognition_wrapper.h"
#include "facedetector.h"
#include "demofaceitem.h"
#include "dfiledialog.h"

using namespace std;
using namespace Digikam;

namespace FaceEngineDemo
{

// --------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN MainWindow::Private
{
public:

    explicit Private()
      : ui            (nullptr),
        myScene       (nullptr),
        myView        (nullptr),
        lastPhotoItem (nullptr),
        detector      (nullptr),
        scale         (0.0)
    {
    }

    Ui::MainWindow*          ui;
    QGraphicsScene*          myScene;
    QGraphicsView*           myView;
    QGraphicsPixmapItem*     lastPhotoItem;
    QList<FaceItem*>         faceitems;

    FacialRecognitionWrapper database;
    FaceDetector*            detector;
    QImage                   currentPhoto;
    double                   scale;
    QString                  lastFileOpenPath;
};

MainWindow::MainWindow(QWidget* const parent)
    : QMainWindow(parent),
      d(new Private)
{
    d->ui = new Ui::MainWindow;
    d->ui->setupUi(this);
    d->ui->recogniseBtn->setEnabled(false);
    d->ui->updateDatabaseBtn->setEnabled(false);
    d->ui->detectFacesBtn->setEnabled(false);
    d->ui->configLocation->setReadOnly(true);

    connect(d->ui->openImageBtn, SIGNAL(clicked()),
            this, SLOT(slotOpenImage()));

    connect(d->ui->accuracySlider, SIGNAL(valueChanged(int)),
            this, SLOT(slotUpdateAccuracy()));

    connect(d->ui->sensitivitySlider, SIGNAL(valueChanged(int)),
            this, SLOT(slotUpdateSensitivity()));

    connect(d->ui->detectFacesBtn, SIGNAL(clicked()),
            this, SLOT(slotDetectFaces()));

    connect(d->ui->recogniseBtn, SIGNAL(clicked()),
            this, SLOT(slotRecognise()));

    connect(d->ui->updateDatabaseBtn, SIGNAL(clicked()),
            this, SLOT(slotUpdateDatabase()));

    d->myScene                = new QGraphicsScene();
    QGridLayout* const layout = new QGridLayout;
    d->myView                 = new QGraphicsView(d->myScene);

    d->myView->setCacheMode(QGraphicsView::CacheBackground);
    d->myScene->setItemIndexMethod(QGraphicsScene::NoIndex);

    setMouseTracking(true);
    layout->addWidget(d->myView);

    d->ui->widget->setLayout(layout);

    d->myView->show();

    d->detector = new FaceDetector();

    d->ui->accuracySlider->setValue(80);
    d->ui->sensitivitySlider->setValue(80);

    d->lastFileOpenPath = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();
}

MainWindow::~MainWindow()
{
    delete d->ui;
    delete d->detector;
    delete d;
}

void MainWindow::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);

    switch (e->type())
    {
        case QEvent::LanguageChange:
        {
            d->ui->retranslateUi(this);
            break;
        }

        default:
        {
            break;
        }
    }
}

void MainWindow::clearScene()
{
    QList<QGraphicsItem*> list = d->myScene->items();

    for (int i = 0 ; i < list.size() ; ++i)
    {
        d->myScene->removeItem(list.at(i));
    }
}

void MainWindow::slotOpenImage()
{
    QString file = DFileDialog::getOpenFileName(this, QLatin1String("Select Image to Open"),
                                                d->lastFileOpenPath,
                                                QString::fromLatin1("Image Files (*.png *.jpg *.bmp *.pgm)"));

    if (file.isEmpty())
    {
        return;
    }

    d->lastFileOpenPath = QFileInfo(file).absolutePath();

    clearScene();

    qCDebug(DIGIKAM_TESTS_LOG) << "Opened file " << file;

    d->currentPhoto.load(file);
    d->lastPhotoItem = new QGraphicsPixmapItem(QPixmap::fromImage(d->currentPhoto));

    if ((1.0 * d->ui->widget->width() / d->currentPhoto.width()) < (1.0 * d->ui->widget->height() / d->currentPhoto.height()))
    {
        d->scale = 1.0 * d->ui->widget->width() / d->currentPhoto.width();
    }
    else
    {
        d->scale = 1.0 * d->ui->widget->height() / d->currentPhoto.height();
    }

    d->lastPhotoItem->setScale(d->scale);

    d->myScene->addItem(d->lastPhotoItem);
    d->ui->detectFacesBtn->setEnabled(true);
}

void MainWindow::slotDetectFaces()
{
    setCursor(Qt::WaitCursor);

    QList<QRectF> currentFaces = d->detector->detectFaces(d->currentPhoto);

    qCDebug(DIGIKAM_TESTS_LOG) << "FacesEngine detected : " << currentFaces.size() << " faces.";
    qCDebug(DIGIKAM_TESTS_LOG) << "Coordinates of detected faces : ";

    foreach (const QRectF& r, currentFaces)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << r;
    }

    foreach (FaceItem* const item, d->faceitems)
    {
        item->setVisible(false);
    }

    d->faceitems.clear();

    for (int i = 0 ; i < currentFaces.size() ; ++i)
    {
        QRect face = d->detector->toAbsoluteRect(currentFaces[i], d->currentPhoto.size());
        d->faceitems.append(new FaceItem(nullptr, d->myScene, face, d->scale));
        qCDebug(DIGIKAM_TESTS_LOG) << face;
    }

    d->ui->recogniseBtn->setEnabled(true);
    d->ui->updateDatabaseBtn->setEnabled(true);

    unsetCursor();
}

void MainWindow::slotUpdateAccuracy()
{
    int value = d->ui->accuracySlider->value();
    d->detector->setParameter(QString::fromLatin1("accuracy"), value/100.0);
}

void MainWindow::slotUpdateSensitivity()
{
    int value = d->ui->sensitivitySlider->value();
    d->detector->setParameter(QString::fromLatin1("sensitivity"), value);
}

void MainWindow::slotRecognise()
{
    setCursor(Qt::WaitCursor);

    int i = 0;

    foreach (FaceItem* const item, d->faceitems)
    {
        QElapsedTimer timer;
        timer.start();

        QImage* face      = new QImage();
        *face             = d->currentPhoto.copy(item->originalRect());

        Identity identity = d->database.recognizeFace(face);
        int elapsed       = timer.elapsed();

        qCDebug(DIGIKAM_TESTS_LOG) << "Recognition took " << elapsed << " for Face #" << i+1;

        if (!identity.isNull())
        {
            item->suggest(identity.attribute(QString::fromLatin1("name")));

            qCDebug(DIGIKAM_TESTS_LOG) << "Face #" << i+1 << " is closest to the person with ID " << identity.id()
                     << " and name "<< identity.attribute(QString::fromLatin1("name"));
        }
        else
        {
            qCDebug(DIGIKAM_TESTS_LOG) << "Face #" << i+1 << " : no Identity match from database.";
        }

        i++;
    }

    unsetCursor();
}

void MainWindow::slotUpdateDatabase()
{
    setCursor(Qt::WaitCursor);

    int i = 0;

    foreach (FaceItem* const item, d->faceitems)
    {
        if (item->text() != QString::fromLatin1("?"))
        {
            QElapsedTimer timer;
            timer.start();

            QString name = item->text();
            qCDebug(DIGIKAM_TESTS_LOG) << "Face #" << i+1 << ": training name '" << name << "'";

            Identity identity = d->database.findIdentity(QString::fromLatin1("name"), name);

            if (identity.isNull())
            {
                QMap<QString, QString> attributes;
                attributes[QString::fromLatin1("name")] = name;
                identity                                = d->database.addIdentity(attributes);
                qCDebug(DIGIKAM_TESTS_LOG) << "Adding new identity ID " << identity.id() << " to database for name " << name;
            }
            else
            {
                qCDebug(DIGIKAM_TESTS_LOG) << "Found existing identity ID " << identity.id() << " from database for name " << name;
            }

            QImage* face = new QImage();
            *face        = d->currentPhoto.copy(item->originalRect());

            d->database.train(identity, face, QString::fromLatin1("test application"));

            int elapsed  = timer.elapsed();

            qCDebug(DIGIKAM_TESTS_LOG) << "Training took " << elapsed << " for Face #" << i+1;
        }

        ++i;
    }

    unsetCursor();
}

} // namespace FaceEngineDemo
