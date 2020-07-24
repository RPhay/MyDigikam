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
#include <QMovie>
#include <QStandardPaths>

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

    QPixmap detectFaceOptionImage = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/detectFacesImg.png")));
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

    QWidget*     faceRecogPage   = new QWidget();
    QGridLayout* faceRecogLayout = new QGridLayout();


    QLabel* identifyLabel        = new QLabel(faceRecogPage);
    QMovie* identifyImage        = new QMovie(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/tagFacesImg.gif")));
    identifyImage->setScaledSize(QSize(185,220));
    identifyLabel->setMovie(identifyImage);
    identifyImage->start();
    identifyLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* identifyTextLabel    = new QLabel(faceRecogPage);
    identifyTextLabel->setWordWrap(true);
    identifyTextLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    identifyTextLabel->setText(i18n("To manually identify a face, hover on any Unknown "
                                    "face and use the Text box to give it a name."
                                    "This will lead to the creation of a Face Tag "
                                    "in case a person of that name doesn't exist already."
                                    "For best results identify 4-5 faces for each "
                                    "person you want the scan to recognize.<br>"
                                    ));

    QLabel* recogLabel      = new QLabel(faceRecogPage);
    recogLabel->setWordWrap(true);
    recogLabel->setText(i18n("Running Face Recognition is similar to Detection, "
                             "just select <u>Recognize Faces</u> from the "
                             "Face Scan Panel this time.<br>"
                             "Face Recognition will work on all Unknown Faces. In case "
                             "there's a face you don't wish to be recognized, "
                             "you can mark it as ignored.<br>"
                             "This leads to the creation of a new <u>Ignored</u> Tag, "
                             "you can later unmark the Face if needed."));

    QPixmap ignoreImage          = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/ignoreImg.png")));
    ignoreImage                  = ignoreImage.scaled(QSize(185,230));

    QLabel* ignoreLabel          = new QLabel(faceRecogPage);
    ignoreLabel->setPixmap(ignoreImage);

    QLabel* faceRecogFooter      = new QLabel(faceRecogPage);
    faceRecogFooter->setWordWrap(true);
    faceRecogFooter->setText(i18n("To increase the speed of Face Recognition, "
                                  "you may check <u>Work on all processor cores</u> "
                                  "from the Settings Tab. You can also modify Recognition "
                                  "Accuracy, to find a balance between Accuracy and Speed."));

    QPixmap workOnAllCoresImage  = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/workOnAllCoresImg.png")));
    workOnAllCoresImage          = workOnAllCoresImage.scaled(QSize(445,170));

    QLabel* workOnAllCoresLabel  = new QLabel(faceRecogPage);
    workOnAllCoresLabel->setPixmap(workOnAllCoresImage);

    faceRecogLayout->addWidget(identifyLabel,       0, 0, 2, 1);
    faceRecogLayout->addWidget(identifyTextLabel,   0, 1, 2, 3);
    faceRecogLayout->addWidget(recogLabel,          2, 0, 1, 3);
    faceRecogLayout->addWidget(ignoreLabel,         2, 3, 1, 1);
    faceRecogLayout->addWidget(workOnAllCoresLabel, 3, 0, 1, 2);
    faceRecogLayout->addWidget(faceRecogFooter,     3, 2, 1, 2);

    faceRecogPage->setLayout(faceRecogLayout);

    tabWidget->addTab(faceRecogPage, i18n("Face Recognition"));

    // --- Confirm Faces ----------------------------------------------------------------

    QWidget*     confirmFacesPage   = new QWidget();
    QGridLayout* confirmFacesLayout = new QGridLayout();

    QLabel* overlayImgLabel = new QLabel(confirmFacesPage);
    QPixmap overlayImg(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/overlayImg.png")));
    overlayImg = overlayImg.scaled(QSize(210, 285));
    overlayImgLabel->setAlignment(Qt::AlignTop);
    overlayImgLabel->setPixmap(overlayImg);

    QLabel* overlayText     = new QLabel(confirmFacesPage);
    overlayText->setWordWrap(true);
    overlayText->setText(i18n("The results of Facial Recognition appear in the form "
                              "of Unconfirmed Results. These results will show up in "
                              "the Unconfirmed Tag, as well as in the Tag of the person "
                              "suggested for each face. <br>"
                              "Hovering over Unconfirmed Faces will display a few options: <br> "
                              ""));
    overlayText->setAlignment(Qt::AlignTop);

    QLabel* confirmIcon     = new QLabel(confirmFacesPage);
    confirmIcon->setPixmap(QIcon::fromTheme(QLatin1String("dialog-ok-apply")).pixmap(QSize(20,20)));
    confirmIcon->setAlignment(Qt::AlignTop);

    QLabel* confirmText     = new QLabel(confirmFacesPage);
    confirmText->setWordWrap(true);
    confirmText->setText(i18n("The Confirm Button allows you to confirm the suggestion. "
                              "This would assign the suggested name to the face"));
    confirmText->setAlignment(Qt::AlignTop);

    QLabel* rejectIcon      = new QLabel(confirmFacesPage);
    rejectIcon->setPixmap(QIcon::fromTheme(QLatin1String("list-remove")).pixmap(QSize(20,20)));
    rejectIcon->setAlignment(Qt::AlignTop);

    QLabel* rejectText      = new QLabel(confirmFacesPage);
    rejectText->setWordWrap(true);
    rejectText->setText(i18n("Use the Reject Button, if the suggestion is incorrect. "
                             "This would move the Face back to Unknown."));
    rejectText->setAlignment(Qt::AlignTop);


    QLabel* deleteIcon      = new QLabel(confirmFacesPage);
    deleteIcon->setPixmap(QIcon::fromTheme(QLatin1String("window-close")).pixmap(QSize(20,20)));
    deleteIcon->setAlignment(Qt::AlignTop);

    QLabel* deleteText      = new QLabel(confirmFacesPage);
    deleteText->setWordWrap(true);
    deleteText->setText(i18n("Use the delete Button, if the suggestion is not a face. "
                             "This will delete the Face Region from the Database."));
    deleteText->setAlignment(Qt::AlignTop);

    QLabel* sortText        = new QLabel(confirmFacesPage);
    sortText->setWordWrap(true);
    sortText->setText(i18n("By default, Faces are sorted by File Names, and hence "
                           "Face Suggestions will appear mixed "
                           "with already Confirmed Results.<br>You can change this "
                           "by modifying the Sort Items Role to \"By Face Type\". "
                           "This can be accessed through the View Menu.<br>"
                           "This allows for batch operations on multiple Faces as shown. <br>"));

    QMovie* sortGif         = new QMovie(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/sortFacesImg.gif")));
    sortGif->setScaledSize(QSize(450, 250));
    QLabel* sortGifLabel    = new QLabel(confirmFacesPage);
    sortGifLabel->setMovie(sortGif);
    sortGif->start();

    confirmFacesLayout->addWidget(overlayImgLabel, 0, 0, 4, 1);
    confirmFacesLayout->addWidget(overlayText,     0, 1, 1, 7);
    confirmFacesLayout->addWidget(confirmIcon,     1, 1, 1, 1);
    confirmFacesLayout->addWidget(confirmText,     1, 2, 1, 6);
    confirmFacesLayout->addWidget(rejectIcon,      2, 1, 1, 1);
    confirmFacesLayout->addWidget(rejectText,      2, 2, 1, 6);
    confirmFacesLayout->addWidget(deleteIcon,      3, 1, 1, 1);
    confirmFacesLayout->addWidget(deleteText,      3, 2, 1, 6);
    confirmFacesLayout->addWidget(sortText,        4, 0, 1, 3);
    confirmFacesLayout->addWidget(sortGifLabel,    4, 3, 1, 5);

    confirmFacesPage->setLayout(confirmFacesLayout);

    tabWidget->addTab(confirmFacesPage, i18n("Confirm Faces"));

    // ----------------------------------------------------------------------------------

    mainLayout->addWidget(tabWidget, 0, 0);
    setLayout(mainLayout);
}

FaceManagementHelpDialog::~FaceManagementHelpDialog()
{
}
} // namespace Digikam