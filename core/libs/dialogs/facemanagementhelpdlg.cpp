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
    // should i set this to true? I'm allowing creation of multiple windows.
    setModal(false);
    setWindowTitle(i18n("Face Management in DigiKam"));

    QGridLayout* mainLayout = new QGridLayout();

    QLabel* const logo      = new QLabel();

    if (QApplication::applicationName() == QLatin1String("digikam"))
    {
        logo->setPixmap(QIcon::fromTheme(QLatin1String("digikam")).pixmap(QSize(48,48)));
    }
    else
    {
        logo->setPixmap(QIcon::fromTheme(QLatin1String("showfoto")).pixmap(QSize(48,48)));
    }

    // --------------------------------------------------------

    QLabel* const header    = new QLabel();
    header->setWordWrap(true);
    header->setText(i18n("<font size=\"5\">%1</font><br/><b>Version %2</b>"
                         "<p>%3</p><p><i>%4</i></p>",
                         QApplication::applicationName(),
                         QApplication::applicationVersion(),
                         DAboutData::digiKamSlogan(),
                         DAboutData::digiKamFamily()));

    QLabel* introText = new QLabel();
    introText->setWordWrap(true);
    introText->setText(i18n("<hr>The Face Workflow allows you to Detect and Recognize people "
                        "in your photographs."));

    QLabel* const faceDetectionHeader = new QLabel();
    faceDetectionHeader->setWordWrap(false);
    faceDetectionHeader->setText(i18n("<b>Face Detection</b>"));

    QLabel* const faceDetection = new QLabel();
    faceDetection->setWordWrap(true);
    faceDetection->setText(i18n("Face Detection allows DigiKam to detect faces in your images. "
                                "This step is required before being able to recognize faces. "
                                "Use the \"Search In\" tab "
                                "to restrict the scan to particular folders."));

    QLabel* const faceRecognitionHeader = new QLabel();
    faceRecognitionHeader->setWordWrap(false);
    faceRecognitionHeader->setText(i18n("<b>Face Recognition</b>"));

    QLabel* const faceRecognition = new QLabel();
    faceRecognition->setWordWrap(true);
    faceRecognition->setText(i18n("Face Recognition will automatically recognize people in your images. "
                                  "To use this, first manually identify a few Faces detected during "
                                  "Face Detection. These faces should appear in the Unknown Category. "));

    QString documentationLink = i18n(
            "https://docs.kde.org/trunk5/en/extragear-graphics/digikam/using-digikam.html#using-mainwindow-peopleview");

    QLabel* const footerText = new QLabel();
    footerText->setOpenExternalLinks(true);
    footerText->setWordWrap(false);
    footerText->setText(i18n("<hr>For additional information, read the %1 \n"
                             "<br>To ask questions, use our mailing list: digikam-users@kde.org \n",
                             QString::fromLatin1("<a href='%1'>digiKam Online Handbook</a>").arg(documentationLink)));

    mainLayout->addWidget(logo, 0, 0, 1, 1);
    mainLayout->addWidget(header, 0, 1, 1, 1);
    mainLayout->addWidget(introText, 1, 0, 1, 2);
    mainLayout->addWidget(faceDetectionHeader,2, 0, 1, 1);
    mainLayout->addWidget(faceDetection,2, 1, 1, 1);
    mainLayout->addWidget(faceRecognitionHeader,3, 0, 1, 1);
    mainLayout->addWidget(faceRecognition,3, 1, 1, 1);
    mainLayout->addWidget(footerText, 4,0, 1, 2);
    setLayout(mainLayout);
    setVisible(true);
}

FaceManagementHelpDialog::~FaceManagementHelpDialog()
{
}
} // namespace Digikam