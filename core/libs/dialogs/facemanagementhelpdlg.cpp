/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : Help Box Dialog for Face Management
 *
 * Copyright (C) 2020 by Kartik Ramesh <kartikx2000 at gmail dot com>
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

#include "facemanagementhelpdlg.h"

#include <QLabel>
#include <QApplication>
#include <QIcon>
#include <QVBoxLayout>
#include <QUrl>
#include <QString>
#include <QTabWidget>

#include <klocalizedstring.h>

#include "daboutdata.h"
namespace Digikam
{
FaceManagementHelpDialog::FaceManagementHelpDialog(QWidget* const parent)
    : QDialog(parent)
{
    setModal(false);
    setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    setWindowTitle(i18n("Face Management in DigiKam"));

    QGridLayout* mainLayout = new QGridLayout();
    QTabWidget*  tabWidget  = new QTabWidget(this);

    // --- Information Page -------------------------------------------------------------

    QWidget* infoPage       = new QWidget();

    QLabel* const logo      = new QLabel(infoPage);
    logo->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(100,100)));

    QLabel* const header    = new QLabel(infoPage);
    header->setWordWrap(true);
    header->setText(i18n("<font size=\"5\">%1</font><br/><b>Version %2</b>"
                         "<p>%3</p><p><i>%4</i></p>",
                         QApplication::applicationName(),
                         QApplication::applicationVersion(),
                         DAboutData::digiKamSlogan(),
                         DAboutData::digiKamFamily()));

    QLabel* introText       = new QLabel(infoPage);
    introText->setWordWrap(true);
    introText->setText(i18n("<hr> The Face Workflow allows you to Detect and Recognize people "
                            "in your photographs."));

    QLabel* const faceDetectionHeader   = new QLabel(infoPage);
    faceDetectionHeader->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    faceDetectionHeader->setWordWrap(false);
    faceDetectionHeader->setText(i18n("<b>Face Detection</b>"));

    QLabel* const faceDetection         = new QLabel(infoPage);
    faceDetection->setWordWrap(true);
    faceDetection->setText(i18n("Face Detection allows DigiKam to detect faces in your images. "
                                "This step is required before being able to recognize faces."));

    QLabel* const faceRecognitionHeader = new QLabel(infoPage);
    faceRecognitionHeader->setWordWrap(false);
    faceRecognitionHeader->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    faceRecognitionHeader->setText(i18n("<b>Face Recognition</b>"));

    QLabel* const faceRecognition       = new QLabel(infoPage);
    faceRecognition->setWordWrap(true);
    faceRecognition->setText(i18n("Face Recognition will automatically recognize people in your images. "));

    // Link to DigiKam documentation
    QString documentationLink = i18n(
            "https://docs.kde.org/trunk5/en/extragear-graphics/digikam/using-digikam.html#using-mainwindow-peopleview");

    QLabel* const footerText = new QLabel(infoPage);
    footerText->setOpenExternalLinks(true);
    footerText->setAlignment(Qt::AlignHCenter);
    footerText->setWordWrap(false);
    footerText->setText(i18n("<hr>For additional information, read the %1 \n"
                             "<br>To ask questions, use our mailing list: digikam-users@kde.org \n",
                             QString::fromLatin1("<a href='%1'>digiKam Online Handbook</a>").arg(documentationLink)));

    QGridLayout* infoPageLayout = new QGridLayout();
    infoPageLayout->addWidget(logo,                     0, 0, 1, 1);
    infoPageLayout->addWidget(header,                   0, 1, 1, 1);
    infoPageLayout->addWidget(introText,                1, 0, 1, 2);
    infoPageLayout->addWidget(faceDetectionHeader,      2, 0, 1, 1);
    infoPageLayout->addWidget(faceDetection,            2, 1, 1, 1);
    infoPageLayout->addWidget(faceRecognitionHeader,    3, 0, 1, 1);
    infoPageLayout->addWidget(faceRecognition,          3, 1, 1, 1);
    infoPageLayout->addWidget(footerText,               4, 0, 1, 2);
    infoPageLayout->setRowStretch(1, 1);
    infoPageLayout->setRowStretch(2, 1);
    infoPageLayout->setRowStretch(3, 1);

    infoPage->setLayout(infoPageLayout);

    tabWidget->addTab(infoPage, i18n("Info"));

    // --- Face Detection Page ----------------------------------------------------------

    QWidget*     faceDetectionPage       = new QWidget();
    QGridLayout* faceDetectionPageLayout = new QGridLayout();

    QPixmap detectFaceOptionImage  = QPixmap(i18n("/home/kartik/detectFaces2.png"));
    detectFaceOptionImage = detectFaceOptionImage.scaled(QSize(350,170));

    QLabel* detectFaceOptionLabel  = new QLabel(faceDetectionPage);
    detectFaceOptionLabel->setPixmap(detectFaceOptionImage);

    QLabel* detectFaceInfoLabel    = new QLabel(faceDetectionPage);
    detectFaceInfoLabel->setWordWrap(true);
    detectFaceInfoLabel->setText(i18n("To run Face Detection, select the "
                                      "<u>Detect Faces</u> option in the Face Scan panel "
                                      "and then click on <u>Scan Collection for Faces</u> <hr>"
                                      "<b>Caution: </b> Running Face Detection for the first time "
                                      "can be fairly time consuming. Make sure you use the "
                                      "<u>Search In</u> tab to restrict the target of the Scan."));

    QLabel* detectIntroLabel       = new QLabel(faceDetectionPage);
    detectIntroLabel->setWordWrap(true);
    detectIntroLabel->setText(i18n("DigiKam offers two kinds of Face Detection Scans: "));

    QLabel* skipScannedHeaderLabel = new QLabel(faceDetectionPage);
    skipScannedHeaderLabel->setWordWrap(true);
    skipScannedHeaderLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    skipScannedHeaderLabel->setText(i18n("<b>Skip Images Already Scanned"));

    QLabel* skipScannedLabel       = new QLabel(faceDetectionPage);
    skipScannedLabel->setWordWrap(true);
    skipScannedLabel->setText(i18n("Choose this option if you haven't scanned yet or "
                                   "if the last scan yielded good results but you would "
                                   "like to scan new images."));

    QLabel* scanAgainHeaderLabel   = new QLabel(faceDetectionPage);
    scanAgainHeaderLabel->setWordWrap(true);
    scanAgainHeaderLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scanAgainHeaderLabel->setText(i18n("<b>Scan Again and Merge"));

    QLabel* scanAgainLabel         = new QLabel(faceDetectionPage);
    scanAgainLabel->setWordWrap(true);
    scanAgainLabel->setText(i18n("Choose this option if the results "
                                 "of the previous scan weren't accurate. "
                                 "This would rescan all images again, and is time consuming."));

    QLabel* resultsDetectedLabel   = new QLabel(faceDetectionPage);
    resultsDetectedLabel->setWordWrap(true);
    resultsDetectedLabel->setText(i18n("After the Face Scan is complete, you should see a new "
                                       "tag <u>Unknown</u> where the results of the Face Scan "
                                       "will appear. You may now manually identify a few of these "
                                       "faces and then proceed for Face Recognition."));

    faceDetectionPageLayout->addWidget(detectFaceOptionLabel,       0, 0, 1, 2);
    faceDetectionPageLayout->addWidget(detectFaceInfoLabel,         0, 2, 1, 2);
    faceDetectionPageLayout->addWidget(detectIntroLabel,            1, 0, 1, 4);
    faceDetectionPageLayout->addWidget(skipScannedHeaderLabel,      2, 0, 1, 2);
    faceDetectionPageLayout->addWidget(skipScannedLabel,            2, 2, 1, 2);
    faceDetectionPageLayout->addWidget(scanAgainHeaderLabel,        3, 0, 1, 2);
    faceDetectionPageLayout->addWidget(scanAgainLabel,              3, 2, 1, 2);
    faceDetectionPageLayout->addWidget(resultsDetectedLabel,        4, 0, 1, 4);
    faceDetectionPageLayout->setRowStretch(2, 1);
    faceDetectionPageLayout->setRowStretch(3, 1);

    faceDetectionPage->setLayout(faceDetectionPageLayout);

    tabWidget->addTab(faceDetectionPage, i18n("Face Detection"));

    // --- Face Recognition Page --------------------------------------------------------


    // ----------------------------------------------------------------------------------

    mainLayout->addWidget(tabWidget, 0, 0);
    setLayout(mainLayout);
    setVisible(true);
}

FaceManagementHelpDialog::~FaceManagementHelpDialog()
{
}
} // namespace Digikam