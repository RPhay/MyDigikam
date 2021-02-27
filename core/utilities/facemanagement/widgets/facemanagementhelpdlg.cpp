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

// Qt includes

#include <QLabel>
#include <QApplication>
#include <QIcon>
#include <QVBoxLayout>
#include <QUrl>
#include <QString>
#include <QTabWidget>
#include <QMovie>
#include <QStandardPaths>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "daboutdata.h"

namespace Digikam
{

FaceManagementHelpDlg::FaceManagementHelpDlg(QWidget* const parent)
    : QDialog(parent)
{
    setModal(false);
    setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    setWindowTitle(i18n("Face Management in digiKam"));

    QGridLayout* const mainLayout = new QGridLayout();
    QTabWidget*  const tabWidget  = new QTabWidget(this);

    // --- Information Page -------------------------------------------------------------

    QWidget* const infoPage = new QWidget();

    QLabel* const logo      = new QLabel(infoPage);
    logo->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(100, 100)));

    QLabel* const header    = new QLabel(infoPage);
    header->setWordWrap(true);
    header->setText(i18n("<font size=\"5\">%1</font><br/><b>Version %2</b>"
                         "<p>%3</p><p><i>%4</i></p>",
                         QApplication::applicationName(),
                         QApplication::applicationVersion(),
                         DAboutData::digiKamSlogan(),
                         DAboutData::digiKamFamily()));

    QLabel* const introText = new QLabel(infoPage);
    introText->setWordWrap(true);
    introText->setText(QLatin1String("<hr>") +
                       i18n("The Face Workflow allows you to Detect and Recognize people "
                            "in your photographs."));

    QLabel* const faceDetectionHeader   = new QLabel(infoPage);
    faceDetectionHeader->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    faceDetectionHeader->setWordWrap(false);
    faceDetectionHeader->setText(i18n("<b>Face Detection</b>"));

    QLabel* const faceDetection         = new QLabel(infoPage);
    faceDetection->setWordWrap(true);
    faceDetection->setText(i18n("Face Detection allows digiKam to detect faces in your images. "
                                "This step is required before being able to recognize faces."));

    QLabel* const faceRecognitionHeader = new QLabel(infoPage);
    faceRecognitionHeader->setWordWrap(false);
    faceRecognitionHeader->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    faceRecognitionHeader->setText(i18n("<b>Face Recognition</b>"));

    QLabel* const faceRecognition       = new QLabel(infoPage);
    faceRecognition->setWordWrap(true);
    faceRecognition->setText(i18n("Face Recognition will automatically recognize people in your images."));

    // Link to digiKam documentation

    QString documentationLink = i18n(
            "https://docs.kde.org/?application=digikam&branch=trunk5&path=using-digikam.html#using-mainwindow-peopleview");

    QLabel* const footerText = new QLabel(infoPage);
    footerText->setOpenExternalLinks(true);
    footerText->setAlignment(Qt::AlignHCenter);
    footerText->setWordWrap(false);
    footerText->setText(i18n("For additional information, read the %1 \n"
                             "To ask questions, use our mailing list: digikam-users@kde.org \n",
                             QString::fromLatin1("<a href='%1'>digiKam Online Handbook</a>").arg(documentationLink)));

    QGridLayout* const infoPageLayout = new QGridLayout();
    infoPageLayout->addWidget(logo,                  0, 0, 1, 1);
    infoPageLayout->addWidget(header,                0, 1, 1, 1);
    infoPageLayout->addWidget(introText,             1, 0, 1, 2);
    infoPageLayout->addWidget(faceDetectionHeader,   2, 0, 1, 1);
    infoPageLayout->addWidget(faceDetection,         2, 1, 1, 1);
    infoPageLayout->addWidget(faceRecognitionHeader, 3, 0, 1, 1);
    infoPageLayout->addWidget(faceRecognition,       3, 1, 1, 1);
    infoPageLayout->addWidget(footerText,            4, 0, 1, 2);
    infoPageLayout->setRowStretch(1, 1);
    infoPageLayout->setRowStretch(2, 1);
    infoPageLayout->setRowStretch(3, 1);

    infoPage->setLayout(infoPageLayout);

    tabWidget->addTab(infoPage, i18n("Info"));

    // --- Face Detection Page ----------------------------------------------------------

    QWidget* const     faceDetectionPage       = new QWidget();
    QGridLayout* const faceDetectionPageLayout = new QGridLayout();

    QPixmap detectFaceOptionImage  = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                             QLatin1String("digikam/data/detectFacesImg.png")));
    detectFaceOptionImage          = detectFaceOptionImage.scaled(QSize(350, 170));

    QLabel* const detectFaceOptionLabel = new QLabel(faceDetectionPage);
    detectFaceOptionLabel->setPixmap(detectFaceOptionImage);

    QLabel* const detectFaceInfoLabel   = new QLabel(faceDetectionPage);
    detectFaceInfoLabel->setWordWrap(true);
    detectFaceInfoLabel->setText(i18n("To run face detection, select the "
                                      "<u>Detect Faces</u> option in the Face Scan panel "
                                      "and then click on <u>Scan Collection for Faces</u>") +
                                 QLatin1String("<hr>") +
                                 i18n("<b>Caution:</b> Running Face Detection for the first time "
                                      "can be fairly time consuming. Make sure you use the "
                                      "<u>Search In</u> tab to restrict the target of the scan."));

    QLabel* const detectIntroLabel       = new QLabel(faceDetectionPage);
    detectIntroLabel->setWordWrap(true);
    detectIntroLabel->setText(i18n("digiKam offers two kinds of face detection scans: "));

    QLabel* const skipScannedHeaderLabel = new QLabel(faceDetectionPage);
    skipScannedHeaderLabel->setWordWrap(true);
    skipScannedHeaderLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    skipScannedHeaderLabel->setText(i18n("<b>Skip Images Already Scanned</b>"));

    QLabel* const skipScannedLabel       = new QLabel(faceDetectionPage);
    skipScannedLabel->setWordWrap(true);
    skipScannedLabel->setText(i18n("Choose this option if you haven't scanned yet or "
                                   "if the last scan yielded good results but you would "
                                   "like to scan new images."));

    QLabel* const scanAgainHeaderLabel   = new QLabel(faceDetectionPage);
    scanAgainHeaderLabel->setWordWrap(true);
    scanAgainHeaderLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    scanAgainHeaderLabel->setText(i18n("<b>Scan Again and Merge</b>"));

    QLabel* const scanAgainLabel         = new QLabel(faceDetectionPage);
    scanAgainLabel->setWordWrap(true);
    scanAgainLabel->setText(i18n("Choose this option if the results "
                                 "of the previous scan weren't accurate. "
                                 "This would rescan all images again, and is time consuming."));

    QLabel* const resultsDetectedLabel   = new QLabel(faceDetectionPage);
    resultsDetectedLabel->setWordWrap(true);
    resultsDetectedLabel->setText(i18n("After the Face Scan is complete, you should see a new "
                                       "tag <u>Unknown</u> where the results of the face scan "
                                       "will appear. You may now manually identify a few of these "
                                       "faces and then proceed for face recognition."));

    faceDetectionPageLayout->addWidget(detectFaceOptionLabel,  0, 0, 1, 2);
    faceDetectionPageLayout->addWidget(detectFaceInfoLabel,    0, 2, 1, 2);
    faceDetectionPageLayout->addWidget(detectIntroLabel,       1, 0, 1, 4);
    faceDetectionPageLayout->addWidget(skipScannedHeaderLabel, 2, 0, 1, 2);
    faceDetectionPageLayout->addWidget(skipScannedLabel,       2, 2, 1, 2);
    faceDetectionPageLayout->addWidget(scanAgainHeaderLabel,   3, 0, 1, 2);
    faceDetectionPageLayout->addWidget(scanAgainLabel,         3, 2, 1, 2);
    faceDetectionPageLayout->addWidget(resultsDetectedLabel,   4, 0, 1, 4);
    faceDetectionPageLayout->setRowStretch(2, 1);
    faceDetectionPageLayout->setRowStretch(3, 1);

    faceDetectionPage->setLayout(faceDetectionPageLayout);

    tabWidget->addTab(faceDetectionPage, i18n("Face Detection"));

    // --- Face Recognition Page --------------------------------------------------------

    QWidget* const faceRecogPage       = new QWidget();
    QGridLayout* const faceRecogLayout = new QGridLayout();


    QLabel* const identifyLabel        = new QLabel(faceRecogPage);
    QMovie* const identifyImage        = new QMovie(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                    QLatin1String("digikam/data/tagFacesImg.gif")));
    identifyImage->setScaledSize(QSize(185, 220));
    identifyLabel->setMovie(identifyImage);
    identifyImage->start();
    identifyLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* const identifyTextLabel    = new QLabel(faceRecogPage);
    identifyTextLabel->setWordWrap(true);
    identifyTextLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    identifyTextLabel->setText(i18n("To manually identify a face, hover on any Unknown "
                                    "face and use the text box to give it a name. "
                                    "This will lead to the creation of a face tag "
                                    "in case a person of that name doesn't exist already. "
                                    "For best results identify 4-5 faces for each "
                                    "person you want the scan to recognize."));

    QLabel* const recogLabel      = new QLabel(faceRecogPage);
    recogLabel->setWordWrap(true);
    recogLabel->setText(i18n("Running face recognition is similar to detection, "
                             "just select <u>Recognize Faces</u> from the "
                             "Face Scan panel this time.\n"
                             "Face recognition will work on all Unknown faces. In case "
                             "there's a face you don't wish to be recognized, "
                             "you can mark it as ignored.\n"
                             "This leads to the creation of a new <u>Ignored</u> tag, "
                             "you can later unmark the face if needed."));

    QPixmap ignoreImage           = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                            QLatin1String("digikam/data/ignoreImg.png")));
    ignoreImage                   = ignoreImage.scaled(QSize(185, 230));

    QLabel* const ignoreLabel     = new QLabel(faceRecogPage);
    ignoreLabel->setPixmap(ignoreImage);

    QLabel* const faceRecogFooter = new QLabel(faceRecogPage);
    faceRecogFooter->setWordWrap(true);
    faceRecogFooter->setText(i18n("You may select <u>Work on all processor cores</u>"
                                  "in order to increase the speed of Face Recognition \n"
                                  "The Face Accuracy slider allows you to control the "
                                  "accuracy of the recognition algorithm. A higher specificity "
                                  "implies higher accuracy, however this results in a smaller set "
                                  "of results."));

    QPixmap workOnAllCoresImage  = QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                           QLatin1String("digikam/data/workOnAllCoresImg.png")));
    workOnAllCoresImage          = workOnAllCoresImage.scaled(QSize(445, 170));

    QLabel* const workOnAllCoresLabel = new QLabel(faceRecogPage);
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

    QWidget* const confirmFacesPage       = new QWidget();
    QGridLayout* const confirmFacesLayout = new QGridLayout();

    QLabel* const overlayImgLabel = new QLabel(confirmFacesPage);
    QPixmap overlayImg(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                              QLatin1String("digikam/data/overlayImg.png")));
    overlayImg                    = overlayImg.scaled(QSize(210, 285));
    overlayImgLabel->setAlignment(Qt::AlignVCenter);
    overlayImgLabel->setPixmap(overlayImg);

    QLabel* const overlayText     = new QLabel(confirmFacesPage);
    overlayText->setWordWrap(true);
    overlayText->setText(i18n("The results of facial recognition appear in the form "
                              "of Unconfirmed results. These results will show up in "
                              "the Unconfirmed tag, as well as in the tag of the person "
                              "suggested for each face.\n"
                              "Hovering over Unconfirmed faces will display a few options:\n"));
    overlayText->setAlignment(Qt::AlignTop);

    QLabel* const confirmIcon    = new QLabel(confirmFacesPage);
    confirmIcon->setPixmap(QIcon::fromTheme(QLatin1String("dialog-ok-apply")).pixmap(QSize(20, 20)));
    confirmIcon->setAlignment(Qt::AlignTop);

    QLabel* const confirmText    = new QLabel(confirmFacesPage);
    confirmText->setWordWrap(true);
    confirmText->setText(i18n("The Confirm button allows you to confirm the suggestion. "
                              "This would assign the suggested name to the face"));
    confirmText->setAlignment(Qt::AlignTop);

    QLabel* const rejectIcon     = new QLabel(confirmFacesPage);
    rejectIcon->setPixmap(QIcon::fromTheme(QLatin1String("list-remove")).pixmap(QSize(20, 20)));
    rejectIcon->setAlignment(Qt::AlignTop);

    QLabel* const rejectText     = new QLabel(confirmFacesPage);
    rejectText->setWordWrap(true);
    rejectText->setText(i18n("Use the Reject button, if the suggestion is incorrect. "
                             "This would move the face back to Unknown."));
    rejectText->setAlignment(Qt::AlignTop);


    QLabel* const deleteIcon     = new QLabel(confirmFacesPage);
    deleteIcon->setPixmap(QIcon::fromTheme(QLatin1String("window-close")).pixmap(QSize(20, 20)));
    deleteIcon->setAlignment(Qt::AlignTop);

    QLabel* const deleteText     = new QLabel(confirmFacesPage);
    deleteText->setWordWrap(true);
    deleteText->setText(i18n("Use the Delete button, if the suggestion is not a face. "
                             "This will delete the face region from the database."));
    deleteText->setAlignment(Qt::AlignTop);

    QLabel* const sortText       = new QLabel(confirmFacesPage);
    sortText->setWordWrap(true);
    sortText->setText(i18n("<b>Face Categorization and Sorting</b>\n"
                           "By default, faces will appear categorized based on their "
                           "name. You may change this behaviour, by going to "
                           "<u>View -> Separate Items</u>.\n"
                           "Note that using a different categorization order will "
                           "lead to Confirmed and Unconfirmed faces appearing "
                           "mixed with one another. To change this behaviour "
                           "you may modify the sorting order within each category "
                           "by going to <u>View -> Sort Items -> By Face Type</u>."));

    QLabel* const improveResults = new QLabel(confirmFacesPage);
    improveResults->setWordWrap(true);
    improveResults->setText(i18n("<b>Improving Results of Face Recognition</b>\n"
                                 "Apart from using the settings mentioned in the last page, "
                                 "face results may also be improved by rebuilding the training data. "
                                 "This may be done through Tools -> Maintenance -> Detect and Recognize Faces "
                                 "-> Clear and rebuild all training data."));
    improveResults->setAlignment(Qt::AlignTop);

    confirmFacesLayout->addWidget(overlayImgLabel, 0, 0, 4, 1);
    confirmFacesLayout->addWidget(overlayText,     0, 1, 1, 7);
    confirmFacesLayout->addWidget(confirmIcon,     1, 1, 1, 1);
    confirmFacesLayout->addWidget(confirmText,     1, 2, 1, 6);
    confirmFacesLayout->addWidget(rejectIcon,      2, 1, 1, 1);
    confirmFacesLayout->addWidget(rejectText,      2, 2, 1, 6);
    confirmFacesLayout->addWidget(deleteIcon,      3, 1, 1, 1);
    confirmFacesLayout->addWidget(deleteText,      3, 2, 1, 6);
    confirmFacesLayout->addWidget(sortText,        4, 0, 1, 8);
    confirmFacesLayout->addWidget(improveResults,  5, 0, 1, 8);

    confirmFacesPage->setLayout(confirmFacesLayout);

    tabWidget->addTab(confirmFacesPage, i18n("Confirm Faces"));

    // ----------------------------------------------------------------------------------

    mainLayout->addWidget(tabWidget, 0, 0);
    setLayout(mainLayout);
}

FaceManagementHelpDlg::~FaceManagementHelpDlg()
{
}

} // namespace Digikam
