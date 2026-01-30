/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-12-20
 * Description : a widget to display a welcome page
 *               on root album.
 *
 * SPDX-FileCopyrightText: 2006-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "welcomepageview.h"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QPushButton>
#include <QButtonGroup>
#include <QApplication>
#include <QStandardPaths>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QStackedWidget>
#include <QStylePainter>
#include <QStyleOptionTab>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_version.h"
#include "daboutdata.h"
#include "thememanager.h"
#include "webbrowserdlg.h"

namespace Digikam
{

class Q_DECL_HIDDEN GradientWidget : public QWidget
{
public:

    GradientWidget(QWidget* const parent = nullptr)
        : QWidget(parent)
    {
    }

protected:

    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0,    QColor("#223c54"));
        gradient.setColorAt(0.55, QColor("#01080F"));
        painter.fillRect(rect(), gradient);
    }
};

// ---

class Q_DECL_HIDDEN ResizableBackgroundWidget : public QWidget
{
    Q_OBJECT

public:

    ResizableBackgroundWidget(QWidget* const parent = nullptr)
        : QWidget(parent),
          backgroundLabel(new QLabel(this))
    {
        backgroundLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        backgroundLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

        QVBoxLayout* const layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(backgroundLabel);
    }

    void setBackgroundPixmap(const QPixmap& pixmap)
    {
        backgroundPixmap = pixmap;
        updateBackground();
    }

protected:

    void resizeEvent(QResizeEvent* event) override
    {
        QWidget::resizeEvent(event);
        updateBackground();
    }

private:

    void updateBackground()
    {
        if (!backgroundPixmap.isNull())
        {
            QPixmap scaledPixmap = backgroundPixmap.scaledToWidth(width(), Qt::SmoothTransformation);
            backgroundLabel->setPixmap(scaledPixmap);
        }
    }

    QLabel* backgroundLabel = nullptr;
    QPixmap backgroundPixmap;
};

// ---

WelcomePageView::WelcomePageView(QWidget* const parent)
    : QWidget(parent)
{
    QScrollArea* const sv = new QScrollArea(this);
    sv->setFrameStyle(QFrame::NoFrame);
    sv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sv->setWidgetResizable(true);

    QWidget* const plain = new QWidget(sv->viewport());
    plain->setMinimumWidth(sv->width());

    ResizableBackgroundWidget* const background = new ResizableBackgroundWidget(plain);
    QPixmap backgroundPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                    QLatin1String("digikam/about/images/body-background.webp")));
    background->setBackgroundPixmap(backgroundPixmap);

    GradientWidget* const grad  = new GradientWidget(plain);

    QWidget* const headerWidget = new QWidget(plain);
    QLabel* const logo          = new QLabel(headerWidget);
    logo->setPixmap(QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                   QLatin1String("digikam/data/logo-digikam.webp")))
                    .scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel* const title         = new QLabel(QLatin1String("<qt><h1>digiKam</h1></qt>"), headerWidget);
    title->setObjectName(QLatin1String("app-name"));
    title->setStyleSheet(QLatin1String(
        "#app-name {"
        "   color: rgba(255, 255, 255, 0.67);"
        "   margin-top: 11px;"
        "}"
    ));

    QGraphicsDropShadowEffect* const effect = new QGraphicsDropShadowEffect;
    effect->setColor(Qt::white);
    effect->setBlurRadius(35);
    effect->setOffset(0, 0);
    title->setGraphicsEffect(effect);

    QHBoxLayout* const headerLayout         = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 10, 10, 10);
    headerLayout->addWidget(logo);
    headerLayout->addStretch();
    headerLayout->addWidget(title);

    QWidget* const titleWidget              = new QWidget(plain);

    QLabel* const smallTitle                = new QLabel(DAboutData::digiKamSlogan(), titleWidget);
    smallTitle->setAlignment(Qt::AlignCenter);
    smallTitle->setStyleSheet(QLatin1String("color: black; font-size: 14px;"));

    QLabel* const bigTitle                  = new QLabel(i18n("Welcome to digiKam %1", QLatin1String(digikam_version)), titleWidget);
    bigTitle->setAlignment(Qt::AlignCenter);
    bigTitle->setStyleSheet(QLatin1String("color: black; font-size: 24px; font-weight: bold;"));

    QVBoxLayout* const titleLayout          = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 20);
    titleLayout->addWidget(smallTitle);
    titleLayout->addWidget(bigTitle);

    // ---

    QWidget* const tabButtonsWidget      = new QWidget(plain);
    QHBoxLayout* const tabButtonsLayout  = new QHBoxLayout(tabButtonsWidget);
    tabButtonsLayout->setContentsMargins(0, 0, 0, 0);
    tabButtonsLayout->setSpacing(10);

    QPushButton* const newFeaturesButton = new QPushButton(i18n("New Features"), tabButtonsWidget);
    QPushButton* const aboutButton       = new QPushButton(i18n("About"), tabButtonsWidget);
    QPushButton* const creditsButton     = new QPushButton(i18n("Background Image Credits"), tabButtonsWidget);

    QString buttonStyle                  = QLatin1String("QPushButton { background-color: rgba(200, 200, 200, 200); border: none; padding: 8px; }"
                                                         "QPushButton:checked { background-color: rgba(150, 150, 150, 200); }");
    newFeaturesButton->setStyleSheet(buttonStyle);
    aboutButton->setStyleSheet(buttonStyle);
    creditsButton->setStyleSheet(buttonStyle);

    // ---

    tabButtonsLayout->addStretch();
    tabButtonsLayout->addWidget(newFeaturesButton);
    tabButtonsLayout->addWidget(aboutButton);
    tabButtonsLayout->addWidget(creditsButton);
    tabButtonsLayout->addStretch();

    // ---

    QButtonGroup* const tabButtonGroup = new QButtonGroup(plain);
    tabButtonGroup->addButton(newFeaturesButton);
    tabButtonGroup->addButton(aboutButton);
    tabButtonGroup->addButton(creditsButton);
    newFeaturesButton->setCheckable(true);
    aboutButton->setCheckable(true);
    creditsButton->setCheckable(true);
    newFeaturesButton->setChecked(true);

    // ---

    QStackedWidget* const stackedWidget = new QStackedWidget(plain);

    QWidget* const newFeaturesTab       = new QWidget(stackedWidget);
    QLabel* const newFeatures           = new QLabel(featuresTabContent(), stackedWidget);
    newFeatures->setStyleSheet(QLatin1String("background: rgba(240, 240, 240, 220); color: black;"));
    newFeatures->setOpenExternalLinks(true);
    QVBoxLayout* const vlay1            = new QVBoxLayout(newFeaturesTab);
    vlay1->addWidget(newFeatures);
    vlay1->addStretch();
    vlay1->setContentsMargins(10, 10, 10, 10);
    stackedWidget->addWidget(newFeaturesTab);

    QWidget* const aboutTab             = new QWidget(stackedWidget);
    QLabel* const about                 = new QLabel(aboutTabContent(), stackedWidget);
    about->setStyleSheet(QLatin1String("background: rgba(240, 240, 240, 220); color: black;"));
    about->setOpenExternalLinks(true);
    QVBoxLayout* const vlay2            = new QVBoxLayout(aboutTab);
    vlay2->addWidget(about);
    vlay2->addStretch();
    vlay2->setContentsMargins(10, 10, 10, 10);
    stackedWidget->addWidget(aboutTab);

    QWidget* const creditsTab           = new QWidget(stackedWidget);
    QLabel* const credits               = new QLabel(creditsTabContent(), stackedWidget);
    credits->setStyleSheet(QLatin1String("background: rgba(240, 240, 240, 220); color: black;"));
    credits->setOpenExternalLinks(true);
    QVBoxLayout* const vlay3            = new QVBoxLayout(creditsTab);
    vlay3->addWidget(credits);
    vlay3->addStretch();
    vlay3->setContentsMargins(10, 10, 10, 10);
    stackedWidget->addWidget(creditsTab);

    connect(newFeaturesButton, &QPushButton::clicked,
            [stackedWidget]()
        {
            stackedWidget->setCurrentIndex(0);
        }
    );

    connect(aboutButton, &QPushButton::clicked,
            [stackedWidget]()
        {
            stackedWidget->setCurrentIndex(1);

        }
    );

    connect(creditsButton, &QPushButton::clicked,
            [stackedWidget]()
        {
            stackedWidget->setCurrentIndex(2);
        }
    );

    QGridLayout* const grid = new QGridLayout(plain);
    grid->addWidget(grad,             0, 0, 1, 3);
    grid->addWidget(headerWidget,     0, 0, 1, 3);
    grid->addWidget(background,       1, 0, 3, 3);
    grid->addWidget(titleWidget,      1, 0, 1, 3, Qt::AlignCenter);
    grid->addWidget(tabButtonsWidget, 2, 0, 1, 3, Qt::AlignCenter);
    grid->addWidget(stackedWidget,    3, 0, 1, 3);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(3, 10);

    sv->setWidget(plain);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(sv);
}

QString WelcomePageView::featuresTabContent() const
{
    QStringList newFeatures;
    newFeatures << i18n("More than <a href=\"https://bugs.kde.org/buglist.cgi?f1=cf_versionfixedin&limit=0&o1=equals&order=bug_id&product=digikam&v1=%1\">350 user-reports</a> closed on bugzilla.", QLatin1String(digikam_version_short));
    newFeatures << i18n("Internal QtAVPlayer has been updated to the latest upstream 2026-01-24 which brings with it a variety of bug fixes and performance improvements (Qt5 only).");
    newFeatures << i18n("Add support of customizable date format to show everywhere in the program.");
    newFeatures << i18n("Color label names are now customizable in the Labels tab from left sidebar by double clicking on text. Label strings size are limited to 32 characters.");
    newFeatures << i18n("Add support of Album Icon-View sort items by Pick and Color Labels.");
    newFeatures << i18n("Add support of Import Icon-View sort items by Pick and Color Labels.");
    newFeatures << i18n("Add support of Album Icon-View sort items by Orientation.");
    newFeatures << i18n("Add support of Album Icon-View sort items by GPS location.");
    newFeatures << i18n("Add support of Album Icon-View sort items by File Format.");
    newFeatures << i18n("Add support of Import Icon-View sort items by File Format.");
    newFeatures << i18n("Add regular expression wildcard support for ignored directories settings.");
    newFeatures << i18n("Add new button to copy all properties as text in clipboard.");
    newFeatures << i18n("Add new button to open current item is file manager.");
    newFeatures << i18n("If no icon-view item selected, show current album statistics.");
    newFeatures << i18n("Show items from an open group highlighted for a better usability with Icon-View.");
    newFeatures << i18n("Usability improvements of the grouped icon-view items overlay.");
    newFeatures << i18n("Thumbbar improvements to support grouped items visibility as with Icon-View.");
    newFeatures << i18n("Split filters view into 3 separated tabs for better usabilty: Properties, Tags, and People.");
    newFeatures << i18n("Load and save Trash-view column states between sessions.");
    newFeatures << i18n("Extend Image and Video preview toolbar overlay with Labels editor.");
    newFeatures << i18n("Add new Preview toolbar buttons: exposure indicators, color management, OSD, magnifier.");
    newFeatures << i18n("Add context-menu to the video player.");
    newFeatures << i18n("Add load progress indicator on the status bar (Icon-view and LightTable).");
    newFeatures << i18n("Add trackpad gesture events support to zoom the contents, pan the canvas, and change to previous/next items.");
    newFeatures << i18n("Add a magnifier glass tool to zoom and explore a small area of the preview canvas. Tool Size and zoom factor are customizable.");
    newFeatures << i18n("Add under and over exposure indicators for the preview canvas.");
    newFeatures << i18n("Railway over the canvas is displayed as an overlay when the scrollbar are visible.");
    newFeatures << i18n("Clicking on GPS icon overlay open the Geolocation tab from the left sidebar.");
    newFeatures << i18n("Add customizable columns to show Grouped and Versioned properties.");
    newFeatures << i18n("New Advanced Rename modifier Date Format.");
    newFeatures << i18n("Add support to merge existing template entries assigned in items with a new template.");
    newFeatures << i18n("Add new Slideshow option to customize the color background instead pure black in case of images transparency usage.");
    newFeatures << i18n("Add new Presentation option to customize the color background instead pure black in case of images transparency usage.");
    newFeatures << i18n("Complete rewrite the File-Tranfert tool GUI for a better usability.");
    newFeatures << i18n("Complete rewrite the File-Copy tool GUI for a better usability.");
    newFeatures << i18n("Add marching-ants effect to the canvas selection rubber band.");
    newFeatures << i18n("Add pick color histogram indicators for the Curves, Levels, White Balance, and Black & White Sepia tools.");
    newFeatures << i18n("Huge web site update about contents and design eg. screenshots, description, history, features, support, download, etc.");
    newFeatures << i18n("Update internal RAW engine to last Libraw snapshot 2026-01-24.");
    newFeatures << i18n("New RAW camera supported:"
                        "<ul>Canon EOS R1, EOS R5 Mark II, EOS R5 C, EOS R6 Mark II, EOS R8, EOS R50, EOS R100, EOS Ra ; </ul>"
                        "<ul>Fujifilm X-T50, GFX 100S II, GFX100-II, X-T5, X-S20, X-H2, X-H2S ; </ul>"
                        "<ul>Hasselblad  CFV-50c, CFV-100c, X2D-100c ; </ul>"
                        "<ul>Leica Q3 43, D-Lux8, SL3, Q3, M11 Monochrom ; </ul>"
                        "<ul>Nikon (standard compression only): Z6-III, Z f, Z30, Z8 ; </ul>"
                        "<ul>Olympus/OM System OM-1 Mark II, TG-7, OM-5 ; </ul>"
                        "<ul>Panasonic GH7, S9, DC-G9 II, DC-ZS200D / ZS220D, DC-TZ200D / TZ202D / TZ220D, DC-S5-II, DC-GH6 ; </ul>"
                        "<ul>Pentax KF, K III Monochrome ; </ul>"
                        "<ul>Sony ZV-E10M2, UMC-R10C, A9-III, ILX-LR1, A7C-II, A7CR, ILCE-6700,  ZV-1M2, ZV-E1, ILCE-7RM5 (A7R-V), ILME-FX30, A1 ; </ul>"
                        "<ul>Multiple DJI and Skydio drones ; </ul>"
                        "<ul>Multiple smartphones with DNG format recorded.</ul>"
                       );

    // Add new features here...

    newFeatures << i18n("...and much more.");

    QString featureItems;

    for (int i = 0 ; i < newFeatures.count() ; ++i)
    {
        featureItems += i18n("<li>%1</li>", newFeatures.at(i));
    }

    QString tabContent = i18n("<h3>Some of the new features in this release of digiKam include (compared to digiKam 8):</h3><ul>%1</ul>",
                         featureItems);

    return tabContent;
}

QString WelcomePageView::aboutTabContent() const
{
    QString tabContent =
       i18n("<h3>"
            "digiKam is an open source photo management program designed to process your digital images on your computer. "
            "With digiKam you can:"
            "<li>import,</li>"
            "<li>organize,</li>"
            "<li>enhance,</li>"
            "<li>search,</li>"
            "<li>export,<li>"
            "<li>and more...<li>"
            "</h3>"
            "<h3>Currently, you are in the Album view mode of digiKam.</h3>"
            "<h3>Albums are the places where your files are stored, and are identical to the folders on your hard disk.</h3>"
            "<li>digiKam has many powerful features which are described in the <a href=\"https://docs.digikam.org/en/index.html\">documentation</a></li>"
            "<li>The <a href=\"https://www.digikam.org\">digiKam homepage</a> provides information about new versions of digiKam.</li>");

    return tabContent;
}

QString WelcomePageView::creditsTabContent()  const
{
    QString creditsTab =
        QString::fromUtf8(
                "<table>"
                  "<tr>"
                    "<td width=\"10%\"><h4>%1</h4></td>"
                    "<td><h4>%2</h4></td>"
                  "</tr>"
                  "<tr>"
                    "<td width=\"10%\"><h4>%3</h4></td>"
                    "<td><h4>%4</h4></td>"
                  "</tr>"
                  "<tr>"
                    "<td width=\"10%\"><h4>%5</h4></td>"
                    "<td><h4>%6</h4></td>"
                  "</tr>"
                  "<tr>"
                    "<td width=\"10%\"><h4>%7</h4></td>"
                    "<td><h4>%8</h4></td>"
                  "</tr>"
                  "<tr>"
                    "<td width=\"10%\"><h4>%9</h4></td>"
                    "<td><h4>%10</h4></td>"
                  "</tr>"
                "</table>"
        ).arg(i18n("Author:"))  .arg(QString::fromUtf8("Andreas K. Huettel"))
         .arg(i18n("Location:")).arg(QString::fromUtf8("<a href=\"https://en.wikipedia.org/wiki/Kjem%C3%A5vatnet\">Kjemåvatnet - Nordland county - Norway.</a>"))
         .arg(i18n("Date:"))    .arg(QString::fromUtf8("August 2020"))
         .arg(i18n("Camera:"))  .arg(QString::fromUtf8("Canon EOS 7D Mark II"))
         .arg(i18n("Lens:"))    .arg(QString::fromUtf8("Sigma 18-250mm f/3.5-6.3 DC OS HSM"))
    ;

    return creditsTab;
}

} // namespace Digikam

#include "welcomepageview.moc"

#include "moc_welcomepageview.cpp"
